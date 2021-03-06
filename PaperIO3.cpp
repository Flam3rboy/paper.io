// GT_HelloWorldWin32.cpp  
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c  

#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>
#include "stdafx.h"
#include <math.h>
#include <wingdi.h>
#include <iostream>
#include <windowsx.h>
#include "game.h"
#include "socket.h"

#pragma comment (lib, "Ws2_32.lib")

#define IDM_GAME_HOST 1
#define IDM_GAME_CLIENT 2
#define IDM_GAME_QUIT 3

// Forward declarations of functions included in this code module:  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

VOID CALLBACK Game(HWND lpParam, BOOLEAN TimerOrWaitFired) {
	if (role == 1) {
		for (int i = 0; i < Clients; i++) {
			BoardEditing(i);
			SendBoard(i);
		}
	}
	InvalidateRect(lpParam, NULL, TRUE);
	//UpdateWindow(lpParam); //Only for force-update immediately
}

HANDLE hTimerQueue = NULL;
HANDLE hTimer = NULL;

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR     lpCmdLine, _In_ int       nCmdShow)
{
	static TCHAR szWindowClass[] = _T("win32app");
	static TCHAR szTitle[] = _T("Win32 Guided Tour Application");
	HINSTANCE hInst;
	HWND hWnd;

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	hInst = hInstance; // Store instance handle in our global variable  

					   // The parameters to CreateWindow explained:  
					   // szWindowClass: the name of the application  
					   // szTitle: the text that appears in the title bar  
					   // WS_OVERLAPPEDWINDOW: the type of window to create  
					   // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
					   // 500, 100: initial size (width, length)  
					   // NULL: the parent of this window  
					   // NULL: this application does not have a menu bar  
					   // hInstance: the first parameter from WinMain  
					   // NULL: not used in this application  
	hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		0, 0,
		800, 800,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hTimerQueue = CreateTimerQueue();
	CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)Game, hWnd, 0, 150, 0);


	HMENU hMenubar;
	HMENU hMenu;

	hMenubar = CreateMenu();
	hMenu = CreateMenu();

	AppendMenuW(hMenu, MF_STRING, IDM_GAME_HOST, L"&Host");
	AppendMenuW(hMenu, MF_STRING, IDM_GAME_CLIENT, L"&Client");
	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu, MF_STRING, IDM_GAME_QUIT, L"&Quit");

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&Game");
	SetMenu(hWnd, hMenubar);

	Reset();

	// Main message loop:  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

HBRUSH brushblue = CreateSolidBrush(RGB(63, 137, 255));
HBRUSH brushlightblue = CreateSolidBrush(RGB(96, 157, 255));
HBRUSH brushdarkblue = CreateSolidBrush(RGB(32, 86, 173));
HBRUSH brushdarkgreen = CreateSolidBrush(RGB(0, 94, 11));
HBRUSH brushgreen = CreateSolidBrush(RGB(0, 160, 18));
HBRUSH brushlightgreen = CreateSolidBrush(RGB(64, 206, 80));
HBRUSH brushdarkpurple = CreateSolidBrush(RGB(76, 0, 102));
HBRUSH brushpurple = CreateSolidBrush(RGB(146, 1, 196));
HBRUSH brushlightpurple = CreateSolidBrush(RGB(222, 88, 252));
HBRUSH brushdarkred = CreateSolidBrush(RGB(137, 0, 18));
HBRUSH brushred = CreateSolidBrush(RGB(216, 0, 28));
HBRUSH brushlightred = CreateSolidBrush(RGB(232, 62, 84));
HBRUSH brushdarkorange = CreateSolidBrush(RGB(193, 106, 0));
HBRUSH brushorange = CreateSolidBrush(RGB(255, 140, 0));
HBRUSH brushlightorange = CreateSolidBrush(RGB(255, 164, 53));
HBRUSH brushgrey = CreateSolidBrush(RGB(153, 153, 153));
int Theme = 1;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR TextScore[] = _T("Hello, World!");
	RECT rect;

	POINT ptPrevious = { 0,0 };
	POINT ptNext = { 0,0 };

	int WindowWidth;
	int WindowHeight;
	if (GetWindowRect(hWnd, &rect))
	{
		WindowWidth = rect.right - rect.left;
		WindowHeight = rect.bottom - rect.top;
	}
	int BlockSize = 25;
	int ScreenMaxRectX = (int)(WindowWidth / BlockSize) - (60 / BlockSize);
	int ScreenMaxRectY = (int)(WindowHeight / BlockSize) - (80 / BlockSize);
	int ScreenX = 0;
	int ScreenY = 0;
	int StartViewX;
	int StartViewY;
	int EndViewX;
	int EndViewY;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		if (Theme == 1) {
			SetTextColor(hdc, RGB(255, 255, 255));
			SetBkColor(hdc, RGB(0, 0, 0));
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(0, 0, 0)));
		}
		else if (Theme == 0) {
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkColor(hdc, RGB(255, 255, 255));
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(255, 255, 255)));
		}

		if (GetGameOver(IDPlayer) == true) {
			TextOut(hdc, WindowWidth / 2, WindowHeight / 2, _T("GameOver"), _tcslen(_T("GameOver")));
		}
		else {

			StartViewX = GetPlayerPositionX(IDPlayer) - ScreenMaxRectX / 2;
			StartViewY = GetPlayerPositionY(IDPlayer) - ScreenMaxRectY / 2;
			EndViewX = GetPlayerPositionX(IDPlayer) + ScreenMaxRectX / 2 + 0;
			EndViewY = GetPlayerPositionY(IDPlayer) + ScreenMaxRectY / 2 + 0;

			if (EndViewX > 99) {
				StartViewX = StartViewX - EndViewX + 100;
				EndViewX = 99;
			} if (EndViewY > 99) {
				StartViewY = StartViewY - EndViewY + 100;
				EndViewY = 99;
			} if (StartViewX < 0) {
				EndViewX = EndViewX + abs(StartViewX);
				StartViewX = 0;
			} if (StartViewY < 0) {
				EndViewY = EndViewY + abs(StartViewY);
				StartViewY = 0;
			}

			ScreenX = 0;
			ScreenY = 0;

			for (int y = StartViewY; y <= EndViewY; y++) {
				for (int x = StartViewX; x <= EndViewX; x++) {

					rect = { ScreenX * BlockSize, ScreenY * BlockSize, ScreenX * BlockSize + BlockSize,ScreenY * BlockSize + BlockSize };

					switch (GetBoard(y, x) % 10) {
					case 1:
						FillRect(hdc, &rect, brushlightgreen);
						break;
					case 2:
						FillRect(hdc, &rect, brushgreen);
						break;
					case 3:
						FillRect(hdc, &rect, brushdarkgreen);
						break;
					}

					switch (GetBoard(y, x)) {
					case 1:
						FillRect(hdc, &rect, brushlightgreen);
						break;
					case 2:
						FillRect(hdc, &rect, brushgreen);
						break;
					case 3:
						FillRect(hdc, &rect, brushdarkgreen);
						break;
					case 4:
						FillRect(hdc, &rect, brushgrey);
					}

					switch (GetBoard(y, x)) {
					case 11:
						FillRect(hdc, &rect, brushlightblue);
						break;
					case 12:
						FillRect(hdc, &rect, brushblue);
						break;
					case 13:
						FillRect(hdc, &rect, brushdarkblue);
						break;
					}

					switch (GetBoard(y, x)) {
					case 21:
						FillRect(hdc, &rect, brushlightpurple);
						break;
					case 22:
						FillRect(hdc, &rect, brushpurple);
						break;
					case 23:
						FillRect(hdc, &rect, brushdarkpurple);
						break;
					}

					switch (GetBoard(y, x)) {
					case 31:
						FillRect(hdc, &rect, brushlightred);
						break;
					case 32:
						FillRect(hdc, &rect, brushred);
						break;
					case 33:
						FillRect(hdc, &rect, brushdarkred);
						break;
					}

					switch (GetBoard(y, x)) {
					case 41:
						FillRect(hdc, &rect, brushlightorange);
						break;
					case 42:
						FillRect(hdc, &rect, brushorange);
						break;
					case 43:
						FillRect(hdc, &rect, brushdarkorange);
						break;
					}

					ScreenX++;
				}
				ScreenY++;
				ScreenX = 0;
			}
		}

		_itow_s(GetScore(IDPlayer), (TCHAR *)TextScore, sizeof(TextScore) / sizeof(TCHAR), 10);
		TextOut(hdc, 0, 0, TextScore, _tcslen(TextScore));

		if (GameStatus[0] != '\0') {
			wchar_t GameStatusWCHAR[DEFAULT_BUFLEN * 2];
			mbstowcs(GameStatusWCHAR, &GameStatus[0], strlen(GameStatus) + 1);

			//TextOut(hdc, WindowWidth / 4, WindowHeight / 4, GameStatusWCHAR, _tcslen(GameStatusWCHAR));
			SetWindowText(hWnd, GameStatusWCHAR);
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_GAME_HOST:
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)SocketServer, 0, 0, 0, WT_EXECUTEONLYONCE);
			break;

		case IDM_GAME_CLIENT:
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)SocketClient, 0, 0, 0, WT_EXECUTEONLYONCE);
			break;

		case IDM_GAME_QUIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		}
		break;

	case WM_KEYDOWN:
		if (GetGameOver(IDPlayer) == true) {
			Initialize(IDPlayer);
			break;
		}
		switch (wParam)
		{
		case VK_LEFT:
			SetInput('a', IDPlayer);
			break;
		case VK_RIGHT:
			SetInput('d', IDPlayer);
			break;
		case VK_UP:
			SetInput('w', IDPlayer);
			break;
		case VK_DOWN:
			SetInput('s', IDPlayer);
			break;
		case VK_ESCAPE:
			SetInput('x', IDPlayer);
			break;
		case 'O':
			Theme = !Theme;
		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		ptPrevious.x = LOWORD(lParam);
		ptPrevious.y = HIWORD(lParam);
		break;

	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON) {
			ptNext.x = LOWORD(lParam);
			ptNext.y = HIWORD(lParam);

			if (fabs(ptPrevious.x - ptNext.x) > fabs(ptPrevious.y - ptNext.y)) {

				if (ptPrevious.x > ptNext.x) {
					WndProc(hWnd, WM_KEYDOWN, VK_LEFT, 0);
				}
				else if (ptPrevious.x < ptNext.x) {
					WndProc(hWnd, WM_KEYDOWN, VK_RIGHT, 0);
				}
			}
			else {
				if (ptPrevious.y > ptNext.y) {
					WndProc(hWnd, WM_KEYDOWN, VK_UP, 0);
				}
				else if (ptPrevious.y < ptNext.y) {
					WndProc(hWnd, WM_KEYDOWN, VK_DOWN, 0);
				}
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;

}