#include "MemoryDLL.h"
#include "Items.h"

#include <tchar.h>

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

	// Variável de apoio
	int totalCarros = 0;

	//Percorre as faixas e adiciona os carros à memória partilhada.
	for (int i = 0; i < jogo->nLanes; i++) {
		// Vai buscar o numero de carros na faixa i
		buffer[bufPos++] = jogo->nCarros[i];

		// Para os novos carros nesta faixa são atribuidos os novos valores.
		for (int h = 0; h < jogo->nCarros[i]; h++) {
			buffer[bufPos++] = jogo->carros[totalCarros].y;
			buffer[bufPos++] = jogo->carros[totalCarros].x;
			buffer[bufPos++] = jogo->carros[totalCarros++].vel;
		}
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

	// Aloca uma array com nLanes elementos que são o número de carros por faixa.
	jogo->nCarros = malloc(sizeof(int) * jogo->nLanes);
	if (jogo->nCarros == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a memória para o número de carros.\n"));
		destroyGame(jogo);
		return 1;
	}

	// Variáveis úteis para os carros
	int posCarros = 0, totalCarros = 0;

	//Percorre as faixas e alloca a memória para os carros, adicionando os carros dessa faixa.
	for (int i = 0; i < jogo->nLanes; i++) {

		// Vai buscar o numero de carros na faixa i e adiciona ao total de carros
		jogo->nCarros[i] = (int)buffer[bufPos++];
		totalCarros += jogo->nCarros[i];

		// Variável temporária para não haver memory leaks
		CARRO* temp = NULL;

		// Alocada memória para os novos carros
		temp = realloc(jogo->carros, sizeof(CARRO) * totalCarros);
		if (temp == NULL && totalCarros != 0) {
			_tprintf_s(_T("Ocorreu um erro ao alocar a memória para os carros no índice %d.\n"), i);
			destroyGame(jogo);
			return 1;
		}

		if (totalCarros != 0) {
			// Libertado o antigo ponteiro dos carros (possivelmente redondante) 
			// e atribuido o novo ponteiro
			free(jogo->carros);
			jogo->carros = temp;

			// Para os novos carros nesta faixa são atribuidos os novos valores.
			for (int h = 0; h < jogo->nCarros[i]; h++) {
				jogo->carros[posCarros].y = (int)buffer[bufPos++];
				jogo->carros[posCarros].x = (float)buffer[bufPos++];
				jogo->carros[posCarros++].vel = (float)buffer[bufPos++];
			}
		}
	}

	// n de obstaculos
	jogo->nObstaculos = (int)buffer[bufPos++];

	// Aloca os obstáculos na memória
	jogo->obstaculos = malloc(sizeof(OBSTACULO) * jogo->nObstaculos);
	if (jogo->obstaculos == NULL) {
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



// -----------------------------------------------	BUFFER CIRCULAR -----------------------------------------------

// Tenho dúvidas no bufferSize e nalgumas aritméticas nomeadamente os /sizeof(char) ou *sizeof(char) uma vez que estamos a usar unicode

// Implementação da função CreateCircularBuffer (bufferSize -> em bytes)
CircularBuffer* CreateCircularBuffer(int bufferSize) {

	CircularBuffer* circBuffer = malloc(sizeof(CircularBuffer));

	if (circBuffer == NULL) {
		_tprintf_s(_T("[Buffer Circular] Ocorreu um erro ao criar o ponteiro.\n"));
		return 1;
	}

	circBuffer->buffer = malloc(bufferSize);
	circBuffer->bufferSize = bufferSize;
	circBuffer->head = 0;
	circBuffer->tail = 0;

	if (circBuffer->buffer == NULL) {
		_tprintf_s(_T("[Buffer Circular] Ocorreu um erro ao alocar memória para o buffer.\n"));
		free(circBuffer);
		return 1;
	}

	return circBuffer;
}

// Apagar o buffer circular
int DestroyCircularBuffer(CircularBuffer* circBuffer) {

	free(circBuffer->buffer);
	free(circBuffer);

	if (circBuffer->buffer != NULL || circBuffer != NULL) {
		_tprintf_s(_T("[Buffer Circular] Ocorreu um erro ao libertar memória.\n"));
		return 1;
	}

	return 0;
}

// Adiciona o elemento data ao buffer circular
int PushToCircularBuffer(CircularBuffer* circBuffer, const char* data) {

	// Verificar se o buffer está cheio
	unsigned int nextIndex = (circBuffer->head + 1) % circBuffer->bufferSize;
	if (nextIndex == circBuffer->tail) {
		// Buffer está cheio, não podemos inserir mais elementos
		return 0;
	}

	// Inserir o elemento no buffer
	memcpy(circBuffer->buffer + circBuffer->head, data, sizeof(char));
	circBuffer->head = (circBuffer->head + 1) % circBuffer->bufferSize;

	return 1;
}

// Remove um elemento do buffer e coloca esse elemento em data
int PopFromCircularBuffer(CircularBuffer* circBuffer, char* data, int dataSize) {

	// Não podemos fazer apenas +1  porque é circular se chegarmos ao fim volta a 0
	unsigned int nextIndex = (circBuffer->head + 1) % circBuffer->bufferSize;

	if (nextIndex == circBuffer->tail) {
		// Buffer está cheio, voltar ao início
		circBuffer->tail = (circBuffer->tail + 1) % circBuffer->bufferSize;
	}

	// Inserir o elemento no próximo índice disponível
	strcpy(circBuffer->buffer + circBuffer->head * sizeof(char), data);
	circBuffer->head = nextIndex;

	return 1;
}
