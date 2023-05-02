#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#define MAX 256

#ifdef UNICODE 
#define print wprintf
#define scan wscanf
#else
#define print printf
#define scan scanf
#endif

int _tmain(int argc, TCHAR * argv[]) {

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

    return 0;
}