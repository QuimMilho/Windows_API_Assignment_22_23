#include "Game.h"
#include "MemoryDLL.h"
#include "Handler.h"

// Fun��o de tick do jogo

int tick(JOGO* jogo) {

	return 0;
}

int createGame(JOGO* jogo, int nSapos, GAME_SETTINGS * gs) {
	// Destroi o jogo antigo para previnir memory leaks
	destroyGame(jogo);

	// Define o n�mero de sapos
	jogo->nSapos = nSapos;

	// Alloca a mem�ria dos sapos
	jogo->sapos = malloc(sizeof(SAPO) * nSapos);

	if (jogo->sapos == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria dos sapos.\n"));
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

	// Define o n�vel atual (0 multiplayer, 1 singleplayer)
	jogo->level = nSapos == 2 ? 0 : 1;

	// Define o n�mero de faixas do jogo atual
	jogo->nLanes = gs->lanes;

	// Alloca a mem�ria da array do n�mero de carros
	jogo->nCarros = malloc(sizeof(int) * gs->lanes);
	if (jogo->nCarros == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria da array do n�mero de carros.\n"));
		return 1;
	}

	for (int i = 0; i < gs->lanes; i++) {
		jogo->nCarros[i] = 0;
	}

	// Define o ponteiro para os carros para NULL (No in�cio ainda n�o h� carros)
	jogo->carros = NULL;

	// Define o n�mero de obst�culos como 0 e o ponteiro dos mesmos para NULL
	jogo->nObstaculos = 0;
	jogo->obstaculos = NULL;

	return 0;
}