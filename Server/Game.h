#pragma once

#include <Windows.h>

#include "Items.h"

// Tickrate do jogo (Max 80)
#define TICKRATE 10
#define RANDOM_TICK_SPEED TICKRATE / 2

#define PROB_GERAR_CARRO 6

#define LEVEL_SPEED_MULTIPLIER 1.1f

// Definições do jogo
typedef struct GameSettings {
	DWORD lanes;
	DWORD init_speed;
} GAME_SETTINGS;

// Função de tick do jogo
int tick(JOGO * jogo, GAME_SETTINGS* gs, int ticks);

// Criar jogo
int createGame(JOGO* jogo, int nSapos, GAME_SETTINGS* gs);