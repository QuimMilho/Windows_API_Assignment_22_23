#pragma once
#include <tchar.h>

#define MAX_OBSTACULOS 32

#define SERVER_TICK_EVENT _T("ServerTickEvent")

typedef struct Sapo {
	int x;
	int y;
	int lastMoved;
} SAPO;

typedef struct Carro {
	int y;
	float x;
	float vel;
} CARRO;

typedef struct Obstaculo {
	int x;
	int y;
} OBSTACULO;

typedef struct Jogo {
	int nSapos;
	SAPO* sapos;
	int level;
	int nLanes;
	int* nCarros;
	CARRO* carros;
	int nObstaculos;
	OBSTACULO* obstaculos;
} JOGO;