#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>
#include <comdef.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const int COUNT_OF_COLUMN = 2;
const int COUNT_OF_ROW = 3;

int textHeight;
int fontHeight;
int windowWidth;
int windowHeight;
int columnWidth, rowHeight;
int countOfWords = 0;
const wchar_t* buf[COUNT_OF_ROW][COUNT_OF_COLUMN] = { { L"Unic odes tringUnicode tringUn icodest ringU nicodestring ", L"Unicode string" }, { L"Ilya", L"Ilya with lab 2" }, { L"Unicode string", L"Ilya" } };

int CountWords(const char* str)
{
    bool inSpaces = true;
    int numWords = 0;

    while (*str != '\0')
    {
        if (std::isspace(*str))
        {
            inSpaces = true;
        }
        else if (inSpaces)
        {
            numWords++;
            inSpaces = false;
        }

        ++str;
    }

    return numWords;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Window Class";

    WNDCLASS windowClass = { };

    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = CLASS_NAME;

    RegisterClass(&windowClass);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Lab 2",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    windowWidth = clientRect.right - clientRect.left;
    windowHeight = clientRect.bottom - clientRect.top;

    for (int i = 0; i < COUNT_OF_ROW; i++) {
        for (int j = 0; j < COUNT_OF_COLUMN; j++) {
            _bstr_t str(buf[i][j]);
            
            int temp = CountWords(str);
            if (temp > countOfWords) {
                countOfWords = temp;
            }
        }
    }

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void fillWindow(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    fontHeight = (windowWidth + windowHeight) / (countOfWords * COUNT_OF_ROW * COUNT_OF_COLUMN);

    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    columnWidth = windowWidth / COUNT_OF_COLUMN;
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, pen);
    HFONT font;
    
    for (int i = 0; i < COUNT_OF_ROW; i++) {
        for (int j = 0; j < COUNT_OF_COLUMN; j++) {
            if (j % 2 == 0) {
                font = CreateFont(fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, VARIABLE_PITCH, NULL);
                SelectObject(hdc, font);
            }
            else {
                font = CreateFont(fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    NONANTIALIASED_QUALITY, VARIABLE_PITCH, NULL);
                SelectObject(hdc, font);
            }
            RECT sell;
            SetRect(&sell, columnWidth * j, rowHeight * i, columnWidth * (j + 1), rowHeight * (i + 1));
            SetBkMode(hdc, TRANSPARENT);
            Rectangle(hdc, columnWidth * j, rowHeight * i, columnWidth * (j + 1), rowHeight * (i + 1));
            DrawText(hdc, buf[i][j], -1, &sell, DT_CENTER| DT_WORDBREAK|DT_WORD_ELLIPSIS);
            SetBkMode(hdc, OPAQUE);
            DeleteObject(font);
        }
    }
    DeleteObject(pen);
    EndPaint(hwnd, &ps);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_PAINT:
    {
        fillWindow(hwnd, uMsg, wParam, lParam);
        return 0;
    }
    case WM_SIZE: 
    {
        windowWidth = LOWORD(lParam);
        windowHeight = HIWORD(lParam);

        columnWidth = windowWidth / COUNT_OF_COLUMN;
        rowHeight = windowHeight / COUNT_OF_ROW;

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}