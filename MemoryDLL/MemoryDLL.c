#include "MemoryDLL.h"
#include "Items.h"

#include <tchar.h>

#define SHARED_SERVER_MEMORY _T("ServerSapoShared")
#define SHARED_SERVER_TOTAL_BYTES 1100

#define SHARED_COMMAND_MEMORY _T("ServerSapoCommands")
#define SHARED_COMMAND_TOTAL_BYTES 104

// Tamanho máximo do jogo

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
	(*hFile) = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_SERVER_TOTAL_BYTES, 
		SHARED_SERVER_MEMORY);
	if (*hFile == NULL) return 1;
	return 0;
}

int openGameFile(HANDLE* hFile) {
	(*hFile) = OpenFileMapping(FILE_MAP_READ, FALSE, SHARED_SERVER_MEMORY);
	if (*hFile == NULL) return 1;
	return 0;
}

int mapGameSharedFile(HANDLE hFile, LPVOID * lpMapAddress, DWORD permissions) {
	(*lpMapAddress) = MapViewOfFile(hFile, permissions, 0, 0, 0);
	if (*lpMapAddress == NULL) return 1;
	return 0;
}

// Transforma a memória partilhada em elementos organizados

int saveStructures(LPVOID address, JOGO * jogo) {
	if (address == NULL) return 1;

	// Pos no buffer
	int bufPos = 0;
	DWORD32* buffer = (DWORD32*)address;

	// n de sapos
	buffer[bufPos++] = jogo->nSapos;

	// Define o primeiro sapo
	buffer[bufPos++] = jogo->sapos[0].x;
	buffer[bufPos++] = jogo->sapos[0].y;
	buffer[bufPos++] = jogo->sapos[0].lastMoved;

	// Define o segundo sapo
	if (jogo->nSapos == 2) {
		buffer[bufPos++] = jogo->sapos[1].x;
		buffer[bufPos++] = jogo->sapos[1].y;
		buffer[bufPos++] = jogo->sapos[1].lastMoved;
	}

	// n do nível e de faixas
	buffer[bufPos++] = jogo->level;
	buffer[bufPos++] = jogo->nLanes;

	// Total de carros
	buffer[bufPos++] = jogo->totalDeCarros;

	//Percorre as faixas e adiciona os carros à memória partilhada.
	for (int i = 0; i < jogo->nLanes; i++) {
		// Vai buscar o numero de carros na faixa i
		buffer[bufPos++] = jogo->direcao[i];
	}

	for (int i = 0; i < jogo->totalDeCarros; i++) {
		buffer[bufPos++] = jogo->carros[i].x;
		buffer[bufPos++] = jogo->carros[i].y;
		buffer[bufPos++] = jogo->carros[i].vel;
	}
	// n de obstaculos
	buffer[bufPos++] = jogo->nObstaculos;

	// Atribui os valores dos obstáculos
	for (int i = 0; i < jogo->nObstaculos; i++) {
		buffer[bufPos++] = jogo->obstaculos[i].x;
		buffer[bufPos++] = jogo->obstaculos[i].y;
	}

	return 0;
}

int toStructures(LPVOID address, JOGO* jogo) {
	if (address == NULL) return 1;

	// Pos no buffer
	int bufPos = 0;
	DWORD32* buffer = (DWORD32*) address;

	// n de sapos
	jogo->nSapos = (int)buffer[bufPos++];

	// Aloca os sapos na memória
	jogo->sapos = malloc(sizeof(SAPO) * jogo->nSapos);
	if (jogo->sapos == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória para os sapos.\n"));
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

	// n do nível e de faixas
	jogo->level = (int)buffer[bufPos++];
	jogo->nLanes = (int)buffer[bufPos++];

	// Total de carros
	jogo->totalDeCarros = buffer[bufPos++];

	// Aloca uma array com nLanes elementos que são o número de carros por faixa.
	jogo->direcao = malloc(sizeof(int) * jogo->nLanes);
	if (jogo->direcao == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória para o número de carros.\n"));
		destroyGame(jogo);
		return 1;
	}

	//Percorre as faixas e alloca a memória para os carros, adicionando os carros dessa faixa.
	for (int i = 0; i < jogo->nLanes; i++) {

		// Vai buscar a direção da faixa
		jogo->direcao[i] = (int)buffer[bufPos++];
	}

	// Alocada memória para os carros
	jogo->carros = malloc(sizeof(CARRO) * jogo->totalDeCarros);
	if (jogo->carros == NULL && jogo->totalDeCarros != 0) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória para os carros.\n"));
		destroyGame(jogo);
		return 1;
	}

	// Percorre todos os carros e define  os seus valores

	for (int i = 0; i < jogo->totalDeCarros; i++) {
		jogo->carros[i].x = (int)buffer[bufPos++];
		jogo->carros[i].y = (float)buffer[bufPos++];
		jogo->carros[i].vel = (float)buffer[bufPos++];
	}

	// n de obstaculos
	jogo->nObstaculos = (int)buffer[bufPos++];

	// Aloca os obstáculos na memória
	jogo->obstaculos = malloc(sizeof(OBSTACULO) * jogo->nObstaculos);
	if (jogo->obstaculos == NULL && jogo->obstaculos != 0) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória para os obstáculos.\n"));
		destroyGame(jogo);
		return 1;
	}

	// Atribui os valores dos obstáculos
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
	free(jogo->direcao);
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