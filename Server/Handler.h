#pragma once

#include <Windows.h>

#ifndef HANDLER_H
#define HANDLER_H

// Thread Info

typedef struct ThreadInfo { 
	HANDLE thread;		// Handle da Thread
	BOOL running;		// If thread must be running or not
	DWORD threadId;		// Thread ID
} THREADINFO;

// Game Thread

DWORD WINAPI GameThread(LPVOID lpParam);

#endif //HANDLER_H