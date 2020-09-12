#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <Gdiplus.h>
#include <shobjidl.h>
#include <gdiplusgraphics.h>
#include <math.h>

int x = 0, y = 0, width = 150, height = 150, delta = 5;
float angle = 0;
const float ANGLE_DELTA = 0.1;
HBITMAP bmp = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void drawRectangle(HDC hdc);
void changeRectPosition(HWND hwnd, int dx = 0, int dy = 0);
void changeImagePosition(HWND hwnd, int dx = 0, int dy = 0);
void handleKeyDown(HWND hwnd, WPARAM keyCode);
void handleMouseWheel(HWND hwnd, WPARAM wParam, LPARAM lParam);
int chooseBpm(HWND hwnd);
void drawImage(HWND hwnd, HDC hdc);
void updateScreen(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Lab 1",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
    {
        handleKeyDown(hwnd, wParam);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        handleMouseWheel(hwnd, wParam, lParam);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc, memDC;
        RECT rect;

        GetClientRect(hwnd, &rect);

        hdc = BeginPaint(hwnd, &ps);

        memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBmp);

        FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        if (!bmp) {
            drawRectangle(memDC);
        }
        else {
            drawImage(hwnd, memDC);
        }

        BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memDC, rect.left, rect.top, SRCCOPY);

        DeleteDC(memDC);
        DeleteObject(memBmp);
        DeleteObject(oldBitmap);

        EndPaint(hwnd, &ps);
    }
    case WM_SIZE: 
    {
        bmp ? changeImagePosition(hwnd) : changeRectPosition(hwnd);
        break;
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void changeAngle(bool positive)
{
    angle += positive ? ANGLE_DELTA : - ANGLE_DELTA;
}

void drawRectangle(HDC hdc) {
    RECT rect = {x, y, x + width, y + height};

    FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 0, 0)));
}

void changeRectPosition(HWND hwnd, int dx, int dy)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    x += dx;
    y += dy;

    x = x < 0 ? 0 : x;
    y = y < 0 ? 0 : y;

    x = x + width > rect.right ? rect.right - width : x;
    y = y + height > rect.bottom ? rect.bottom - height : y;

    updateScreen(hwnd);
}

void changeImagePosition(HWND hwnd, int dx, int dy)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    x += dx;
    y += dy;

    x = x < 0 ? 0 : x;
    y = y < 0 ? 0 : y;

    x = x + width > rect.right ? rect.right - width : x;
    y = y + height > rect.bottom ? rect.bottom - height : y;

    updateScreen(hwnd);
}

void updateScreen(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);;

    InvalidateRect(hwnd, &rect, true);
    UpdateWindow(hwnd);
}

void handleKeyDown(HWND hwnd, WPARAM keyCode)
{
    switch (keyCode) {
        case VK_OEM_MINUS:
        {
            changeAngle(false);
            updateScreen(hwnd);
            break;
        }
        case VK_OEM_PLUS:
        {
            changeAngle(true);
            updateScreen(hwnd);
            break;
        }
        case VK_LEFT:
        {
            bmp ? changeImagePosition(hwnd, -delta) : changeRectPosition(hwnd, -delta);
            break;
        }
        case VK_RIGHT:
        {
            bmp ? changeImagePosition(hwnd, delta) : changeRectPosition(hwnd, delta);
            break;
        }
        case VK_UP:
        {
            bmp ? changeImagePosition(hwnd, 0, -delta) : changeRectPosition(hwnd, 0, -delta);
            break;
        }
        case VK_DOWN:
        {
            bmp ? changeImagePosition(hwnd, 0, delta) : changeRectPosition(hwnd, 0, delta);
            break;
        }
        case VK_SPACE:
        {
            chooseBpm(hwnd);
        }
    }
}

void handleMouseWheel(HWND hwnd, WPARAM wParam, LPARAM lParam) 
{
    int movement = GET_WHEEL_DELTA_WPARAM(wParam) / 120 * delta;
	int key = LOWORD(wParam);

    if (key & MK_SHIFT)
    {
        bmp ? changeImagePosition(hwnd, movement) : changeRectPosition(hwnd, movement);
    }
    else
    {
        bmp ? changeImagePosition(hwnd, 0, -movement) : changeRectPosition(hwnd, 0, -movement);
    }
}

int chooseBpm(HWND hwnd)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        bmp = (HBITMAP)LoadImage(NULL, pszFilePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

                        BITMAP bitmap;
                        GetObject(bmp, sizeof(bitmap), &bitmap);

                        width = bitmap.bmWidth;
                        height = bitmap.bmHeight;

                        updateScreen(hwnd);

                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return 0;
}

void drawImage(HWND hwnd, HDC hdc) {
    HDC hdcMem;
    BITMAP bitmap;
    HGDIOBJ oldBitmap;

    hdcMem = CreateCompatibleDC(hdc);
    oldBitmap = SelectObject(hdcMem, bmp);
    
    GetObject(bmp, sizeof(bitmap), &bitmap);
    BitBlt(hdc, x, y, width, height, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);
    DeleteObject(oldBitmap);
}