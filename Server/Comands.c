#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#include "Commands.h"

#define MAX 250

// Definições

typedef struct arguments { TCHAR arg[MAX]; } Args;

// Reencaminha os comandos para onde podem ser executados
// Comandos de server: pausa, retoma, recomeça, exit
// Comandos de server + operator: stop, obstáculo, inverte
int execute(TCHAR* cmdName, Args * args, int nargs, int origin);

//Funcoes 

int getCommands() {
	TCHAR cmd[MAX];
	do {
		_tprintf_s(_T(">"));
		fflush(stdin);
		_fgetts(cmd, MAX, stdin);
		cmd[_tcslen(cmd) - 1] = '\0';
		if (_tcslen(cmd) != 0) {
			int err = process(cmd, SERVER);
			switch (err) {
			case NO_EXIST:
				_tprintf_s(_T("Esse comando não existe!\n"));
				break;
			case NO_ARGS:
				_tprintf_s(_T("Não introduziste nenhum argumento!\n"));
				break;
			case NO_ENOUGH_ARGS:
				_tprintf_s(_T("Introduziste um número inválido de argumentos!\n"));
				break;
			case INV_ARGS:
				_tprintf_s(_T("Introduziste argumentos inválidos!\n"));
				break;
			case ERROR:
				_tprintf_s(_T("Ocorreu um erro ao executar o comando!\n"));
				break;
			}
		}
		else {
			_tprintf_s(_T("O comando não pode estar vazio!\n"));
		}
	} while (_tcscmp(cmd, _T("exit")) != 0);
}

int process(TCHAR * cmdStr, int origin) {
	// Nome do comando
	TCHAR cmd[MAX] = _T("");
	// N de argumentos
	int nArgs = 0, cmdStrSize = _tcslen(cmdStr), start = 0;

	// Percorre a string introduzida
	for (int i = 0; i < cmdStrSize - 1; i++) {
		// Caso char seja um espaço e o a seguir diferente de espaço
		if (cmdStr[i] == ' ' && cmdStr[i + 1] != ' ') {
			// Caso já tenha o nome do comando completo aumenta o número de argumentos
			// Caso contrário adiciona o caracter ao nome do comando.
			if (cmd[0] != '\0') {
				if (nArgs == 0) start = i;
				nArgs++;
			}
		}
		else if (nArgs == 0) {
			int len = _tcslen(cmd);
			cmd[len] = cmdStr[i];
			cmd[len + 1] = _T('\0');
		}
	}

	// Caso não haja argumentos vai buscar o último caracter no caso de ser diferente de espaço
	// para não haver perdas
	if (nArgs == 0 && cmdStr[cmdStrSize - 1] != ' ') {
		int len = _tcslen(cmd);
		cmd[len] = cmdStr[cmdStrSize - 1];
		cmd[len + 1] = _T('\0');
		start = cmdStrSize;
	}

	// Alocação da array dos Args do comando
	Args * args = malloc(sizeof(Args) * nArgs);
	if (args == NULL) return ERROR;

	for (int i = 0; i < nArgs; i++) {
		_tcscpy_s(args[i].arg, 250, _T(""));
	}
	
	//Procura os argumentos
	int arg = 0;
	// Percorre a string introduzida a partir do final do nome do comando
	for (int i = start + 1; i < cmdStrSize - 1; i++) {
		if (cmdStr[i] != ' ') {
			TCHAR * str = &(args[arg].arg);
			int len = _tcslen(str);
			str[len] = cmdStr[i];
			str[len + 1] = '\0';
		}
		else if (cmdStr[i + 1] != ' ' && args[arg].arg != '\0') {
			arg++;
		}
	}

	// Caso não haja argumentos vai buscar o último caracter no caso de ser diferente de espaço
	// para não haver perdas
	if (cmdStr[cmdStrSize - 1] != ' ') {
		TCHAR* str = &(args[arg].arg);
		int len = _tcslen(str);
		str[len] = cmdStr[cmdStrSize - 1];
		str[len + 1] = _T('\0');
	}
	
	// Executa o comando
	int err = execute(cmd, args, nArgs, origin);

	// Liberta a memória
	free(args);
	return err;
}

int execute(TCHAR* cmdName, TCHAR* args, int nargs, int origin) {
	switch (origin)
	{
	case SERVER:

	case OPERATOR:

		break;
	}
	return DONE;
}