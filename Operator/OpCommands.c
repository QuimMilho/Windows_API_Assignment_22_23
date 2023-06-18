#include "OpCommands.h"
#include "MemoryDLL.h"
#include "Console.h"

#include <tchar.h>

int cmdLoop() {

    // Mem�ria partilhada

    HANDLE file;

    int err = openCommandFile(&file);
    if (err) {
        _tprintf_s(_T("Erro ao criar ficheiro da mem�ria partilhada\n"));
        return 1;
    }

    LPVOID address;
    err = mapCommandSharedFile(file, &address);

    if (err) {
        _tprintf_s(_T("Erro ao mapear a mem�ria partilhada\n"));
        CloseHandle(file);
        return 1;
    }

    // Eventos

    HANDLE commandWriteEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SERVER_COMMAND_WRITE);

    if (commandWriteEvent == NULL) {
        _tprintf_s(_T("O evento %s n�o foi criado! C�digo de Erro: %d\n"), SERVER_TICK_EVENT, GetLastError());
        closeSharedFile(&file, &address);
        return 1;
    }

    HANDLE commandReadEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SERVER_COMMAND_READ);

    if (commandReadEvent == NULL) {
        _tprintf_s(_T("O evento %s n�o foi criado! C�digo de Erro: %d\n"), SERVER_TICK_EVENT, GetLastError());
        closeSharedFile(&file, &address);
        CloseHandle(commandWriteEvent);
        return 1;
    }

    HANDLE semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SERVER_COMMAND_SEMAPHORE);

    if (semaphore == NULL) {
        _tprintf_s(_T("O evento %s n�o foi criado! C�digo de Erro: %d\n"), SERVER_TICK_EVENT, GetLastError());
        closeSharedFile(&file, &address);
        CloseHandle(commandWriteEvent);
        CloseHandle(commandReadEvent);
        return 1;
    }

    TCHAR cmd[SHARED_COMMAND_BUFFER_CHARS];

    DWORD result;

    do {
        setCursorPosition(5, 13);
        _tprintf_s(_T(">"));
        fflush(stdin);
        _fgetts(cmd, SHARED_COMMAND_BUFFER_CHARS, stdin);
        cmd[_tcslen(cmd) - 1] = '\0';
        if (_tcscmp(cmd, _T("sair")) != 0) {
            clearLine(13);
            setCursorPosition(5, 13);
            result = WaitForSingleObject(semaphore, 5000);
            if (result != WAIT_OBJECT_0) {
                _tprintf_s("Esperei demasiado tempo pelo sem�foro!\n");
                return 2;
            }
            err = WriteCircularBufferChar(address, cmd);
            if (err) {
                _tprintf_s("Ocorreu um erro ao executar um comando!\n");
                return 1;
            }
            SetEvent(commandWriteEvent);
            result = WaitForSingleObject(commandReadEvent, 5000);
            if (result != WAIT_OBJECT_0) {
                _tprintf_s("Esperei demasiado tempo pelo evento de comando concluido!\n");
                return 2;
            }
            ReleaseSemaphore(semaphore, 1, NULL);
        }
    } while (_tcscmp(cmd, _T("sair")) != 0);

    closeSharedFile(&file, &address);
    CloseHandle(commandWriteEvent);
    CloseHandle(commandReadEvent);

    return 0;
}