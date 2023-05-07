#include "MemoryDLL.h"
#include "Items.h"

#include <tchar.h>

#define SHARED_SERVER_MEMORY _T("ServerSapoShared")
#define SHARED_SERVER_TOTAL_BYTES 1096

#define SHARED_COMMAND_MEMORY _T("ServerSapoCommands")
#define SHARED_COMMAND_TOTAL_BYTES 104

int totalSize() {
	int k = sizeof(int) + sizeof(SAPO) * 2 + sizeof(int) * 10 + sizeof(CARRO) * 64 + 
		sizeof(int) + sizeof(OBSTACULO) * MAX_OBSTACULOS;
	return k;
}

// Game Files
// 
// Maping:
// int n Sapos - Sapos (1 ou 2) - int level - int nFaixas - int nCarrosF1 - CarrosF1 - int nCarrosF2 - CarrosF2 - ... 
//		- int nObstaculos - Obstaculos
//

int createGameFile(HANDLE * hFile) {
	(*hFile) = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_SERVER_TOTAL_BYTES, SHARED_SERVER_MEMORY);
	if (*hFile == NULL) return 1;
	return 0;
}

int openGameFile(HANDLE* hFile) {
	(*hFile) = OpenFileMapping(FILE_MAP_READ, FALSE, SHARED_SERVER_MEMORY);
	if (*hFile == NULL) return 1;
	return 0;
}

int mapGameSharedFile(HANDLE hFile, LPVOID * lpMapAddress) {
	(*lpMapAddress) = MapViewOfFile(hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (*lpMapAddress == NULL) return 1;
	return 0;
}

// Transforma a mem�ria partilhada em elementos organizados

int saveStructures(LPVOID address, JOGO * jogo) {

}

int toStructures(LPVOID address, JOGO* jogo) {
	if (address == NULL) return 1;

	// Pos no buffer
	int bufPos = 0;
	DWORD32* buffer = (DWORD32*) address;

	// n de sapos
	jogo->nSapos = (int)buffer[bufPos++];

	// Aloca os sapos na mem�ria
	jogo->sapos = malloc(sizeof(SAPO) * jogo->nSapos);
	if (jogo->sapos == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria para os sapos.\n"));
		return 1;
	}

	// Define o primeiro sapo
	jogo->sapos[0].x = (int)buffer[bufPos++];
	jogo->sapos[0].y = (int)buffer[bufPos++];
	jogo->sapos[0].lastMoved = (int)buffer[bufPos++];

	// Define o segundo sapo
	if (jogo->nSapos == 2) {
		jogo->sapos[1].x = (int)buffer[bufPos++];
		jogo->sapos[1].y = (int)buffer[bufPos++];
		jogo->sapos[1].lastMoved = (int)buffer[bufPos++];
	}

	// n do n�vel e de faixas
	jogo->level = (int)buffer[bufPos++];
	jogo->nLanes = (int)buffer[bufPos++];

	// Aloca uma array com nLanes elementos que s�o o n�mero de carros por faixa.
	jogo->nCarros = malloc(sizeof(int) * jogo->nLanes);
	if (jogo->nCarros == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria para o n�mero de carros.\n"));
		destroyGame(jogo);
		return 1;
	}

	// Vari�veis �teis para os carros
	int posCarros = 0, totalCarros = 0;

	//Percorre as faixas e alloca a mam�ria para os carros, adicionando os carros dessa faixa.
	for (int i = 0; i < jogo->nLanes; i++) {

		// Vai buscar o numero de carros na faixa i e adiciona ao total de carros
		jogo->nCarros[i] = (int)buffer[bufPos++];
		totalCarros += jogo->nCarros[i];

		// Vari�vel tempor�ria para n�o haver memory leaks
		CARRO* temp = NULL;

		// Alocada mem�ria para os novos carros
		temp = realloc(jogo->carros, sizeof(CARRO) * totalCarros);
		if (temp == NULL) {
			_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria para os carros no �ndice %d.\n"), i);
			destroyGame(jogo);
			return 1;
		}

		// Libertado o antigo ponteiro dos carros (possivelmente redondante) 
		// e atribuido o novo ponteiro
		free(jogo->carros);
		jogo->carros = temp;

		// Para os novos carros nesta faixa s�o atribuidos os novos valores.
		for (int h = 0; h < jogo->nCarros[i]; h++) {
			jogo->carros[posCarros].y = (int)buffer[bufPos++];
			jogo->carros[posCarros].x = (float)buffer[bufPos++];
			jogo->carros[posCarros++].vel = (float)buffer[bufPos++];
		}
	}

	// n de obstaculos
	jogo->nObstaculos = (int)buffer[bufPos++];

	// Aloca os obst�culos na mem�ria
	jogo->obstaculos = malloc(sizeof(OBSTACULO) * jogo->nObstaculos);
	if (jogo->obstaculos == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria para os obst�culos.\n"));
		destroyGame(jogo);
		return 1;
	}

	// Atribui os valores dos obst�culos
	for (int i = 0; i < jogo->nObstaculos; i++) {
		jogo->obstaculos[i].x = (int)buffer[bufPos++];
		jogo->obstaculos[i].y = (int)buffer[bufPos++];
	}

	return 0;
}

int destroyGame(JOGO* jogo) {
	if (jogo == NULL) return 1;
	free(jogo->carros);
	free(jogo->sapos);
	free(jogo->obstaculos);
	free(jogo->nCarros);
	return 0;
}

// Command Files

int createCommandFile(HANDLE* hFile) {
	(*hFile) = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_COMMAND_TOTAL_BYTES, SHARED_SERVER_MEMORY);
	if (*hFile == NULL) return 1;
	return 0;
}

int openCommandFile(HANDLE* hFile) {
	(*hFile) = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_SERVER_MEMORY);
	if (*hFile == NULL) return 1;
	return 0;
}

int mapCommandSharedFile(HANDLE hFile, LPVOID* lpMapAddress) {
	(*lpMapAddress) = MapViewOfFile(hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (*lpMapAddress == NULL) return 1;
	return 0;
}

// Close Shared Files

int closeSharedFile(HANDLE * hFile, LPVOID* lpMapAddress) {
	UnmapViewOfFile(*lpMapAddress);
	CloseHandle(*hFile);
	return 0;
}