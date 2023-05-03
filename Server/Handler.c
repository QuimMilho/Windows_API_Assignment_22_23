#include "Handler.h"
#include "Game.h"

#include <time.h>
#include <tchar.h>
#include <math.h>
#include <Windows.h>

// Precis�es do rel�gio usadas

#define PRECISION_NANO 1000000000
#define PRECISION_MICRO 1000000
#define PRECISION_MILLIS 1000

#define PRECISION PRECISION_MICRO

// Loop do jogo

int mainLoop() {

	// Frequ�ncia do rel�gio do computador
	LARGE_INTEGER frequency;

	// Tempos guardados para calculo dos ticks
	LARGE_INTEGER last, now, lastPrint;

	// Obten��o da frequ�ncia e dos primeiros tempos
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&now);
	last = lastPrint = now;

	// Defini��o das v�ri�veis e do tempo entre cada tick
	// delta � o tempo entre cada tick (1 segundo dividido pelo numero de ticks)
	double eleapsed_time, delta = PRECISION / (double) TICKRATE;
	int ticks = 0, lastPrinted = -1;

	// Log do servidor a come�ar
	_tprintf(_T("Game loop started!\n"));

	// Game loop
	while (ticks < 6400) {
		//Busca do tempo atual
		QueryPerformanceCounter(&now);

		// C�lculo do tempo entre o ultimo tick e o tempo atual
		eleapsed_time = floor((now.QuadPart - last.QuadPart) / (double)frequency.QuadPart * PRECISION);
		// � realizado um tick a cada "delta" tempo.
		if (eleapsed_time >= delta) {
			last = now;
			tick();
			ticks++;
		}

		// C�lculo do tempo entre o ultimo print e o tempo atual
		eleapsed_time = floor((now.QuadPart - lastPrint.QuadPart) / (double)frequency.QuadPart * PRECISION);
		// Uma vez por segundo s�o impressas as tickrates
		if (eleapsed_time > PRECISION) {
			lastPrint = now;
			_tprintf(_T("%d %d\n"), ticks, ticks - lastPrinted);
			lastPrinted = ticks;
		}
	}
	return 0;
}
