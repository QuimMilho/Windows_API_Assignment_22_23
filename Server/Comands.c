#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#include "Commands.h"
#include "Handler.h"
#include "MemoryDLL.h"

//Funcoes 

int getCommands(THREADINFO * threadInfo) {
	TCHAR cmd[MAX];
	do {
		_tprintf_s(_T(">"));
		fflush(stdin);
		_fgetts(cmd, MAX, stdin);
		cmd[_tcslen(cmd) - 1] = '\0';
		if (_tcslen(cmd) != 0) {
			int err = process(cmd, SERVER, threadInfo);
			if (err) {
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
				case CMD_ERROR:
					_tprintf_s(_T("Ocorreu um erro ao executar o comando!\n"));
					break;
				default:
					_tprintf_s(_T("Ocorreu um erro desconhecido ao executar o comando: %d\n"), err);
				}
			}
		}
		else {
			_tprintf_s(_T("O comando não pode estar vazio!\n"));
		}
	} while (_tcscmp(cmd, _T("sair")) != 0);
	return 0;
}

int process(TCHAR * cmdStr, int origin, THREADINFO * threadInfo) {
	TCHAR cmd[MAX] = _T("");
	int nArgs = 0, cmdStrSize = _tcslen(cmdStr), start = 0;

	for (int i = 0; i < cmdStrSize - 1; i++) {
		if (cmdStr[i] == ' ' && cmdStr[i + 1] != ' ') {
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

	if (nArgs == 0 && cmdStr[cmdStrSize - 1] != ' ') {
		int len = _tcslen(cmd);
		cmd[len] = cmdStr[cmdStrSize - 1];
		cmd[len + 1] = _T('\0');
		start = cmdStrSize;
	}

	Args * args = malloc(sizeof(Args) * nArgs);
	if (args == NULL) return CMD_ERROR;

	for (int i = 0; i < nArgs; i++) {
		_tcscpy_s(args[i].arg, 250, _T(""));
	}
	
	int arg = 0;
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

	if (cmdStr[cmdStrSize - 1] != ' ') {
		TCHAR* str = &(args[arg].arg);
		int len = _tcslen(str);
		str[len] = cmdStr[cmdStrSize - 1];
		str[len + 1] = _T('\0');
	}
	
	int err = execute(cmd, args, nArgs, origin, threadInfo);

	free(args);
	return err;
}

int execute(TCHAR* cmdName, Args* args, int nargs, int origin, THREADINFO* threadInfo) {
	switch (origin)
	{
	case SERVER:
		if (_tcscmp(cmdName, _T("pausa")) == 0) {
			SuspendThread(threadInfo->thread);
			return DONE;
		}
		else if (_tcscmp(cmdName, _T("retoma")) == 0) {
			ResumeThread(threadInfo->thread);
			return DONE;
		}
		else if (_tcscmp(cmdName, _T("recomeça")) == 0) {
			createGame(threadInfo->jogo, strcmp(args->arg[0] ? args->arg[0] : _T("2"), _T("1") == 0 ? 1 : 2), threadInfo->gs);
			return DONE;
		}
		else if (_tcscmp(cmdName, _T("sair")) == 0) {
			threadInfo->running = FALSE;
			ResumeThread(threadInfo->thread);
			return DONE;
		}
		else if (_tcscmp(cmdName, _T("define")) == 0) {
			return DONE;
		}
	case OPERATOR:
		if (_tcscmp(cmdName, _T("stop")) == 0) {
			_tprintf_s(_T("stop!"));
			return DONE;
		}
		else if (_tcscmp(cmdName, _T("obstáculo")) == 0) {
			_tprintf_s(_T("obstáculo!"));
			return DONE;
		}
		else if (_tcscmp(cmdName, _T("inverte")) == 0) {
			_tprintf_s(_T("inverte!"));
			return DONE;
		}
		else return NO_EXIST;
	default:
		_tprintf_s(_T("Um comando foi executado sem ter as permissões adequadas!"));
	}
	return DONE;
}
