#include <windows.h>
#include <tchar.h>

#define OP_SAPO 1
#define OP_CARRO 2
#define OP_OBS 3

#define WIDTH 1024
#define HEIGHT 768

int currentScore = 0;
int highScore = 0;
int timeS = 0;
int nFaixas = 6;

// Bitmaps
HBITMAP hFrogBitmap;
HBITMAP hCarBitmap;
HBITMAP hTreeBitmap;
BITMAP frogBitmap;
BITMAP carBitmap;
BITMAP treeBitmap;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void printIcon(HWND hwnd, HDC hdc, int x, int y, int flag);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class
    const wchar_t CLASS_NAME[] = L"MyGameClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    // Load the icon from the .ico file
    HICON hIcon = static_cast<HICON>(LoadImage(NULL, L"frog.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));

    if (hIcon)
    {
        // Set the loaded icon as the application icon
        wc.hIcon = hIcon;
    }

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Frogger",
        WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX), // Remove the resizable border and maximize button
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
        return 0;

    // Display the window
    ShowWindow(hwnd, nCmdShow);

    // Enter the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    wchar_t scoreValue[20];
    wchar_t highScoreValue[20];
    wchar_t timeValue[30];
    WCHAR buffer[MAX_PATH];
    DWORD length = GetCurrentDirectoryW(MAX_PATH, buffer);

    switch (uMsg)
    {
    case WM_CREATE:
        // Load the bitmaps
        hFrogBitmap = (HBITMAP)LoadImage(NULL, _T("frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        GetObject(hFrogBitmap, sizeof(frogBitmap), &frogBitmap);


        hCarBitmap = (HBITMAP)LoadImage(NULL, _T("carL-R.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        GetObject(hTreeBitmap, sizeof(treeBitmap), &treeBitmap);

        hTreeBitmap = (HBITMAP)LoadImage(NULL, _T("tree.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        GetObject(hCarBitmap, sizeof(carBitmap), &carBitmap);
        return 0;

    case WM_DESTROY:
        // Cleanup resources
        DeleteObject(hFrogBitmap);
        DeleteObject(hCarBitmap);
        DeleteObject(hTreeBitmap);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Get the client area of the window
        RECT rect;
        GetClientRect(hwnd, &rect);

        // Calculate the heights of the sections
        int windowHeight = rect.bottom - rect.top;
        int scoreHeight = windowHeight / 10;
        int bottomHeight = windowHeight / 10;
        int gameHeight = windowHeight - scoreHeight - bottomHeight;
        int margin = 10;
        
        // Draw the score section
        RECT scoreRect = { 0, 0, rect.right, scoreHeight };
        HBRUSH scoreBrush = CreateSolidBrush(RGB(0, 0, 0)); // Black color brush
        FillRect(hdc, &scoreRect, scoreBrush);

        // Draw the score text in the left side of the score section
        RECT leftScoreRect = { margin, margin, rect.right / 2 - margin, scoreHeight };
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, L"SCORE", -1, &leftScoreRect, DT_LEFT | DT_TOP | DT_SINGLELINE);

        // Draw the score value aligned vertically with the SCORE text
        RECT leftScoreValueRect = { margin, 0, rect.right / 2 - margin, scoreHeight };
        swprintf(scoreValue, sizeof(scoreValue), L"%d", currentScore); // Convert score to string
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, scoreValue, -1, &leftScoreRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);


        // Draw the high score text in the right side of the score section
        RECT rightScoreRect = { rect.right / 2 + margin, margin, rect.right - margin, scoreHeight };
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, L"HIGH SCORE", -1, &rightScoreRect, DT_RIGHT | DT_TOP | DT_SINGLELINE);

        // Draw the high score value below the high score text
        RECT rightScoreValueRect = { rect.right / 2 + margin, scoreHeight / 2, rect.right - margin, scoreHeight };

        // Convert high score to string
        swprintf(highScoreValue, sizeof(highScoreValue), L"%d", highScore);

        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, highScoreValue, -1, &rightScoreValueRect, DT_RIGHT | DT_TOP | DT_SINGLELINE);

        // Draw the game section
        RECT gameRect = { 0, scoreHeight, rect.right, scoreHeight + gameHeight };
        int totalFaixas = 2 + nFaixas;
        int faixaHeight = gameHeight / totalFaixas;

        // Draw the rectangles inside the game section
        for (int i = 0; i < totalFaixas; i++)
        {
            int faixaTop = gameRect.top + (faixaHeight * i);
            int faixaBottom = gameRect.top + (faixaHeight * (i + 1));

            RECT faixaRect = { gameRect.left, faixaTop, gameRect.right, faixaBottom };

            // Set the brush color based on the position of the rectangle
            HBRUSH faixaBrush;
            if (i == 0 || i == totalFaixas - 1)
            {
                faixaBrush = CreateSolidBrush(RGB(0, 128, 0)); // Dark green color for first and last rectangle
            }
            else
            {
                faixaBrush = CreateSolidBrush(RGB(192, 192, 192)); // Gray color for other rectangles
            }

            // Fill the rectangle with the specified color
            FillRect(hdc, &faixaRect, faixaBrush);

            // Draw a black border around the rectangle
            HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            SelectObject(hdc, hBorderPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, faixaRect.left, faixaRect.top, faixaRect.right, faixaRect.bottom);

            // Cleanup
            DeleteObject(faixaBrush);
            DeleteObject(hBorderPen);
        }

        // Draw the bottom section
        RECT bottomRect = { 0, scoreHeight + gameHeight, rect.right, windowHeight };
        HBRUSH bottomBrush = CreateSolidBrush(RGB(0, 0, 0)); // Black color brush
        FillRect(hdc, &bottomRect, bottomBrush);

        // Draw the time text in the right side of the bottom section with a margin
        RECT timeRect = { rect.right / 2 + margin, windowHeight - scoreHeight, rect.right - margin, windowHeight };
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        swprintf(timeValue, sizeof(timeValue), L"TIME: %ds", timeS); // Format the time value
        DrawText(hdc, timeValue, -1, &timeRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        // Print the frog in the center of the bottom dark green rectangle
        printIcon(hwnd, hdc, 7, 15, OP_SAPO);

        EndPaint(hwnd, &ps);
        return 0;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void printIcon(HWND hwnd, HDC hdc, int x, int y, int flag)
{

    MessageBox(hwnd, _T("Teste!"), _T("Error"), MB_OK | MB_ICONERROR);
    // Calculate the size of each cell in the fictional grid
    int cellWidth = WIDTH / 16;
    int cellHeight = HEIGHT / 7;

    // Calculate the position within the window based on the grid coordinates
    int posX = x * cellWidth;
    int posY = y * cellHeight;

    HBITMAP hBitmap = nullptr;

    // Select the appropriate bitmap based on the flag
    if (flag == OP_SAPO)
        hBitmap = hFrogBitmap;

    else if (flag == OP_CARRO)
        hBitmap = hCarBitmap;

    else if (flag == OP_OBS)
        hBitmap = hTreeBitmap;

    // Create a memory device context compatible with the window's DC
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    // Draw the icon at the specified position
    BOOL result = BitBlt(hdc, posX, posY, cellWidth, cellHeight, memDC, 0, 0, SRCCOPY);
    if (result) {
        MessageBox(hwnd, _T("TRUE"), _T("Error"), MB_OK | MB_ICONERROR);
    } else MessageBox(hwnd, _T("FALSE"), _T("Error"), MB_OK | MB_ICONERROR);

    // Clean up resources
    SelectObject(memDC, hOldBitmap);
    DeleteDC(memDC);
}


