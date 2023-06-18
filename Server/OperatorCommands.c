#include <Windows.h>

#include "Handler.h"
#include "MemoryDLL.h"
#include "Items.h"
#include "Commands.h"

int cmdMainLoop(THREADINFO* ti);

DWORD WINAPI CommandsThread(LPVOID lpParam) {
    THREADINFO* threadInfo = (THREADINFO*)lpParam;
    int err = cmdMainLoop(threadInfo);
    if (err) {
        _tprintf_s(_T("Ocorreu um erro ao começar o main loop!\n"));
        return 1;
    }
    _tprintf_s(_T("Commands Thread terminado!\n"));
    return 0;
}

int cmdMainLoop(THREADINFO * ti) {
    HANDLE semaphore = CreateSemaphore(NULL, 0, 1, SERVER_COMMAND_SEMAPHORE);

    if (semaphore == NULL) {
        _tprintf_s(_T("O semáforo %s não foi criado! Código de Erro: %d\n"), SERVER_COMMAND_SEMAPHORE, GetLastError());
        return 1;
    }

    HANDLE commandWriteEvent = CreateEvent(NULL, FALSE, FALSE, SERVER_COMMAND_WRITE);

    if (commandWriteEvent == NULL) {
        _tprintf_s(_T("O evento %s não foi criado! Código de Erro: %d\n"), SERVER_COMMAND_WRITE, GetLastError());
        CloseHandle(semaphore);
        return 1;
    }

    HANDLE commandReadEvent = CreateEvent(NULL, FALSE, FALSE, SERVER_COMMAND_READ);

    if (commandReadEvent == NULL) {
        _tprintf_s(_T("O evento %s não foi criado! Código de Erro: %d\n"), SERVER_COMMAND_READ, GetLastError());
        CloseHandle(semaphore);
        CloseHandle(commandWriteEvent);
        return 1;
    }

    HANDLE cmdFile;

    int err = createCommandFile(&cmdFile);

    if (err) {
        _tprintf_s(_T("Erro ao criar ficheiro da memória partilhada dos comandos\n"));
        CloseHandle(semaphore);
        CloseHandle(commandWriteEvent);
        CloseHandle(commandReadEvent);
        return 1;
    }

    // Mapeia a memória partilhada
    LPVOID cmdAddress;
    err = mapGameSharedFile(cmdFile, &cmdAddress, FILE_MAP_ALL_ACCESS);
    if (err) {
        _tprintf_s(_T("Erro ao mapear a memória partilhada dos comandos\n"));
        CloseHandle(cmdFile);
        CloseHandle(semaphore);
        CloseHandle(commandWriteEvent);
        CloseHandle(commandReadEvent);
        return 1;
    }

    StartCircularBuffer(cmdAddress);

    DWORD result;

    while (ti->running) {
        result = WaitForSingleObject(commandWriteEvent, 1000);

        if (result == WAIT_TIMEOUT) {
            continue;
        }
        else if (result != WAIT_OBJECT_0) {
            ti->running = FALSE;
            _tprintf_s(_T("Ocorreu um erro!\n"));
            break;
        }

        TCHAR str[SHARED_COMMAND_BUFFER_CHARS];

        err = ReadCircularBufferChar(cmdAddress, str, SHARED_COMMAND_BUFFER_CHARS);

        if (err) {
            ti->running = FALSE;
            _tprintf_s(_T("Ocorreu um erro a ler um comando!\n"));
            break;
        }

        process(str, OPERATOR, ti);

        SetEvent(commandReadEvent);
    }

    CloseHandle(semaphore);
    CloseHandle(commandWriteEvent);
    CloseHandle(commandReadEvent);
    closeSharedFile(&cmdFile, cmdAddress);
}