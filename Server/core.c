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
    DWORD gameThreadId;
    HANDLE gameThread = CreateThread(NULL, NULL, GameThread, NULL, NULL, &gameThreadId);

    //Comandos
    getCommands();

    // Quando o jogo acaba, o servidor é libertado para um novo poder ser aberto

    ReleaseMutex(mut);
    CloseHandle(mut);

    return 0;
}