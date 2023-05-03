#include "Handler.h"
#include "Game.h"

#include <time.h>
#include <tchar.h>
#include <math.h>
#include <Windows.h>

#define PRECISION_NANO 1000000000
#define PRECISION_MICRO 1000000
#define PRECISION_MILLIS 1000
#define PRECISION_SEC 1

#define PRECISION PRECISION_MICRO

int mainLoop() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER last, now, lastPrint;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&now);
	last = lastPrint = now;

	double eleapsed_time, delta = PRECISION / (double) TICKRATE;
	int ticks = 0, lastPrinted = 0;

	_tprintf(_T("Game loop started!\n"));

	while (ticks < 6400) {
		QueryPerformanceCounter(&now);

		eleapsed_time = floor((now.QuadPart - last.QuadPart) / (double)frequency.QuadPart * PRECISION);
		if (eleapsed_time >= delta) {
			last = now;
			tick();
			ticks++;
		}

		eleapsed_time = floor((now.QuadPart - lastPrint.QuadPart) / (double)frequency.QuadPart * PRECISION);
		if (eleapsed_time > PRECISION) {
			lastPrint = now;
			_tprintf(_T("%d %d\n"), ticks, ticks - lastPrinted);
			lastPrinted = ticks;
			
		}
	}
	return 0;
}
