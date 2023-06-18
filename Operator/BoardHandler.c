#include "BoardHandler.h"
#include "MemoryDLL.h"
#include "Console.h"
#include "Items.h"

#include <tchar.h>

DWORD WINAPI BoardThread(LPVOID lpParam) {

	// Thread info
	THREADINFO * threadInfo = (THREADINFO *) lpParam;

	// Memória partilhada
	HANDLE hfile;
	int err = openGameFile(&hfile);
	if (err != 0) {
		_tprintf_s(_T("Erro ao abrir o ficheiro de memória partilhada."));
		return 1;
	}

	// Mapia a memória partilhada
	LPVOID gameInfo;
	err = mapGameSharedFile(hfile, &gameInfo, FILE_MAP_READ);
	if (err != 0) {
		_tprintf_s(_T("Erro ao mapear a memória partilhada."));
		return 1;
	}

	// Evento
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SERVER_TICK_EVENT);
	if (hEvent == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao abrir o evento! Código do erro: %d"), GetLastError());
		return 1;
	}

	DWORD dwEventResult;

	threadInfo->running = TRUE;

	setCursorPosition(15, 20);

	clear();

	// Thread running
	while (threadInfo->running) {
		dwEventResult = WaitForSingleObject(hEvent, 1000);
		if (dwEventResult == WAIT_TIMEOUT) {
			_tprintf_s(_T("O evento deu timeout! O server ainda está ligado?\n"));
			return 1;
		}
		else if (dwEventResult == WAIT_FAILED) {
			_tprintf_s(_T("Ocorreu um erro ao esperar pelo evento '%s'! Código do erro: %d\n"), 
					SERVER_TICK_EVENT, GetLastError());
			return 1;
		}
		else if (dwEventResult == WAIT_ABANDONED) {
			_tprintf_s(_T("Handler do evento abandonado!\n"));
			return 1;
		}

		JOGO jogo = {0, NULL, 0, 0, NULL, NULL, 0, NULL};
		copyGame(&jogo, gameInfo);
		CHAR_INFO* tab = malloc(sizeof(CHAR_INFO) * (jogo.nLanes + 2) * 20);
		if (tab == NULL) {
			_tprintf_s(_T("Ocorreu um erro ao alocar a memória da array de chars\n"));
			return 1;
		}

		getTab(tab, &jogo);
		printTab(tab, jogo.nLanes);

		free(tab);

		do {
			dwEventResult = WaitForSingleObject(hEvent, 0);
		} while (dwEventResult == WAIT_TIMEOUT);
	}

	// Desliga a memória partilhada
	closeSharedFile(&hfile, &gameInfo);

	_tprintf_s(_T("Thread terminada!\n"));

	return 0;
}