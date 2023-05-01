#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#define MAX 256

int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE 
    if (_setmode(_fileno(stdin), _O_WTEXT) == -1) {
        _tprintf(TEXT("Failed to set input mode.\n"));
        return 1;
    }
    if (_setmode(_fileno(stdout), _O_WTEXT) == -1) {
        _tprintf(TEXT("Failed to set output mode.\n"));
        return 1;
    }
#endif

    // Mem�ria Partilhada dsad sad as dsadsadas dsad asd

    // Obtém a handle para o ficheiro
    // CreateFile()


    // Criar o objeto FileMapping usando o handle anterior
    HANDLE WINAPI CreateFileMapping (
         HANDLE hFile, // Ficheiro a usar
         LPSECURITY_ATTRIBUTES lpAttributes,
         DWORD flProtect, // flags para: escrita/leitura/execu��o
         DWORD dwMaximumSizeHigh, // Tamanho dado em duas DWORDS
         DWORD dwMaximumSizeLow, // (mais significativo e menos significativo)
         LPCTSTR lpName // Nome a dar ao recurso (fich. mapeado)
    );



    // Mapear uma vista do ficheiro no seu espaço de endere�amento
    // MapViewOfFile



    // Usa a mem�ria partilhada através da vista (sintaxe habitual ponteiros * -> [])


    Carlos da Maia


    // Desmapeia a vista
    // UnmapViewOfFile()

    // Fecha a handle
    // CloseHandle()

    // Fecha a handle do ficheiro
    // CloseHandle()

    return 0;
}