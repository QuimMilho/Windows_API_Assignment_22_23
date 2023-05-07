#include "Handler.h"
#include "Game.h"
#include "Items.h"

#include <time.h>
#include <tchar.h>
#include <math.h>
#include <Windows.h>

// Precisões do relógio usadas
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
        _tprintf_s(_T("Erro a criar a chave 'Jogo'. Código de Erro: %ld\n"), createStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Velocidade -> Sub-chave de Jogo
    createStatus = RegCreateKeyEx(hKey, VELOCITY_KEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
        NULL, &hKeyVel, NULL);

    if (createStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro a criar sub-chave '%s'. Código de Erro : % ld\n"), VELOCITY_KEY, createStatus);
        RegCloseKey(hKey);
        RegCloseKey(hKeyVel);
        return 1;
    }

    // Faixas -> Sub-chave de Jogo
    createStatus = RegCreateKeyEx(hKey, LANE_KEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
        NULL, &hKeyLane, NULL);

    if (createStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro a criar sub-chave '%s'. Código de Erro : % ld\n"), LANE_KEY, createStatus);
        RegCloseKey(hKey);
        RegCloseKey(hKeyLane);
        RegCloseKey(hKeyVel);
        return 1;
    }

    // Chamar a função setOptions com os valores pretendidos

    setOptions(hKey, gameSettings, VELOCITY_KEY, DEFAULT_SPEED);
    setOptions(hKey, gameSettings, LANE_KEY, DEFAULT_LANES);

    // Fechar a parent key Jogo com o seu handler hkey
    RegCloseKey(hKey);
    RegCloseKey(hKeyLane);
    RegCloseKey(hKeyVel);

    return 0;
}

// Carrega as definições
int loadOptions(GAME_SETTINGS * gameSettings) {

    HKEY hKey;
    LONG openStatus;
    DWORD valueType;

    // Abre a parent key Jogo
    openStatus = RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_READ, &hKey);

    if (openStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao abrir chave do Registry. Código de Erro: %ld\n"), openStatus);
        return 1;
    }

    // Obtém o valor da velocidade -> vVelocidade
    openStatus = RegGetValue(hKey, NULL, VELOCITY_KEY, RRF_RT_REG_DWORD, &valueType,
        &(gameSettings->init_speed), sizeof(DWORD));

    if (openStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao aceder ao valor da sub-chave '%s'. Código de Erro: %ld\n"), VELOCITY_KEY, openStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Obtém o valor das faixas -> nFaixas
    openStatus = RegGetValue(hKey, NULL, LANE_KEY, RRF_RT_REG_DWORD, &valueType, 
        &(gameSettings->lanes), sizeof(DWORD));

    if (openStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao aceder ao valor da sub-chave '%s'. Código de Erro: %ld\n"), LANE_KEY, openStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Close the "game" key
    RegCloseKey(hKey);

    return 0;
}

// Atribui valores ás sub-chaves
int setOptions(HKEY * hKey, GAME_SETTINGS* gameSettings, TCHAR * option, DWORD value) {

    LONG setValueStatus;
    BOOL keyAlreadyOpened = TRUE;

    // Caso receba uma key que ainda não esteja aberta
    if (*hKey == NULL) {
        keyAlreadyOpened = FALSE;
        // Abrir Key
        LONG openStatus;
        openStatus = RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_READ, hKey);
        if (openStatus != ERROR_SUCCESS) {
            _tprintf_s(_T("Erro ao abrir chave do Registry. Código de Erro: %ld\n"), openStatus);
            return 1;
        }
    }

    // Atribuir à sub-chave "velocidade" o valor vVelocidade
    setValueStatus = RegSetValueEx(*hKey, option, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));

    if (setValueStatus != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao atribuir valor à sub-chave '%s'. Código de Erro: %ld\n"), option, setValueStatus);
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
	int err = mainLoop(&(threadInfo->running));
    if (err) {
        _tprintf_s(_T("Ocorreu um erro ao começar o main loop!\n"));
        return 1;
    }
	_tprintf_s(_T("Thread terminado!\n"));
	return 0;
}

// Loop do jogo
int mainLoop(BOOL* running) {

	// Frequência do relógio do computador
	LARGE_INTEGER frequency;

	// Tempos guardados para calculo dos ticks
	LARGE_INTEGER last, now, lastPrint;

	// Obtenção da frequência e dos primeiros tempos
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&now);
	last = lastPrint = now;

	// Definição das váriáveis e do tempo entre cada tick
	// Delta é o tempo entre cada tick (1 segundo dividido pelo numero de ticks)
	double elapsed_time, delta = PRECISION / (double) TICKRATE;
	int ticks = 0, lastPrinted = 0;

    HANDLE serverTickEvent = CreateEvent(NULL, TRUE, FALSE, SERVER_TICK_EVENT);

    if (serverTickEvent == NULL) {
        _tprintf_s(_T("O evento %s não foi criado! Código de Erro: %d\n"), SERVER_TICK_EVENT, GetLastError());
        return 1;
    }

    // Define o server como running!
    *running = TRUE;

	// Game loop
	while (*running) {

		//Busca do tempo atual
		QueryPerformanceCounter(&now);

		// Cálculo do tempo entre o ultimo tick e o tempo atual
		elapsed_time = floor((now.QuadPart - last.QuadPart) / (double)frequency.QuadPart * PRECISION);

		// É realizado um tick a cada "delta" tempo.
		if (elapsed_time >= delta) {
			last = now;

            // Game tick
			tick();
			ticks++;

            // Informa os operadores que devem atualizar
            SetEvent(serverTickEvent);
            // Espera 5ms para dar reset ao evento
            Sleep(1);
            ResetEvent(serverTickEvent);
		}

		// Cálculo do tempo entre o ultimo print e o tempo atual (Imprime 1 vez por segundo)
		/*
		eleapsed_time = floor((now.QuadPart - lastPrint.QuadPart) / (double)frequency.QuadPart * PRECISION);
		// Uma vez por segundo são impressas as tickrates
		if (eleapsed_time > PRECISION) {
			lastPrint = now;
			_tprintf(_T("%d %d\n"), ticks, ticks - lastPrinted);
			lastPrinted = ticks;
		}
		*/
	}

    CloseHandle(serverTickEvent);

	return 0;
}
