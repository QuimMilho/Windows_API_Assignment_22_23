#include "Game.h"
#include "MemoryDLL.h"
#include "Handler.h"

#include <math.h>

int moveCars(JOGO* jogo);
int addCars(JOGO* jogo, GAME_SETTINGS* gs);
int addCar(JOGO* jogo, int lane, GAME_SETTINGS* gs);
int removeCar(JOGO* jogo, int index);

float getSpeed(int level, GAME_SETTINGS* gs);

int addObstaculo(JOGO* jogo);
int removeObstaculo(JOGO* jogo);

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
	// Destroi o jogo antigo para previnir memory leaks
	destroyGame(jogo);

	// Define o número de sapos
	jogo->nSapos = nSapos;

	// Alloca a memória dos sapos
	jogo->sapos = malloc(sizeof(SAPO) * nSapos);

	if (jogo->sapos == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória dos sapos.\n"));
		return 1;
	}

	// Define os sapos
	jogo->sapos[0].lastMoved = 0;
	jogo->sapos[0].x = genRand(20);
	jogo->sapos[0].y = 0;

	if (nSapos == 2) {
		jogo->sapos[1].lastMoved = 0;
		int k = genRand(19);
		if (k >= jogo->sapos[0].y) k++;
		jogo->sapos[1].x = k;
		jogo->sapos[1].y = 0;
	}

	// Define o nível atual (0 multiplayer, 1 singleplayer)
	jogo->level = nSapos == 2 ? 0 : 1;

	// Define o número de faixas do jogo atual
	jogo->nLanes = gs->lanes;

	// Alloca a memória da array do número de carros
	jogo->direcao = malloc(sizeof(int) * gs->lanes);
	if (jogo->direcao == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória da array da direção das faixas.\n"));
		return 1;
	}

	for (int i = 0; i < gs->lanes; i++) {
		jogo->direcao[i] = genRand(2);
		if (jogo->direcao[i] == 0) jogo->direcao[i] = -1;
	}

	jogo->totalDeCarros = 0;

	// Define o ponteiro para os carros para NULL (No início ainda não há carros)
	jogo->carros = NULL;

	// Define o número de obstáculos como 0 e o ponteiro dos mesmos para NULL
	jogo->nObstaculos = 0;
	jogo->obstaculos = NULL;

	return 0;
}

int moveCars(JOGO* jogo) {
	for (int i = 0; i < jogo->totalDeCarros; i++) {
		jogo->carros[i].x += (jogo->carros[i].vel * jogo->direcao[jogo->carros[i].y - 1]);
		if (jogo->carros[i].x < 0 || jogo->carros[i].x > 20) {
			int err = removeCar(jogo, i);
			if (err) {
				return 1;
			}
			i--;
		}
			
	}
	return 0;
}

int addCars(JOGO* jogo, GAME_SETTINGS* gs) {
	int k[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < jogo->totalDeCarros; i++) {
		int lane = jogo->carros[i].y - 1;
		if (k[lane] != -1) {
			if (floor(jogo->carros[i].x == 0) && jogo->direcao[lane] == 1) {
				k[lane] = -1;
			}
			else if (floor(jogo->carros[i].x == 20) || floor(jogo->carros[i].x) == 19 && jogo->direcao[lane] == -1) {
				k[lane] = -1;
			}
			else {
				k[lane]++;
			}
		}
	}
	for (int i = 0; i < jogo->nLanes; i++) {
		if (k[i] != -1 && k[i] < 8) {
			if (genRand(PROB_GERAR_CARRO) == 0) { 
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
	CARRO* temp = realloc(jogo->carros, sizeof(CARRO) * (jogo->totalDeCarros + 1));
	if (temp == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória dos carros!\n"));
		destroyGame(jogo);
		return 1;
	}
	jogo->carros = temp;
	jogo->carros[jogo->totalDeCarros].x = jogo->direcao[lane] == 1 ? 0 : 20;
	jogo->carros[jogo->totalDeCarros].y = lane + 1;
	jogo->carros[jogo->totalDeCarros].vel = getSpeed(jogo->level, gs);
	jogo->totalDeCarros++;
	return 0;
}

int removeCar(JOGO* jogo, int index) {
	CARRO* temp = malloc(sizeof(CARRO) * (jogo->totalDeCarros - 1));
	if (temp == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a nova memória dos carros!"));
		destroyGame(jogo);
		return 1;
	}
	BOOL passed = FALSE;
	for (int i = 0; i < jogo->totalDeCarros; i++) {
		if (i == index) {
			passed = TRUE;
			continue;
		}
		int h = passed ? i - 1 : i;
		temp[h].vel = jogo->carros[i].vel;
		temp[h].x = jogo->carros[i].x;
		temp[h].y = jogo->carros[i].y;
	}
	free(jogo->carros);
	jogo->carros = temp;
	jogo->totalDeCarros--;

	return 0;
}

float getSpeed(int level, GAME_SETTINGS* gs) {
	if (level == 0 || level == 1) return gs->init_speed / (float)TICKRATE;
	else return (gs->init_speed / (float)TICKRATE) * pow(LEVEL_SPEED_MULTIPLIER, level);
}

int addObstaculo(JOGO* jogo) {

}

int removeObstaculo(JOGO* jogo) {

}