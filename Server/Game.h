#pragma once

#include <Windows.h>

// Tickrate do jogo (Max 80)
#define TICKRATE 10

// Definições do jogo
typedef struct GameSettings {
	DWORD lanes;
	DWORD init_speed;
} GAME_SETTINGS;

// Função de tick do jogo
int tick();

// Loop do Jogo
int mainLoop(BOOL* running);

// Game Thread
DWORD WINAPI GameThread(LPVOID lpParam);