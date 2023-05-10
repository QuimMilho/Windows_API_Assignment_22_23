#pragma once

#include <Windows.h>

#include "Items.h"

// Tickrate do jogo (Max 80)
#define TICKRATE 10

// Defini��es do jogo
typedef struct GameSettings {
	DWORD lanes;
	DWORD init_speed;
} GAME_SETTINGS;

// Fun��o de tick do jogo
int tick(JOGO * jogo);

// Criar jogo
int createGame(JOGO* jogo, int nSapos, GAME_SETTINGS* gs);