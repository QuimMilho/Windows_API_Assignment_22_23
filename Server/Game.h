#pragma once

#ifndef GAME_H
#define GAME_H

#include <Windows.h>

// Tickrate do jogo
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

#endif //GAME_H