#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <d2d1_1.h>
#include <dwrite.h>

#pragma comment(lib, "dwrite.lib")
#pragma comment (lib, "windowscodecs.lib")
#pragma comment (lib, "d2d1.lib")

enum class RenderTarget
{
	renderTarget,
	drawArea,
	previewArea
};

class Graphics
{
private:
	ID2D1Factory* factory = NULL;
	ID2D1HwndRenderTarget* renderTarget = NULL;
	ID2D1SolidColorBrush* brush = NULL;
	ID2D1SolidColorBrush* previewBrush = NULL;
	ID2D1SolidColorBrush* drawBrush = NULL;
	ID2D1BitmapRenderTarget* pDrawArea = NULL;
	ID2D1BitmapRenderTarget* pPreviewArea = NULL;

	IDWriteFactory* writeFactory = NULL;
	IDWriteTextFormat* writeFormat = NULL;
public:
	Graphics() {}
	~Graphics();

	bool Init(HWND hWnd);
	
	ID2D1Factory* GetFactory() { return factory;	}
	ID2D1RenderTarget* GetRenderTarget() { return renderTarget; }
	ID2D1BitmapRenderTarget* GetDrawArea() { return pDrawArea; }
	ID2D1BitmapRenderTarget* GetPreviewArea() { return pPreviewArea; }

	void BeginDraw(RenderTarget target = RenderTarget::renderTarget);
	void EndDraw(RenderTarget target = RenderTarget::renderTarget);

	void ClearScreen(D2D1_COLOR_F color, RenderTarget target = RenderTarget::renderTarget);
	void DrawLine(D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color, float thickness = 3.0f, RenderTarget target = RenderTarget::renderTarget);
	void FillRect(D2D1_RECT_F area, D2D1_COLOR_F color, RenderTarget target = RenderTarget::renderTarget);
	void DrawRect(D2D1_RECT_F area, D2D1_COLOR_F color, float thickness = 3.0f, RenderTarget target = RenderTarget::renderTarget);
	void DrawText(const wchar_t* text, D2D1_RECT_F area, D2D1_COLOR_F color, RenderTarget target = RenderTarget::renderTarget);
};