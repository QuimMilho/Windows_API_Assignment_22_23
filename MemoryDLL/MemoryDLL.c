#include "MemoryDLL.h"
#include "Items.h"

#include <tchar.h>

int createGameFile(HANDLE * hFile) {
	(*hFile) = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(JOGO),
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

int copyGame(JOGO* to, JOGO* from) {
	to->nSapos = from->nSapos;
	to->nLanes = from->nLanes;
	to->level = from->level;
	to->nObstaculos = from->nObstaculos;
	to->totalDeCarros = from->totalDeCarros;

	for (int i = 0; i < 2; i++) {
		to->sapos[i].lastMoved = from->sapos[i].lastMoved;
		to->sapos[i].x = from->sapos[i].x;
		to->sapos[i].y = from->sapos[i].y;
	}

	for (int i = 0; i < from->nLanes; i++) {
		to->direcao[i] = from->direcao[i];
	}

	for (int i = 0; i < from->totalDeCarros; i++) {
		to->carros[i].vel = from->carros[i].vel;
		to->carros[i].x = from->carros[i].x;
		to->carros[i].y = from->carros[i].y;
	}

	for (int i = 0; i < from->nObstaculos; i++) {
		to->obstaculos[i].x = from->obstaculos[i].x;
		to->obstaculos[i].y = from->obstaculos[i].y;
	}
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