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
} THREADINFO;

// REGISTRY
int createOptions(GAME_SETTINGS* gameSettings);
int loadOptions(GAME_SETTINGS* gameSettings);
int setOptions(HKEY hkey, GAME_SETTINGS* gameSettings, int k);