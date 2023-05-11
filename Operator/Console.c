#include "Console.h"
#include "Items.h"

void setUnicodeChar(CHAR_INFO* c, TCHAR v);
void setAsciiChar(CHAR_INFO* c, TCHAR v);

int getPos(int total, int x, int y);

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

int setCursorPosition(int x, int y) {
	// Pede instancia da consola
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console == NULL) {
		_tprintf_s(_T("Erro ao obter instância da consola."));
		return 1;
	}

	SetConsoleCursorPosition(console, (COORD) { x, y });

	return 0;
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
	SMALL_RECT rect = { pos.X, pos.Y, pos.X + 19, pos.Y + nLanes + 1 };

	// Imprime na consola sem usar o cursor.
	WriteConsoleOutput(console, tab, size, pos, &rect);
	return 0;
}

// Maping:
// int n Sapos - Sapos (1 ou 2) - int level - int nFaixas - int nCarrosF1 - CarrosF1 - int nCarrosF2 - CarrosF2 - ... 
//		- int nObstaculos - Obstaculos
int getTab(CHAR_INFO* tab, JOGO* jogo) {
	int totalLanes = ((jogo->nLanes) + 2) * 20;
	// Define os caracteres para os default
	for (int i = 0; i < totalLanes; i++) {
		if (i < 20) {
			// BACKGROUND COLOR BLUE, FOREGROUND COLOR WHITE
			tab[i].Attributes = BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
			_tsetCharInfoChar(tab + i, _T(' '));
		}
		else if (i >= ((jogo->nLanes) + 1) * 20) {
			// BACKGROUND COLOR BLUE, FOREGROUND COLOR WHITE
			tab[i].Attributes = BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
			_tsetCharInfoChar(tab + i, _T(' '));
		}
		else {
			// BACKGROUND COLOR BLACK, FOREGROUND COLOR WHITE
			tab[i].Attributes = (BACKGROUND_BLUE & BACKGROUND_GREEN & BACKGROUND_RED) |
				(FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
			_tsetCharInfoChar(tab + i, _T(' '));
		}
	}

	// Define os caracteres dos sapos
	if (jogo->nSapos == 2) {
		_tsetCharInfoChar(&tab[getPos(totalLanes, jogo->sapos[0].x, jogo->sapos[0].y)], _T('1'));
		_tsetCharInfoChar(&tab[getPos(totalLanes, jogo->sapos[1].x, jogo->sapos[1].y)], _T('2'));
	}
	else {
		_tsetCharInfoChar(&tab[getPos(totalLanes, jogo->sapos[0].x, jogo->sapos[0].y)], _T('S'));
	}

	for (int i = 0; i < jogo->totalDeCarros; i++) {
		if (jogo->carros[i].x != 20)
		_tsetCharInfoChar(&tab[getPos(totalLanes, jogo->carros[i].x, jogo->carros[i].y)], CHAR_CARRO);
	}

	for (int i = 0; i < jogo->nObstaculos; i++) {
		_tsetCharInfoChar(&tab[getPos(totalLanes, jogo->obstaculos[i].x, jogo->obstaculos[i].y)], CHAR_CARRO);
	}
}

void setUnicodeChar(CHAR_INFO * c, TCHAR v) {
	c->Char.UnicodeChar = v;
}

void setAsciiChar(CHAR_INFO* c, TCHAR v) {
	c->Char.AsciiChar = v;
}

int getPos(int total, int x, int y) {
	return total - (y + 1) * 20 + x;
}