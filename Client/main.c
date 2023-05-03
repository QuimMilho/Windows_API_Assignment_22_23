#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#define MAX 256

int _tmain(int argc, TCHAR* argv[]) {

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

    // Memória Partilhada

    // Obtém a handle para o ficheiro
    // CreateFile()


    // Criar o objeto FileMapping usando o handle anterior
    //HANDLE WINAPI CreateFileMapping (
    //     HANDLE hFile, // Ficheiro a usar
    //     LPSECURITY_ATTRIBUTES lpAttributes,
    //     DWORD flProtect, // flags para: escrita/leitura/execu��o
    //     DWORD dwMaximumSizeHigh, // Tamanho dado em duas DWORDS
    //     DWORD dwMaximumSizeLow, // (mais significativo e menos significativo)
    //     LPCTSTR lpName // Nome a dar ao recurso (fich. mapeado)
    //);



    // Mapear uma vista do ficheiro no seu espa�o de endereçamento
    // MapViewOfFile



    // Usa a memória partilhada atrav�s da vista (sintaxe habitual ponteiros * -> [])

    // Desmapeia a vista
    // UnmapViewOfFile()

    // Fecha a handle
    // CloseHandle()

    // Fecha a handle do ficheiro
    // CloseHandle()

    return 0;
}