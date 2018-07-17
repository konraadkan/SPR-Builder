#include "Graphics.h"
#include "SpriteSheet.h"
#include "Level1.h"
#include "defines.h"
#include <string>
#include <vector>

#define MYCLASSNAME "MainWindow"
#define MYWINDOWNAME "SPR Builder 4"

static Level* CurrentLevel;

HMENU createMenu()
{
	HMENU menu = CreateMenu();
	HMENU smenu = CreatePopupMenu();
	AppendMenu(smenu, MF_STRING, MENU_NEW, "New");
	AppendMenu(smenu, MF_STRING, MENU_OPEN, "Open Image");
	AppendMenu(smenu, MF_STRING, MENU_EDITSPR3, "Edit SPR3");
	AppendMenu(smenu, MF_STRING, MENU_EXPORT, "Export");
	AppendMenu(smenu, MF_STRING, MENU_EXIT, "Exit");
	AppendMenu(menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(smenu), "File");

	smenu = CreatePopupMenu();
	for (int i = MENU_25; i < MENU_25 + 40; i++)
	{
		std::string text = std::to_string(((i - MENU_25) + 1) * 25);
		text.append(" Ticks");
		AppendMenu(smenu, MF_STRING, i, text.c_str());
	}
	AppendMenu(menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(smenu), "Speed");

	smenu = CreatePopupMenu();
	AppendMenu(smenu, MF_STRING, MENU_TOGGLE_GRID, "Toggle Grid");
	//AppendMenu(smenu, MF_STRING, MENU_CHANGE_BGC, "Change Background Color");
	AppendMenu(menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(smenu), "Advanced");

	return menu;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }

	if (wParam)
	{
		switch (wParam)
		{
		case MENU_NEW:
		{
			Graphics* gfx = CurrentLevel->gfx;
			if (gfx)
			{
				delete CurrentLevel;
				CurrentLevel = new Level1(gfx);
				Level::ClearChecks();
				CheckMenuItem(GetMenu(hWnd), MENU_25 + 3, MF_CHECKED);
			}
			break;
		}
		case MENU_EXIT:
			CurrentLevel->UnloadLevel();
			PostQuitMessage(0);
			return 0;
		case MENU_OPEN:
			//load image
			CurrentLevel->OpenImage(hWnd);
			break;
		case MENU_TOGGLE_GRID:
			CurrentLevel->ToggleGrid();
			break;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPWSTR cmd, int iCmdShow)
{
	WNDCLASSEX wndClass = {};
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = wndProc;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszClassName = MYCLASSNAME;

	HMENU menu = createMenu();

	RegisterClassEx(&wndClass);
	RECT rect = { 0, 0, 1920, 1080 };
	RECT desktop = { 0, 0, 0, 0 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, true, WS_EX_OVERLAPPEDWINDOW);

	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, MYCLASSNAME, MYWINDOWNAME, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, (desktop.right / 2) - (rect.right / 2), (desktop.bottom / 2) - (rect.bottom / 2), rect.right - rect.left, rect.bottom - rect.top, NULL, menu, hInstance, 0);

	if (!hWnd)
	{
		return -1;
	}

	Level::hWnd = hWnd;
	Level::ClearChecks();
	CheckMenuItem(GetMenu(hWnd), MENU_25 + 3, MF_CHECKED);

	Graphics* gfx = new Graphics();
	if (!gfx->Init(hWnd))
	{
		delete gfx;
		return -1;
	}
	Graphics* graphics = gfx;
	
	CurrentLevel = new Level1(gfx);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	msg.message = WM_NULL;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			CurrentLevel->Update(msg.wParam, msg.lParam);
			CurrentLevel->Render();
		}
	}

	delete CurrentLevel;
	delete gfx;
	return 0;
}