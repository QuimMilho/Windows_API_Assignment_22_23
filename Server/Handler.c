#include "Handler.h"
#include "Game.h"

#include <time.h>
#include <tchar.h>
#include <math.h>
#include <Windows.h>

#define TICKRATE 64

int mainLoop() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER last, now, lastPrint;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&now);
	last = lastPrint = now;

	double eleapsed_time, delta = 1000000000 / (double) TICKRATE;
	int ticks = 0, lastPrinted = 0;

	while (ticks < 6400) {
		QueryPerformanceCounter(&now);

		eleapsed_time = (now.QuadPart - last.QuadPart) / (double) frequency.QuadPart * 1000000000;
		if (eleapsed_time >= delta) {
			last = now;
			tick();
			ticks++;
		}

		eleapsed_time = (now.QuadPart - lastPrint.QuadPart) / (double) frequency.QuadPart * 1000000000;
		if (eleapsed_time > 1000000000) {
			lastPrint = now;
			_tprintf(_T("%d %d\n"), ticks, ticks - lastPrinted);
			lastPrinted = ticks;
			
		}
	}
	return 0;
}
