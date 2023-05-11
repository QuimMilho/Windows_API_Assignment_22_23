#include "Handler.h"
#include "Game.h"
#include "Items.h"
#include "MemoryDLL.h"

#include <time.h>
#include <tchar.h>
#include <math.h>
#include <Windows.h>

// Precis�es do rel�gio usadas
#define PRECISION_NANO 1000000000
#define PRECISION_MICRO 1000000
#define PRECISION_MILLIS 1000

#define PRECISION PRECISION_MICRO

// Cria as chaves da registry
int createOptions(GAME_SETTINGS* gameSettings) {

	HKEY hKey, hKeyVel, hKeyLane;
	LONG createStatus;

    // Criar a chave do jogo em HKEY_CURRENT_USER\Software
    createStatus = RegCreateKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, NULL, REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, NULL, &hKey, NULL);

    if (createStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro a criar a chave 'Jogo'. C�digo de Erro: %ld\n"), createStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Chamar a fun��o setOptions com os valores pretendidos

    setOptions(&hKey, gameSettings, VELOCITY_KEY, DEFAULT_SPEED);
    setOptions(&hKey, gameSettings, LANE_KEY, DEFAULT_LANES);

    // Fechar a parent key Jogo com o seu handler hkey
    RegCloseKey(hKey);

    gameSettings->init_speed = DEFAULT_SPEED;
    gameSettings->lanes = DEFAULT_LANES;

    return 0;
}

// Carrega as defini��es
int loadOptions(GAME_SETTINGS * gameSettings) {

    HKEY hKey;
    LONG openStatus;
    DWORD temp = sizeof(DWORD);

    // Abre a parent key Jogo
    openStatus = RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_READ, &hKey);

    if (openStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao abrir chave do Registry. C�digo de Erro: %ld\n"), openStatus);
        return 1;
    }

    // Obt�m o valor da velocidade -> vVelocidade
    openStatus = RegGetValue(hKey, NULL, VELOCITY_KEY, RRF_RT_DWORD, NULL,
        &(gameSettings->init_speed), &temp);

    if (openStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao aceder ao valor da sub-chave '%s'. C�digo de Erro: %ld\n"), VELOCITY_KEY, openStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Obt�m o valor das faixas -> nFaixas
    openStatus = RegGetValue(hKey, NULL, LANE_KEY, RRF_RT_DWORD, NULL,
        &(gameSettings->lanes), &temp);

    if (openStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao aceder ao valor da sub-chave '%s'. C�digo de Erro: %ld\n"), LANE_KEY, openStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Close the "game" key
    RegCloseKey(hKey);

    return 0;
}

// Atribui valores �s sub-chaves
int setOptions(HKEY * hKey, GAME_SETTINGS* gameSettings, LPSTR option, DWORD value) {

    LONG setValueStatus;
    BOOL keyAlreadyOpened = TRUE;

    // Caso receba uma key que ainda n�o esteja aberta
    if (*hKey == NULL) {
        keyAlreadyOpened = FALSE;
        // Abrir Key
        LONG openStatus;
        openStatus = RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_READ, *hKey);
        if (openStatus != ERROR_SUCCESS) {
            _tprintf_s(_T("Erro ao abrir chave do Registry. C�digo de Erro: %ld\n"), openStatus);
            return 1;
        }
    }

    // Atribuir � sub-chave "velocidade" o valor vVelocidade
    setValueStatus = RegSetValueEx(*hKey, option, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));

    if (setValueStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao atribuir valor � sub-chave '%s'. C�digo de Erro: %ld\n"), option, setValueStatus);
        if (!keyAlreadyOpened) RegCloseKey(*hKey);
        return 1;
    }

    // Fechar a parent key Jogo com o seu handler hkey
    if (!keyAlreadyOpened) RegCloseKey(*hKey);

    return 0;
}

// Game Thread
DWORD WINAPI GameThread(LPVOID lpParam) {
	THREADINFO* threadInfo = (THREADINFO*)lpParam;
    int err = mainLoop(&(threadInfo->running), threadInfo->gs);
    if (err) {
        _tprintf_s(_T("Ocorreu um erro ao come�ar o main loop!\n"));
        return 1;
    }
	_tprintf_s(_T("Thread terminado!\n"));
	return 0;
}

// Loop do jogo
int mainLoop(BOOL* running, GAME_SETTINGS * gs) {

    initRandom();

	// Frequ�ncia do rel�gio do computador
	LARGE_INTEGER frequency;

	// Tempos guardados para calculo dos ticks
	LARGE_INTEGER last, now, lastPrint;

	// Obten��o da frequ�ncia e dos primeiros tempos
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&now);
	last = lastPrint = now;

	// Defini��o das v�ri�veis e do tempo entre cada tick
	// Delta � o tempo entre cada tick (1 segundo dividido pelo numero de ticks)
	double elapsed_time, delta = PRECISION / (double) TICKRATE;
	int ticks = 0, lastPrinted = 0;

    // Server tick event
    HANDLE serverTickEvent = CreateEvent(NULL, TRUE, FALSE, SERVER_TICK_EVENT);

    if (serverTickEvent == NULL) {
        _tprintf_s(_T("O evento %s n�o foi criado! C�digo de Erro: %d\n"), SERVER_TICK_EVENT, GetLastError());
        return 1;
    }

    // Cria a mem�ria partilhada
    HANDLE file;
    int err = createGameFile(&file);

    if (err) {
        _tprintf_s(_T("Erro ao criar ficheiro da mem�ria partilhada\n"));
        return 1;
    }

    // Mapeia a mem�ria partilhada
    LPVOID address;
    err = mapGameSharedFile(file, &address, FILE_MAP_ALL_ACCESS);

    if (err) {
        _tprintf_s(_T("Erro ao mapear a mem�ria partilhada\n"));
        return 1;
    }

    // Define o server como running!
    *running = TRUE;

    // Cria o jogo
    JOGO jogo;

    err = createGame(&jogo, 2, gs);

    if (err) {
        _tprintf_s(_T("Erro ao criar inst�ncia inicial do jogo!\n"));
        return 1;
    }

	// Game loop
	while (*running) {

		//Busca do tempo atual
		QueryPerformanceCounter(&now);

		// C�lculo do tempo entre o ultimo tick e o tempo atual
		elapsed_time = floor((now.QuadPart - last.QuadPart) / (double)frequency.QuadPart * PRECISION);

		// � realizado um tick a cada "delta" tempo.
		if (elapsed_time >= delta) {
			last = now;

            // Game tick
			err = tick(&jogo, gs, ticks);
			ticks++;

            if (err) {
                _tprintf_s(_T("Ocorreu um erro ao dar tick no jogo!\n"));
                break;
            }

            saveStructures(address, &jogo);

            // Informa os operadores que devem atualizar
            SetEvent(serverTickEvent);
            // Espera 5ms para dar reset ao evento
            Sleep(1);
            ResetEvent(serverTickEvent);
		}

		// C�lculo do tempo entre o ultimo print e o tempo atual (Imprime 1 vez por segundo)
		/*
		eleapsed_time = floor((now.QuadPart - lastPrint.QuadPart) / (double)frequency.QuadPart * PRECISION);
		// Uma vez por segundo s�o impressas as tickrates
		if (eleapsed_time > PRECISION) {
			lastPrint = now;
			_tprintf(_T("%d %d\n"), ticks, ticks - lastPrinted);
			lastPrinted = ticks;
		}
		*/
	}

    destroyGame(&jogo);

    CloseHandle(serverTickEvent);

    closeSharedFile(&file, &address);

	return 0;
}

int initRandom() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    srand(now.QuadPart);
}

int genRand(int a) {
    return rand() % a;
}

int genRandBt(int a, int b) {
    return genRand(b - a) + a;
}