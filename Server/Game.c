#include "Game.h"
#include "MemoryDLL.h"
#include "Handler.h"

#include <math.h>

int moveCars(JOGO* jogo);
int addCars(JOGO* jogo, GAME_SETTINGS* gs);
int addCar(JOGO* jogo, int lane, GAME_SETTINGS* gs);
int removeCar(JOGO* jogo, int index);

float getSpeed(int level, GAME_SETTINGS* gs);

int hasObstaculo(JOGO* jogo, int x, int y);

// Função de tick do jogo

int tick(JOGO* jogo, GAME_SETTINGS* gs, int ticks) {
	int err = moveCars(jogo);
	if (err) {
		_tprintf_s(_T("Aqui\n"));
		return 1;
	}
	if (ticks % RANDOM_TICK_SPEED == 0) {
		err = addCars(jogo, gs);
		if (err) {
			_tprintf_s(_T("Aqui2\n"));
			return 1;
		}
	}
	return 0;
}

int createGame(JOGO* jogo, int nSapos, GAME_SETTINGS * gs) {
	// Define o número de sapos
	jogo->nSapos = nSapos;

	// Define os sapos
	jogo->sapos[0].lastMoved = 0;
	jogo->sapos[0].x = genRand(20);
	jogo->sapos[0].y = 0;

	if (nSapos == 2) {
		jogo->sapos[1].lastMoved = 0;
		int k = genRand(19);
		if (k >= jogo->sapos[0].x) k++;
		jogo->sapos[1].x = k;
		jogo->sapos[1].y = 0;
	}

	// Define o nível atual (0 multiplayer, 1 singleplayer)
	jogo->level = nSapos == 2 ? 0 : 1;

	// Define o número de faixas do jogo atual
	jogo->nLanes = gs->lanes;

	for (int i = 0; i < gs->lanes; i++) {
		jogo->direcao[i] = genRand(2);
		if (jogo->direcao[i] == 0) jogo->direcao[i] = -1;
	}

	jogo->totalDeCarros = 0;

	// Define o número de obstáculos como 0 e o ponteiro dos mesmos para NULL
	jogo->nObstaculos = 0;

	return 0;
}

int moveCars(JOGO* jogo) {
	for (int i = 0; i < jogo->totalDeCarros; i++) {
		jogo->carros[i].x += (jogo->carros[i].vel * jogo->direcao[jogo->carros[i].y - 1]);
		// Fora dos limites
		if (jogo->carros[i].x < 0 || jogo->carros[i].x > 20) {
			int err = removeCar(jogo, i);
			if (err) {
				return 1;
			}
			i--;
		}
		// Colisões
		int x = floor(jogo->carros[i].x), y = floor(jogo->carros->y);
		//		Carros
		for (int h = 0; h < jogo->totalDeCarros; h++) {
			if (&(jogo->carros[i]) != &(jogo->carros[h])) {
				int cX = floor(jogo->carros[h].x), cY = floor(jogo->carros[h].y);
				if (y == cY && x == cX) {
					jogo->carros[i].x = x - jogo->direcao[y - 1];
				}
			}
		}
		// Obstaculo
		for (int h = 0; h < jogo->nObstaculos; h++) {
			int cX = floor(jogo->obstaculos[h].x), cY = floor(jogo->obstaculos[h].y);
			if (y == cY && x == cX) {
				jogo->carros[i].x = x - jogo->direcao[y - 1];
			}
		}
	}
	return 0;
}

int addCars(JOGO* jogo, GAME_SETTINGS* gs) {
	int k[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < jogo->totalDeCarros; i++) {
		int lane = jogo->carros[i].y - 1;
		if (k[lane] != -1) {
			if ((floor(jogo->carros[i].x) == 0 || floor(jogo->carros[i].x) == 1) && jogo->direcao[lane] == 1) {
				k[lane] = -1;
			}
			else if ((floor(jogo->carros[i].x) == 20 || floor(jogo->carros[i].x)) == 19 && jogo->direcao[lane] == -1) {
				k[lane] = -1;
			}
			else {
				k[lane]++;
			}
		}
	}
	for (int i = 0; i < jogo->nLanes; i++) {
		if (k[i] != -1 && k[i] < 8) {
			if (genRand(PROB_GERAR_CARRO + k[i]) == 0) {
				int err = addCar(jogo, i, gs);
				if (err) {
					_tprintf_s(_T("Ocorreu um erro ao adicionar um carro na faixa %d\n"), i + 1);
					return 1;
				}
			}
		}
	}
	return 0;
}

int addCar(JOGO* jogo, int lane, GAME_SETTINGS* gs) {
	if (jogo->totalDeCarros == MAX_CARS) return 1;

	jogo->carros[jogo->totalDeCarros].x = jogo->direcao[lane] == 1 ? 0 : 20;
	jogo->carros[jogo->totalDeCarros].y = lane + 1;
	jogo->carros[jogo->totalDeCarros].vel = getSpeed(jogo->level, gs);

	jogo->totalDeCarros++;

	return 0;
}

int removeCar(JOGO* jogo, int index) {
	if (index >= jogo->totalDeCarros) return 1;

	for (int i = 0; i < jogo->totalDeCarros - 1; i++) {
		if (i >= index) {
			jogo->carros[i].vel = jogo->carros[i + 1].vel;
			jogo->carros[i].x = jogo->carros[i + 1].x;
			jogo->carros[i].y = jogo->carros[i + 1].y;
		}
	}

	jogo->totalDeCarros--;

	return 0;
}

float getSpeed(int level, GAME_SETTINGS* gs) {
	if (level == 0 || level == 1) return gs->init_speed / (float) TICKRATE;
	else return gs->init_speed / (float) TICKRATE * pow(LEVEL_SPEED_MULTIPLIER, level);
}

int addObstaculo(JOGO* jogo, int x, int y) {
	if (jogo->nObstaculos == MAX_OBSTACULOS) return 1;

	if (hasObstaculo(jogo, x, y)) return 1;

	jogo->obstaculos[jogo->nObstaculos].x = x;
	jogo->obstaculos[jogo->nObstaculos].y = y;
	jogo->nObstaculos++;

	return 0;
}

int removeObstaculo(JOGO* jogo, int index) {
	if (index >= jogo->nObstaculos) return 1;

	for (int i = 0; i < jogo->nObstaculos - 1; i++) {
		if (i >= index) {
			jogo->obstaculos[i].x = jogo->obstaculos[i + 1].x;
			jogo->obstaculos[i].y = jogo->obstaculos[i + 1].y;
		}
	}

	jogo->nObstaculos--;

	return 0;
}

int hasObstaculo(JOGO* jogo, int x, int y) {
	for (int i = 0; i < jogo->nObstaculos; i++) 
		if (jogo->obstaculos[i].x == x && jogo->obstaculos[i].y == y) 
			return 1;
	return 0;
}