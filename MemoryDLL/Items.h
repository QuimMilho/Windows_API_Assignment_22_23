#pragma once
#include <tchar.h>

// Códigos dos comandos
#define DONE 0
#define NO_EXIST 1
#define NO_ARGS 2
#define NO_ENOUGH_ARGS 3
#define INV_ARGS 4
#define CMD_ERROR 5

#define MAX_OBSTACULOS 32
#define MAX_LANES 8
#define MAX_CARS_LANE 8
#define MAX_CARS (MAX_LANES * MAX_CARS_LANE)

#define SERVER_TICK_EVENT _T("ServerTickEvent")

#define SERVER_COMMAND_WRITE _T("ServerCommandWrite")
#define SERVER_COMMAND_READ _T("ServerCommandRead")

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
	SAPO sapos[2];
	int level;
	int nLanes;
	int totalDeCarros;
	int direcao[MAX_LANES];
	CARRO carros[MAX_CARS];
	int nObstaculos;
	OBSTACULO obstaculos[MAX_OBSTACULOS];
} JOGO;