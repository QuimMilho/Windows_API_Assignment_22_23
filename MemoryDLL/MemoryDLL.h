#pragma once

#include <Windows.h>
#include <tchar.h>

#include "Items.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define SHARED_SERVER_MEMORY _T("ServerSapoShared")

#define SHARED_COMMAND_MEMORY _T("ServerSapoCommands")
#define SHARED_COMMAND_BUFFER_CHARS 100

DLL_API int createGameFile(HANDLE* hFile);

DLL_API int openGameFile(HANDLE* hFile);

DLL_API int mapGameSharedFile(HANDLE hFile, LPVOID* lpMapAddress, DWORD permissions);

DLL_API int copyGame(JOGO * from, JOGO * to);

DLL_API int createCommandFile(HANDLE* hFile);

DLL_API int openCommandFile(HANDLE* hFile);

DLL_API int mapCommandSharedFile(HANDLE hFile, LPVOID* lpMapAddress);

DLL_API int closeSharedFile(HANDLE* hFile, LPVOID* lpMapAddress);

DLL_API int StartCircularBuffer(LPVOID address);

DLL_API int WriteCircularBufferChar(LPVOID address, TCHAR* cmdStr);

DLL_API int ReadCircularBufferChar(LPVOID address, TCHAR* cmdStr, int max);