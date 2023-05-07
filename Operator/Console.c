#include "Console.h"
#include "Items.h"

void setUnicodeChar(CHAR_INFO* c, TCHAR v);
void setAsciiChar(CHAR_INFO* c, TCHAR v);

#ifdef UNICODE
#define _tsetCharInfoChar setUnicodeChar
#else
#define _tsetCharInfoChar setAsciiChar
#endif

DWORD clear() {
	// Pede instancia da consola
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console == NULL) {
		_tprintf_s(_T("Erro ao obter instância da consola."));
		return 1;
	}

	// Informação sobre a consola
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(console, &consoleInfo);

	DWORD charsWritten;

	FillConsoleOutputCharacter(console, _T(' '), consoleInfo.dwSize.X * consoleInfo.dwSize.Y,
		(COORD) { 0, 0 }, & charsWritten);
	FillConsoleOutputAttribute(console, consoleInfo.wAttributes, consoleInfo.dwSize.X * consoleInfo.dwSize.Y,
		(COORD) { 0, 0 }, & charsWritten);

	return charsWritten;
}

int printTab(CHAR_INFO* tab, int nLanes) {
	// Pede instancia da consola
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console == NULL) {
		_tprintf_s(_T("Erro ao obter instância da consola."));
		return 1;
	}

	// Informação sobre a consola
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(console, &consoleInfo);

	// Calcula o centro da consola e define como posição do topo.
	// Calcula a altura da àrea a imprimir
	COORD pos = { 0, 0 }, size = { 20, nLanes + 2 };

	// Tamanho do retângulo a imprimir
	SMALL_RECT rect = { pos.X, pos.Y, pos.X + 19, pos.Y + 9 };

	// Imprime na consola sem usar o cursor.
	WriteConsoleOutput(console, tab, size, pos, &rect);
	_tprintf_s(_T("%d"), 2);
	return 0;
}

// Maping:
// int n Sapos - Sapos (1 ou 2) - int level - int nFaixas - int nCarrosF1 - CarrosF1 - int nCarrosF2 - CarrosF2 - ... 
//		- int nObstaculos - Obstaculos
int getTab(CHAR_INFO * tab, LPVOID tabInfo, int * nLanes, int * level) {
	// Define os caracteres para os default
	for (int i = 0; i < ((*nLanes) + 2) * 20; i++) {
		if (i < 20) {
			// BACKGROUND COLOR BLUE, FOREGROUND COLOR WHITE
			tab[i].Attributes = BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
			_tsetCharInfoChar(&tab[i], _T(' '));
		}
		else if (i >= ((*nLanes) + 1) * 20) {
			// BACKGROUND COLOR BLUE, FOREGROUND COLOR WHITE
			tab[i].Attributes = BACKGROUND_BLUE;
			_tsetCharInfoChar(&tab[i], _T(' '));
		}
		else {
			// BACKGROUND COLOR BLACK, FOREGROUND COLOR WHITE
			tab[i].Attributes = (BACKGROUND_BLUE & BACKGROUND_GREEN & BACKGROUND_RED) |
				(FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
			_tsetCharInfoChar(&tab[i], _T(' '));
		}
	}
}

void setUnicodeChar(CHAR_INFO * c, TCHAR v) {
	c->Char.UnicodeChar = v;
}

void setAsciiChar(CHAR_INFO* c, TCHAR v) {
	c->Char.AsciiChar = v;
}