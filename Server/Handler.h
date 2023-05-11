#pragma once

#include "Game.h"

#include <Windows.h>

// Defines de caminhos do Registry
#define KEY_PATH _T("Software\\Jogo")
#define VELOCITY_KEY _T("velocidade")
#define LANE_KEY _T("faixas")

// Defines de defaults
#define DEFAULT_SPEED 10
#define DEFAULT_LANES 8

// Thread Info
typedef struct ThreadInfo { 
	HANDLE thread;		// Handle da Thread
	BOOL running;		// If thread must be running or not
	DWORD threadId;		// Thread ID
	GAME_SETTINGS* gs;
} THREADINFO;

// REGISTRY
int createOptions(GAME_SETTINGS* gameSettings);
int loadOptions(GAME_SETTINGS* gameSettings);
int setOptions(HKEY* hKey, GAME_SETTINGS* gameSettings, LPSTR option, DWORD value);

// Game Thread
DWORD WINAPI GameThread(LPVOID lpParam);

// Loop do Jogo
int mainLoop(BOOL* running, GAME_SETTINGS* gs);

// Random numbers
int initRandom();

int genRand(int a);
int genRandBt(int a, int b);