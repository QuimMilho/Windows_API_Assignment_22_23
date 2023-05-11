#pragma once

#include <Windows.h>

#include "Items.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define SHARED_SERVER_MEMORY _T("ServerSapoShared")
#define SHARED_SERVER_TOTAL_BYTES 1096

#define SHARED_COMMAND_MEMORY _T("ServerSapoCommands")
#define SHARED_COMMAND_TOTAL_BYTES 104

DLL_API int totalSize();

DLL_API int createGameFile(HANDLE* hFile);

DLL_API int openGameFile(HANDLE* hFile);

DLL_API int mapGameSharedFile(HANDLE hFile, LPVOID* lpMapAddress, DWORD permissions);

DLL_API int saveStructures(LPVOID address, JOGO* jogo);

DLL_API int toStructures(LPVOID address, JOGO* jogo);

DLL_API int destroyGame(JOGO* jogo);

DLL_API int createCommandFile(HANDLE* hFile);

DLL_API int openCommandFile(HANDLE* hFile);

DLL_API int mapCommandSharedFile(HANDLE hFile, LPVOID* lpMapAddress);

DLL_API int closeSharedFile(HANDLE* hFile, LPVOID* lpMapAddress);

//==| BUFFER CIRCULAR |==
// Definição da estrutura do buffer circular
typedef struct CircularBuffer {

	CircularBuffer* circBuffer;
	char commands[SHARED_COMMAND_TOTAL_BYTES];

} CircularBuffer;

CircularBuffer* CreateCircularBuffer(int bufferSize);
void DestroyCircularBuffer(CircularBuffer* circBuffer);
int PushToCircularBuffer(CircularBuffer* circBuffer, const char* data);
int PopFromCircularBuffer(CircularBuffer* circBuffer, char* data, int dataSize);