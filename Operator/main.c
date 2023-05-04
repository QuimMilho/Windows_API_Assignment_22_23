#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#define MAX 256

int _tmain(int argc, TCHAR* argv[]) {

    char comString[50], com[30], arg1[10], arg2[10];
    int nArgs;

//-----------------------Unicode----------------------
#ifdef UNICODE 
    if (_setmode(_fileno(stdin), _O_WTEXT) == -1) {
        _tprintf(_T("Failed to set input mode.\n"));
        return 1;
    }
    if (_setmode(_fileno(stdout), _O_WTEXT) == -1) {
        _tprintf(_T("Failed to set output mode.\n"));
        return 1;
    }
#endif
//-----------------------/Unicode---------------------
    

    //1 - Obtém handle para o objecto memória partilhada
    //
    //    HANDLE WINAPI OpenFileMapping(
    //          DWORD dwDesiredAccess, // acesso pretendido
    //          BOOL bInheritHandle,
    //          LPCTSTR lpName // nome dado ao recurso (ficheiro mapeado)
    //    );

    //2 – Mapeia uma vista da memória partilhada no seu espaço de endereçamento
    //
    //    LPVOID WINAPI MapViewOfFile (
    //          HANDLE hFileMappingObject, // Handle do ficheiro mapeado
    //          DWORD dwDesiredAccess, // Flags de acesso (ler, escrever)
    //          DWORD dwFileOffsetHigh, // Início dentro do bloco pretendido
    //          DWORD dwFileOffsetLow, // dentro do ficheiro (+signific., -signific.)
    //          SIZE_T dwNumberOfBytesToMap // Tamanho da view pretendida
    //    );

    //3 – Usa a memória partilhada através da vista(ponteiros ou[])

    //4 – Desmapeia a vista, eventualmente no final do processo
    //    LPVOID WINAPI UnmapViewOfFile(
    // 
    //          // Ponteiro para o inicio da view (vem de MapViewOfFile())
    //          LPCVOID lpBaseAddress
    //    );

    //5 – Fecha o handle
    //    CloseHandle()

    printf("|====================| Operador |====================|\n");

    printf("Comandos Disponíveis (<> -> Obrigatório, [] -> Opcional):\n    stop <tempo em s>: Parar o movimento dos carros durante um determinado período de tempo (especificado em segundos).\n    obstaculo <linha>[coluna]: Inserir obstáculos nas faixas de rodagem que são intransponíveis pelos carros e pelos sapos.\n    inverte <linha>: Inverter o sentido da marcha de determinada faixa de rodagem.");

    fflush(stdin);

    while (fgets(comString, sizeof(comString), stdin) == 0) {

        printf("\nComando Inválido... Comandos Disponíveis(<> -> Obrigatório, [] -> Opcional):\n    stop <tempo em s>: Parar o movimento dos carros durante um determinado período de tempo(especificado em segundos).\n    obstaculo <linha>[coluna]: Inserir obstáculos nas faixas de rodagem que são intransponíveis pelos carros e pelos sapos.\n    inverte <linha>: Inverter o sentido da marcha de determinada faixa de rodagem.\nInsira o comando: ");
        fflush(stdin);

    }

    nArgs = sscanf_s(comString, "%s %s %s", com, (unsigned)_countof(com), arg1, (unsigned)_countof(arg1), arg2, (unsigned)_countof(arg2));

    printf("Nº Parâmetros: %d\n", nArgs);
    printf("Com: %s", com);
    printf("arg1: %s", arg1);
    printf("arg2: %s", arg2);

    return 0;
}