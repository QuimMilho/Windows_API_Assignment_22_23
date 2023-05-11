#pragma once

#include "Handler.h"

// Origem dos comandos
#define OPERATOR 1
#define SERVER 2

// Lê e executa os comandos!
int getCommands(THREADINFO * threadInfo);

// Processa e executa os comandos
int process(TCHAR* cmdStr, int origin, THREADINFO* threadInfo);