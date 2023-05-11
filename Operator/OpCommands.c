#include "OpCommands.h"
#include "MemoryDLL.h"
#include "Console.h"

#include <tchar.h>

int cmdLoop() {

    // Memória partilhada

    HANDLE file;

    int err = openCommandFile(&file);
    if (err) {
        _tprintf_s(_T("Erro ao criar ficheiro da memória partilhada\n"));
        return 1;
    }

    LPVOID address;
    err = mapCommandSharedFile(file, &address);

    if (err) {
        _tprintf_s(_T("Erro ao mapear a memória partilhada\n"));
        CloseHandle(file);
        return 1;
    }

    // Eventos

    HANDLE commandWriteEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SERVER_COMMAND_WRITE);

    if (commandWriteEvent == NULL) {
        _tprintf_s(_T("O evento %s não foi criado! Código de Erro: %d\n"), SERVER_TICK_EVENT, GetLastError());
        closeSharedFile(&file, &address);
        return 1;
    }

    HANDLE commandReadEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SERVER_COMMAND_READ);

    if (commandReadEvent == NULL) {
        _tprintf_s(_T("O evento %s não foi criado! Código de Erro: %d\n"), SERVER_TICK_EVENT, GetLastError());
        closeSharedFile(&file, &address);
        CloseHandle(commandWriteEvent);
        return 1;
    }

    TCHAR cmd[SHARED_COMMAND_BUFFER_CHARS];

    do {
        setCursorPosition(5, 13);
        _tprintf_s(_T(">"));
        fflush(stdin);
        _fgetts(cmd, SHARED_COMMAND_BUFFER_CHARS, stdin);
        cmd[_tcslen(cmd) - 1] = '\0';
        if (_tcscmp(cmd, _T("sair")) != 0) {
            clearLine(12);
            clearLine(13);
            setCursorPosition(5, 12);
            SetEvent(commandWriteEvent);
            err = WriteCircularBufferChar(address, cmd);
            if (err) {
                _tprintf_s("Ocorreu um erro ao executar um comando!\n");
                return 1;
            }
            SetEvent(commandWriteEvent);
            WaitForSingleObject(commandReadEvent, 1000);
            DWORD error = 0;
            err = ReadCircularBufferDWORD(address, &error);
            if (err) {
                _tprintf_s("Ocorreu um erro ao ler a resposta a um comando!\n");
                return 1;
            }
            if (error) {
                GetCommandErrorSTR(cmd, SHARED_COMMAND_BUFFER_CHARS, error);
                _tprintf_s(_T("%s\n"), cmd);
            }
            ResetEvent(commandWriteEvent);
        }
    } while (_tcscmp(cmd, _T("sair")) != 0);

    closeSharedFile(&file, &address);
    CloseHandle(commandWriteEvent);
    CloseHandle(commandReadEvent);

    return 0;
}