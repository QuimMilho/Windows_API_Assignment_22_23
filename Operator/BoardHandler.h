#pragma once

#include <Windows.h>

// Thread Info
typedef struct ThreadInfo {
	HANDLE thread;		// Handle da Thread
	BOOL running;		// If thread must be running or not
	DWORD threadId;		// Thread ID
} THREADINFO;

// Board Thread
DWORD WINAPI BoardThread(LPVOID lpParam);