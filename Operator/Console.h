#pragma once

#include <Windows.h>
#include <tchar.h>

#include "Items.h"

#define CHAR_CARRO _T('\U0001F697')
#define CHAR_OBSTACULO _T('\U0001F6A7')
#define CHAR_SAPO _T('\U0001F438')

int printTab(CHAR_INFO * tab, int * nLanes);

DWORD clear();

int getTab(CHAR_INFO* tab, JOGO * jogo);