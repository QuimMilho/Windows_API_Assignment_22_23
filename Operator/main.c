#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "Console.h"
#include "BoardHandler.h"

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

    // Creates the board thread
    THREADINFO threadInfo = { NULL, FALSE, 0 };
    threadInfo.thread = CreateThread(NULL, 0, BoardThread, (LPVOID) & threadInfo, 0, &threadInfo.threadId);

    TCHAR cmd[MAX];

    do {
        _tprintf_s(_T(">"));
        fflush(stdin);
        _fgetts(cmd, MAX, stdin);
        cmd[_tcslen(cmd) - 1] = '\0';

    } while (_tcscmp(cmd, _T("sair")) != 0);

    threadInfo.running = FALSE;

    WaitForSingleObject(threadInfo.thread, 5000);

    return 0;
}