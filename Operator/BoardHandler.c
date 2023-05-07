#include "BoardHandler.h"
#include "MemoryDLL.h"
#include "Console.h"
#include "Items.h"

#include <tchar.h>

DWORD WINAPI BoardThread(LPVOID lpParam) {

	// Thread info
	THREADINFO * threadInfo = (THREADINFO *) lpParam;

	// Mem�ria partilhada
	HANDLE hfile;
	int err = openGameFile(&hfile);
	if (err != 0) {
		_tprintf_s(_T("Erro ao abrir o ficheiro de mem�ria partilhada."));
		return 1;
	}

	// Mapia a mem�ria partilhada
	LPVOID gameInfo;
	err = mapGameSharedFile(hfile, &gameInfo);
	if (err != 0) {
		_tprintf_s(_T("Erro ao mapear a mem�ria partilhada."));
		return 1;
	}

	// Evento
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SERVER_TICK_EVENT);
	DWORD dwEventResult;

	// Thread running
	while (threadInfo->running) {
		dwEventResult = WaitForSingleObject(hEvent, 1000);
		if (dwEventResult == WAIT_TIMEOUT) {
			_tprintf_s(_T("O evento deu timeout! O server ainda est� ligado?\n"));
			return 1;
		}
		else if (dwEventResult == WAIT_FAILED) {
			_tprintf_s(_T("Ocorreu um erro ao esperar pelo evento '%s'! C�digo do erro: %d\n"), 
					SERVER_TICK_EVENT, GetLastError());
			return 1;
		}
		else if (dwEventResult == WAIT_ABANDONED) {
			_tprintf_s(_T("Handler do evento abandonado!\n"));
			return 1;
		}

		JOGO jogo = {0, NULL, 0, 0, NULL, NULL, 0, NULL};
		toStructures(gameInfo, &jogo);
		CHAR_INFO* tab = malloc(sizeof(CHAR_INFO) * (jogo.nLanes + 2) * 20);
		if (tab == NULL) {
			_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria da array de chars\n"));
			return 1;
		}

		getTab(tab, &jogo);
		printTab(tab, &(jogo.nLanes));

		free(tab);
		destroyGame(&jogo);

		do {
			dwEventResult = WaitForSingleObject(hEvent, 0);
		} while (dwEventResult == WAIT_TIMEOUT);
	}

	// Desliga a mem�ria partilhada
	closeSharedFile(hfile, gameInfo);

	return 0;
}