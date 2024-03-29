﻿#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "Handler.h"
#include "Commands.h"
#include "MemoryDLL.h"

#define MAX 256

int _tmain(int argc, TCHAR* argv[]) {

    // Definição do UNICODE no output e input

#ifdef UNICODE 
    if (_setmode(_fileno(stdin), _O_WTEXT) == -1) {
        _tprintf(_T("Failed to set input mode.\n"));
        return 1;
    }
    if (_setmode(_fileno(stdout), _O_WTEXT) == -1) {
        _tprintf(_T("Failed to set output mode.\n"));
        return 1;
    }
#endif

    // Inicializa uma seed para random numbers

    initRandom();

    // Verificação se programa está a correr ou não

    HANDLE mut = CreateMutex(NULL, FALSE, _T("SapoServerMutex"));

    if (mut == NULL) {
        _tprintf(_T("Error creating Mutex: %d\n"), GetLastError());
        return 2;
    }
    else if (GetLastError() == ERROR_ALREADY_EXISTS) {
        _tprintf(_T("Server is already running!\n"));
        return 0;
    }

    // Carrega as definições

    GAME_SETTINGS gs = {0, 0};
    int err = loadOptions(&gs);

    if (err) createOptions(&gs);
    else _tprintf_s(_T("As definições foram carregadas com sucesso!\n"));

    // Loop do jogo

    THREADINFO threadInfo = { NULL, FALSE, 0, &gs, NULL, NULL, 0 };
    threadInfo.thread = CreateThread(NULL, 0, GameThread, (LPVOID) & threadInfo, 0, &(threadInfo.threadId));

    //Comandos

    err = getCommands(&threadInfo);

    // Quando o jogo acaba
    // Verifica se a Thread já está fechada
    
    if (threadInfo.thread) WaitForSingleObject(threadInfo.thread, 5000);
    if (threadInfo.opCmdThread) WaitForSingleObject(threadInfo.opCmdThread, 5000);

    // O mutex é libertado para um novo servidor poder ser aberto

    ReleaseMutex(mut);
    CloseHandle(mut);

    return err;
}