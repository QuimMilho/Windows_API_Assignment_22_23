#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "BoardHandler.h"
#include "MemoryDLL.h"
#include "OpCommands.h"

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
    threadInfo.thread = CreateThread(NULL, 0, BoardThread, (LPVOID)&threadInfo, 0, &threadInfo.threadId);

    int err = cmdLoop();
    if (err) {
        _tprintf_s(_T("Ocorreu um erro ao ler os comandos!\n"));
    }

    threadInfo.running = FALSE;

    WaitForSingleObject(threadInfo.thread, 5000);

    return 0;
}