#include "MemoryDLL.h"
#include "Items.h"

#include <tchar.h>

// Tamanho m�ximo do jogo

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

// Transforma a mem�ria partilhada em elementos organizados

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

	// n do n�vel e de faixas
	buffer[bufPos++] = jogo->level;
	buffer[bufPos++] = jogo->nLanes;

	// Total de carros
	buffer[bufPos++] = jogo->totalDeCarros;

	//Percorre as faixas e adiciona os carros � mem�ria partilhada.
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

	// Atribui os valores dos obst�culos
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

	// Total de carros
	jogo->totalDeCarros = buffer[bufPos++];

	// Aloca uma array com nLanes elementos que s�o o n�mero de carros por faixa.
	jogo->direcao = malloc(sizeof(int) * jogo->nLanes);
	if (jogo->direcao == NULL) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria para o n�mero de carros.\n"));
		destroyGame(jogo);
		return 1;
	}

	//Percorre as faixas e alloca a mem�ria para os carros, adicionando os carros dessa faixa.
	for (int i = 0; i < jogo->nLanes; i++) {

		// Vai buscar a dire��o da faixa
		jogo->direcao[i] = (int)buffer[bufPos++];
	}

	// Alocada mem�ria para os carros
	jogo->carros = malloc(sizeof(CARRO) * jogo->totalDeCarros);
	if (jogo->carros == NULL && jogo->totalDeCarros != 0) {
		_tprintf_s(_T("Ocorreu um erro ao alocar a mem�ria para os carros.\n"));
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

	// Aloca os obst�culos na mem�ria
	jogo->obstaculos = malloc(sizeof(OBSTACULO) * jogo->nObstaculos);
	if (jogo->obstaculos == NULL && jogo->obstaculos != 0) {
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
	free(jogo->direcao);
	return 0;
}

// Command Files

int createCommandFile(HANDLE* hFile) {
	(*hFile) = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,  
		sizeof(TCHAR) * SHARED_COMMAND_BUFFER_CHARS + sizeof(DWORD) * 2, SHARED_COMMAND_MEMORY);
	if (*hFile == NULL) return 1;
	return 0;
}

int openCommandFile(HANDLE* hFile) {
	(*hFile) = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_COMMAND_MEMORY);
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

// SHARED_COMMAND_BUFFER_BYTES Bytes + int in + int out

int StartCircularBuffer(LPVOID address) {

	DWORD32* buffer = (DWORD32*)address;

	// Inicializar os elementos do buffer circular a 0
	for (int i = 0; i < SHARED_COMMAND_BUFFER_CHARS; i++) {
		buffer[i] = 0;
	}

	buffer[SHARED_COMMAND_BUFFER_CHARS] = 0;
	buffer[SHARED_COMMAND_BUFFER_CHARS + 1] = 0;

	return 0;
}

// Adiciona o elemento data ao buffer circular
int WriteCircularBufferChar(LPVOID address, TCHAR* cmdStr) {

	int size = _tcslen(cmdStr);

	if (size > SHARED_COMMAND_BUFFER_CHARS) {
		_tprintf_s(_T("A string de caracteres introduzida é demasiado grande!\n"));
		return 1;
	}

	DWORD32* buffer = (DWORD32*)address;
	TCHAR* strBuf = (TCHAR*)address;

	// Obter os índices
	int* in = &(buffer[SHARED_COMMAND_BUFFER_CHARS + 1]), * out = &(buffer[SHARED_COMMAND_BUFFER_CHARS + 2]);

	*in = (*out + 1) % SHARED_COMMAND_BUFFER_CHARS;
	*out = (*in + size) % SHARED_COMMAND_BUFFER_CHARS - 1;


	// Copia os caracteres de cmdStr para o buffer circular
	for (int i = 0; i < size; i++) {
		strBuf[((*in) + i) % SHARED_COMMAND_BUFFER_CHARS] = cmdStr[i];
	}

	return 0;
}

int ReadCircularBufferChar(LPVOID address, TCHAR* cmdStr, int max) {

	DWORD32* buffer = (DWORD32*)address;
	TCHAR* strBuf = (TCHAR*)address;

	// Iterar os índices
	int in = buffer[SHARED_COMMAND_BUFFER_CHARS + 1], out = buffer[SHARED_COMMAND_BUFFER_CHARS + 2];

	// Calcula o tamanho dos dados no buffer circular
	int size = in < out ? out - in : SHARED_COMMAND_BUFFER_CHARS - in + out;

	// Correção do tamanho real
	size++; 

	if (size + 1 > max) {
		_tprintf_s(_T("A string introduzida não aguenta o tamanho escrito no buffer!\n"));
		return 1;
	}

	// Copia os caracteres do buffer circular para a output string (cmdStr)
	for (int i = 0; i < size; i++) {
		cmdStr[i] = strBuf[(in + i) % SHARED_COMMAND_BUFFER_CHARS];
	}

	// Termina a string com \0
	cmdStr[size] = _T('\0');

	return 0;
}

DLL_API int WriteCircularBufferDWORD(LPVOID address, DWORD32 dword) {
	DWORD32* buffer = (DWORD32*)address;

	// Incrementar os índices
	int* in = &(buffer[SHARED_COMMAND_BUFFER_CHARS + 1]), * out = &(buffer[SHARED_COMMAND_BUFFER_CHARS + 2]);

	*in = (*out + 1) % SHARED_COMMAND_BUFFER_CHARS;
	*out = (*in);

	buffer[*in] = dword;

	return 0;
}


DLL_API int ReadCircularBufferDWORD(LPVOID address, DWORD32* dword) {
	DWORD32* buffer = (DWORD32*)address;

	// Iterar o índice
	int in = buffer[SHARED_COMMAND_BUFFER_CHARS + 1];

	// Colocar o que lemos para a string
	*dword = buffer[in];

	return 0;
}

int GetCommandErrorSTR(TCHAR* str, int size, int err) {
	int wrote = 0;
	switch (err) {
	case NO_EXIST:
		wrote = _tcscpy_s(str, size, _T("Esse comando não existe!"));
		break;
	case NO_ARGS:
		wrote = _tcscpy_s(str, size, _T("Não introduziste nenhum argumento!"));
		break;
	case NO_ENOUGH_ARGS:
		wrote = _tcscpy_s(str, size, _T("Introduziste um número inválido de argumentos!"));
		break;
	case INV_ARGS:
		wrote = _tcscpy_s(str, size, _T("Introduziste argumentos inválidos!"));
		break;
	case CMD_ERROR:
		wrote = _tcscpy_s(str, size, _T("Ocorreu um erro ao executar o comando!"));
		break;
	default:
		return 1;
	}

	return wrote;
}