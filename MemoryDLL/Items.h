#pragma once

#define MAX_OBSTACULOS 32

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