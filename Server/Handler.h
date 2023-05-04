#pragma once

#include <Windows.h>

// Thread Info

typedef struct ThreadInfo { 
	HANDLE thread;		// Handle da Thread
	BOOL running;		// If thread must be running or not
	DWORD threadId;		// Thread ID
} THREADINFO;

// Definições do jogo

typedef struct GameSettings { 
	DWORD lanes; 
	DWORD init_speed; 
} GAME_SETTINGS;

// Load Settings

int loadOptions(GAME_SETTINGS* gameSettings);

int setOptions(GAME_SETTINGS* gameSettings, int k);

// Game Thread

DWORD WINAPI GameThread(LPVOID lpParam);