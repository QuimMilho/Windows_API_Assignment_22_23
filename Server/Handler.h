#pragma once

#include "Game.h"

#include <Windows.h>

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