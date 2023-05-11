#pragma once

#include <Windows.h>
#include <tchar.h>

#include "Items.h"

#define CHAR_CARRO _T('C')
#define CHAR_OBSTACULO _T('|')

int printTab(CHAR_INFO * tab, int nLanes);

DWORD clear();
DWORD clearLine(int y);

int getTab(CHAR_INFO* tab, JOGO * jogo);

int setCursorPosition(int x, int y);