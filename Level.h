#pragma once
#include "Graphics.h"
#include "SpriteSheet.h"
#include "defines.h"

enum class Direction
{
	up,
	down,
	left,
	right
};

class Level
{
public:
	static D2D1_POINT_2F GetMousePositionForCurrentDpi(LPARAM lParam, ID2D1Factory* factory)
	{
		static D2D1_POINT_2F dpi = { 96,96 };	//default dpi

		factory->GetDesktopDpi(&dpi.x, &dpi.y);

		return D2D1::Point2F(static_cast<int>(static_cast<short>(LOWORD(lParam))) * 96 / dpi.x,
			static_cast<int>(static_cast<short>(HIWORD(lParam))) * 96 / dpi.y);
	}
	static void ClearChecks()
	{
		for (int i = MENU_25; i < MENU_25 + 40; i++)
		{
			CheckMenuItem(GetMenu(Level::hWnd), i, MF_UNCHECKED);
		}
	}
public:
	virtual void LoadLevel() = 0;
	virtual void UnloadLevel() = 0;
	virtual bool OpenImage(HWND hWnd) = 0;
	virtual bool OpenImage(const wchar_t* filePath) = 0;
	virtual void Render() = 0;
	virtual void RenderDrawArea() = 0;
	virtual void RenderPreviewArea() = 0;
	virtual void Update(WPARAM wParam, LPARAM lParam) = 0;
	virtual void Move(Direction direction, double deltaTime, float _speed) = 0;
	virtual void ShiftSelectionArea(Direction direction, double deltaTime, float _speed = 0.0f) = 0;
	virtual void Zoom(bool in = true) = 0;
	virtual void ChangeSpeed(bool increase = true) = 0;
	virtual void Export(const char* FilePath) = 0;
	virtual void OpenSPR3(const char* filePath) = 0;
	virtual void ToggleGrid() = 0;
public:
	static Graphics* gfx;
	static HWND hWnd;
};