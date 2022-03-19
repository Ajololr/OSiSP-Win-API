#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <math.h>

#define PI 3.14

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void PrintEllipseStamp(HDC hDc, LPCWSTR str, POINT radius, POINT position, int fontHeight);
void PrintRectangleStamp(HDC hDc, LPCWSTR str, POINT sides, POINT position, int fontHeight);
void Rotate(float radians, POINT position, HDC destDC, HDC sourceDC, HBITMAP hBitmap);
void DisplayText(HWND hWnd);

int startWidth = 0;
int startHeight = 0;

int windowWidth = 0;
int windowHeight = 0;

LPCWSTR rectText = L"Ilya Androsov";
LPCWSTR roundText = L"װאכûזאגמה מאûהגכמא û מכûגהמ אûהכגמ";

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

	HDC hDc = GetDC(hwnd);

	RECT rect;
	GetWindowRect(hwnd, &rect);
	startWidth = rect.right - rect.left;
	startHeight = rect.bottom - rect.top;

	DisplayText(hwnd);

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
	case WM_SIZE:
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);

		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case WM_PAINT:
	{
		DisplayText(hwnd);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void PrintEllipseStamp(HDC hDc, LPCWSTR str, POINT radius, POINT position, int fontHeight)
{
	size_t textLength = wcslen(rectText);

	FLOAT angle = PI;
	FLOAT delta = 2 * PI / textLength;

	HDC sourceDC = CreateCompatibleDC(hDc);
	HBITMAP bmp = CreateCompatibleBitmap(hDc, 0, 0);
	HBITMAP oldBmp = (HBITMAP)SelectObject(sourceDC, bmp);

	HFONT hFont = CreateFontA(fontHeight, 0, 0, 0, 0, 0, 0, 0, 
		OEM_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		FF_ROMAN, NULL);

	SelectObject(sourceDC, hFont);

	for (INT i = 0; i < textLength; angle += delta, i++)
	{
		RECT rect;
		DrawTextW(sourceDC, &str[i], 1, &rect, DT_CALCRECT);
		bmp = CreateCompatibleBitmap(hDc, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(sourceDC, bmp);
		TextOutW(sourceDC, 0, 0, &str[i], 1);
		Rotate(angle + PI / 2,
			POINT{ position.x + (long)((cos(angle) + 1) * radius.x),
			position.y + (long)((sin(angle) + 1) * radius.y) },
			hDc, sourceDC, bmp);
		DeleteObject(bmp);
		DeleteObject(hFont);
		DeleteObject(oldBmp);
	}
	DeleteObject(sourceDC);

}

void PrintRectangleStamp(HDC hDc, LPCWSTR str, POINT sides, POINT position, int fontHeight)
{
	size_t textLength = wcslen(str);
	INT frameWidth = 0;
	for (int i = 0; i < textLength; i++)
	{
		frameWidth = (INT)ceil((double)((sides.x + sides.y) - 8 * frameWidth) * 2 / textLength);
	}
	INT charHeight = frameWidth * 2;
	RECT rect;
	POINT pos = position;
	pos.x += charHeight;

	INT k = 0;
	FLOAT angle = 0; 
	INT lettersPerWidth = (INT)ceil((double)((INT)sides.x - 2 * charHeight) / frameWidth);
	INT lettersPerHeight = (INT)ceil((double)((INT)sides.y - 2 * charHeight) / frameWidth);
	INT printedLetters = lettersPerWidth;

	HDC sourceDC = CreateCompatibleDC(hDc);
	HBITMAP bmp = CreateCompatibleBitmap(hDc, 0, 0);
	HBITMAP oldBmp = (HBITMAP)SelectObject(sourceDC, bmp);

	HFONT font = CreateFont(fontHeight, 0, 0, 0, 0, 0, 0, 0,
		OEM_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_ROMAN, NULL);

	SelectObject(sourceDC, font);

	for (int i = 1; i <= 4; ++i)
	{
		while (k < printedLetters && str[k])
		{
			DrawTextW(sourceDC, &str[k], 1, &rect, DT_CALCRECT);
			bmp = CreateCompatibleBitmap(hDc, rect.right - rect.left, rect.bottom - rect.top);
			DeleteObject(SelectObject(sourceDC, bmp));
			TextOutW(sourceDC, 0, 0, &str[k], 1);
			Rotate(angle, pos, hDc, sourceDC, bmp);
			DeleteObject(bmp);

			switch (i)
			{
			case 1:
				pos.x += frameWidth;
				break;
			case 2:
				pos.y += frameWidth;
				break;
			case 3:
				pos.x -= frameWidth;
				break;
			case 4:
				pos.y -= frameWidth;
				break;
			}

			k++;
			DeleteObject(font);
			DeleteObject(bmp);
		}
		angle += PI / 2;
		switch (i)
		{
		case 1:
			pos.x = position.x + sides.x;
			pos.y += charHeight;
			printedLetters += lettersPerHeight;
			break;	
		case 2:
			pos.y = position.y + sides.y;
			pos.x -= charHeight;
			printedLetters += lettersPerWidth;
			break;
		case 3:
			pos.x = position.x;
			pos.y -= charHeight;
			printedLetters += lettersPerHeight;
			break;
		}
	}

	DeleteObject(font);
	DeleteObject(bmp);
	DeleteObject(oldBmp);
}

void Rotate(float radians, POINT position, HDC destDC, HDC sourceDC, HBITMAP hBitmap)
{
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);

	float cosine = (float)cos(radians);
	float sine = (float)sin(radians);

	SetGraphicsMode(destDC, GM_ADVANCED);

	XFORM xform;
	xform.eM11 = cosine;
	xform.eM12 = sine;
	xform.eM21 = -sine;
	xform.eM22 = cosine;
	xform.eDx = position.x;
	xform.eDy = position.y;

	SetWorldTransform(destDC, &xform);

	StretchBlt(destDC, 0, 0, bm.bmWidth, bm.bmHeight, sourceDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	DeleteObject(hBitmap);
}

void DisplayText(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	if (startWidth != 0 && startHeight != 0)
	{
		int stamp1X = 800 * windowWidth / startWidth;
		int stamp1Y = 50 * windowHeight / startHeight;
		int stamp1RX = 150 * windowWidth / startWidth;
		int stamp1RY = 150 * windowWidth / startWidth;
		size_t textLength1 = wcslen(rectText);
		INT fontHeight = (windowWidth + windowHeight) / 40;

		int stamp2X = 50 * windowWidth / startWidth;
		int stamp2Y = 50 * windowHeight / startHeight;
		int stamp2RX = 400 * windowWidth / startWidth;
		int stamp2RY = 400 * windowWidth / startWidth;
		size_t textLength3 = wcslen(roundText);

		HDC hDc = GetDC(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		FillRect(hDc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
		PrintEllipseStamp(hDc, rectText, POINT{ stamp1RX, stamp1RY }, POINT{ stamp1X, stamp1Y }, fontHeight);
		PrintRectangleStamp(hDc, roundText, POINT{ stamp2RX, stamp2RY }, POINT{ stamp2X, stamp2Y }, fontHeight);
	}
	EndPaint(hWnd, &ps);
}