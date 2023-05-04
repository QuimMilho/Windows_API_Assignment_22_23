#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "Handler.h"
#include "Commands.h"

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

    // Verificação se programa está a correr ou não

    HANDLE mut = CreateMutex(NULL, FALSE, _T("SapoServerMutex"));

    if (mut == NULL) {
        _tprintf(_T("Error creating Mutex: %d"), GetLastError());
        return 2;
    }
    else if (GetLastError() == ERROR_ALREADY_EXISTS) {
        _tprintf(_T("Server is already running!"));
        return 0;
    }

    // Loop do jogo
    THREADINFO threadInfo = { NULL, FALSE, 0 };
    threadInfo.thread = CreateThread(NULL, 0, GameThread, (LPVOID) & threadInfo, 0, &(threadInfo.threadId));

    //Comandos
    int err = getCommands(&threadInfo);

    // Quando o jogo acaba
    // Verifica se a Thread já está fechada
    /*
    DWORD codigo = 0;
    do {
        if (threadInfo.thread != NULL)
            GetExitCodeThread(threadInfo.thread, &codigo);
    } while (codigo != 0);
    */
    if (threadInfo.thread) WaitForSingleObject(threadInfo.thread, 5000);
    // O mutex é libertado para um novo servidor poder ser aberto

    ReleaseMutex(mut);
    CloseHandle(mut);

    return err;
}