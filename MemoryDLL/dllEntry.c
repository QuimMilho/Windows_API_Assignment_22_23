#include <Windows.h>
#include <tchar.h>

#include "MemoryDLL.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH: 
		_tprintf_s(_T("DLL carregada!\n"));
		break;
	case DLL_PROCESS_DETACH:
		_tprintf_s(_T("DLL libertada!\n"));
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}