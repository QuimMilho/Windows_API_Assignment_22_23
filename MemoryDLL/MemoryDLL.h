#pragma once

#include <Windows.h>

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

DLL_API int totalSize();

DLL_API int createGameFile(HANDLE* hFile);

DLL_API int openGameFile(HANDLE* hFile);

DLL_API int mapGameSharedFile(HANDLE hFile, LPVOID* lpMapAddress);

DLL_API int createCommandFile(HANDLE* hFile);

DLL_API int openCommandFile(HANDLE* hFile);

DLL_API int mapCommandSharedFile(HANDLE hFile, LPVOID* lpMapAddress);

DLL_API int closeSharedFile(HANDLE* hFile, LPVOID* lpMapAddress);