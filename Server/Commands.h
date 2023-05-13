#pragma once

#include "Handler.h"

// Origem dos comandos
#define OPERATOR 1
#define SERVER 2
#define MAX 250

typedef struct arguments { TCHAR arg[MAX]; } Args;

// Lê e executa os comandos!
int getCommands(THREADINFO * threadInfo);

// Processa e executa os comandos
int process(TCHAR* cmdStr, int origin, THREADINFO* threadInfo);


int execute(TCHAR* cmdName, Args* args, int nargs, int origin, THREADINFO* threadInfo);