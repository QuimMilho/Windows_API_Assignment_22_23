#include "Handler.h"
#include "Game.h"

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

	HKEY hKey;
	LONG createStatus;

    // Criar a chave do jogo em HKEY_CURRENT_USER\Software
    createStatus = RegCreateKeyEx(
        HKEY_CURRENT_USER,
        TEXT("Software\\Jogo"),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,

        // Handle da key atual (Jogo)
        &hKey,

        NULL
    );

    if (createStatus != ERROR_SUCCESS) {
        printf("Erro a criar a chave 'Jogo'. Código de Erro: %ld\n", createStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Velocidade -> Sub-chave de Jogo
    createStatus = RegCreateKeyEx(
        hKey,
        TEXT("velocidade"),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,

        // Handle da parent key (Jogo)
        &hKey,

        NULL
    );

    if (createStatus != ERROR_SUCCESS) {
        printf("Erro a criar sub-chave 'velocidade'. Código de Erro : % ld\n", createStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Faixas -> Sub-chave de Jogo
    createStatus = RegCreateKeyEx(
        hKey,
        TEXT("faixas"),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,

        // Handle da parent key (Jogo)
        &hKey,

        NULL
    );

    if (createStatus != ERROR_SUCCESS) {
        printf("Erro a criar sub-chave 'faixas'. Código de Erro : % ld\n", createStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Chamar a função setOptions com os valores pretendidos

    // Fechar a parent key Jogo com o seu handler hkey
    RegCloseKey(hKey);
}

// Carrega as definições
int loadOptions(GAME_SETTINGS * gameSettings) {

    HKEY hKey;
    LONG openStatus, getValueStatus;
    DWORD nVelocidade, nFaixas;
    DWORD valueType;

    // Tipo de dados das sub-keys -> Valores Inteiros
    DWORD   dataSize = sizeof(DWORD),

    // Abre a parent key Jogo
    openStatus = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        TEXT("Software\\Jogo"),
        0,
        KEY_READ,
        &hKey
    );

    if (openStatus != ERROR_SUCCESS) {
        printf("Failed to open registry key. Error code: %ld\n", openStatus);
        return 1;
    }

    // Obtém o valor da velocidade -> vVelocidade
    getValueStatus = RegGetValue(
        hKey,
        NULL,
        TEXT("velocidade"),
        RRF_RT_REG_DWORD,
        &valueType,
        &nVelocidade,
        &dataSize
    );

    if (getValueStatus != ERROR_SUCCESS) {
        printf("Erro ao aceder ao valor da sub-chave 'carros'. Código de Erro: %ld\n", getValueStatus);
        RegCloseKey(hKey);
    }

    // Obtém o valor das faixas -> nFaixas
    getValueStatus = RegGetValue(
        hKey,
        NULL,
        TEXT("faixas"),
        RRF_RT_REG_DWORD,
        &valueType,
        &nFaixas,
        &dataSize
    );

    if (getValueStatus != ERROR_SUCCESS) {
        printf("Erro ao aceder ao valor da sub-chave 'faixas'. Código de Erro: %ld\n", getValueStatus);
        RegCloseKey(hKey);
    }

    // Close the "game" key
    RegCloseKey(hKey);

}

// Atribui valores ás sub-chaves
int setOptions(HKEY hKey, GAME_SETTINGS* gameSettings, int k) {

    LONG    setValueStatus;
    DWORD   nVelocidade = 1,
            nFaixas = 1;

    // Atribuir à sub-chave "velocidade" o valor vVelocidade
    setValueStatus = RegSetValueEx(
        hKey,
        TEXT("velocidade"),
        0,
        REG_DWORD,
        (const BYTE*)&nVelocidade,
        sizeof(nVelocidade)
    );

    if (setValueStatus != ERROR_SUCCESS) {
        printf("Erro ao atribuir valor à sub-chave 'velocidade'. Código de Erro: %ld\n", setValueStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Atribuir à sub-chave "faixas" o valor vFaixas
    setValueStatus = RegSetValueEx(
        hKey,
        TEXT("faixas"),
        0,
        REG_DWORD,
        (const BYTE*)&nFaixas,
        sizeof(nFaixas)
    );

    if (setValueStatus != ERROR_SUCCESS) {
        printf("Erro ao atribuir valor à sub-chave 'faixas'. Código de Erro: %ld\n", setValueStatus);
        RegCloseKey(hKey);
        return 1;
    }

    // Fechar a parent key Jogo com o seu handler hkey
    RegCloseKey(hKey);
}

// Game Thread
DWORD WINAPI GameThread(LPVOID lpParam) {

	THREADINFO* threadInfo = (THREADINFO*)lpParam;
	threadInfo->running = TRUE;
	mainLoop(&(threadInfo->running));
	_tprintf_s(_T("Thread terminado!"));

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
	double eleapsed_time, delta = PRECISION / (double) TICKRATE;
	int ticks = 0, lastPrinted = 0;

	// Log do servidor a começar
	_tprintf(_T("Game loop started!\n"));

	// Game loop
	while (*running) {

		//Busca do tempo atual
		QueryPerformanceCounter(&now);

		// Cálculo do tempo entre o ultimo tick e o tempo atual
		eleapsed_time = floor((now.QuadPart - last.QuadPart) / (double)frequency.QuadPart * PRECISION);

		// É realizado um tick a cada "delta" tempo.
		if (eleapsed_time >= delta) {
			last = now;
			tick();
			ticks++;
		}

		// Cálculo do tempo entre o ultimo print e o tempo atual
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

	return 0;
}
