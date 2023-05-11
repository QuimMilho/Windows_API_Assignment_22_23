#pragma once

#include "Handler.h"

// Códigos dos comandos
#define DONE 0
#define NO_EXIST 1
#define NO_ARGS 2
#define NO_ENOUGH_ARGS 3
#define INV_ARGS 4
#define CMD_ERROR 5

// Origem dos comandos
#define OPERATOR 1
#define SERVER 2

// Lê e executa os comandos!
int getCommands(THREADINFO * threadInfo, GAME_SETTINGS* gs);

// Processa e executa os comandos
int process(TCHAR* cmdStr, int origin, THREADINFO* threadInfo, GAME_SETTINGS* gs);

int execute(TCHAR* cmdName, Args* args, int nargs, int origin, THREADINFO* threadInfo, GAME_SETTINGS* gs);