#include "Graphics.h"

Graphics::~Graphics()
{
	if (factory) factory->Release();
	if (renderTarget) renderTarget->Release();
	if (pDrawArea) pDrawArea->Release();
	if (pPreviewArea) pPreviewArea->Release();
	if (brush) brush->Release();
	if (writeFactory) writeFactory->Release();
	if (writeFormat) writeFormat->Release();
	if (drawBrush) drawBrush->Release();
	if (previewBrush) previewBrush->Release();
}

bool Graphics::Init(HWND hWnd)
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (hr != S_OK) return false;

	RECT rect;
	GetClientRect(hWnd, &rect);
	hr = factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rect.right, rect.bottom), D2D1_PRESENT_OPTIONS_NONE), &renderTarget);
	if (hr != S_OK) return false;
	
	hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), &brush);
	if (hr != S_OK) return false;

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
	if (hr != S_OK) return false;

	hr = writeFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"en-us", &writeFormat);
	if (hr != S_OK) return false;

	hr = renderTarget->CreateCompatibleRenderTarget(D2D1::SizeF((float)rect.right * 0.75f, (float)rect.bottom), &pDrawArea);
	if (hr != S_OK) return false;

	hr = pDrawArea->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), &drawBrush);
	if (hr != S_OK) return false;

	hr = renderTarget->CreateCompatibleRenderTarget(D2D1::SizeF((float)rect.right * 0.25f, (float)rect.bottom), &pPreviewArea);
	if (hr != S_OK) return false;

	hr = pPreviewArea->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), &previewBrush);
	if (hr != S_OK) return false;

	return true;
}

void Graphics::ClearScreen(D2D1_COLOR_F color, RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::renderTarget:
		renderTarget->Clear(color);
		break;
	case RenderTarget::previewArea:
		pPreviewArea->Clear(color);
		break;
	case RenderTarget::drawArea:
		pDrawArea->Clear(color);
		break;
	}
}

void Graphics::DrawLine(D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color, float thickness, RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::renderTarget:
		brush->SetColor(color);
		renderTarget->DrawLine(p1, p2, brush, thickness);
		break;
	case RenderTarget::drawArea:
		drawBrush->SetColor(color);
		pDrawArea->DrawLine(p1, p2, drawBrush, thickness);
		break;
	case RenderTarget::previewArea:
		previewBrush->SetColor(color);
		pPreviewArea->DrawLine(p1, p2, previewBrush, thickness);
	}
}

void Graphics::FillRect(D2D1_RECT_F area, D2D1_COLOR_F color, RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::renderTarget:
		brush->SetColor(color);
		renderTarget->FillRectangle(area, brush);
		break;
	case RenderTarget::drawArea:
		drawBrush->SetColor(color);
		pDrawArea->FillRectangle(area, drawBrush);
		break;
	case RenderTarget::previewArea:
		previewBrush->SetColor(color);
		pPreviewArea->FillRectangle(area, previewBrush);
		break;
	}
}

void Graphics::DrawRect(D2D1_RECT_F area, D2D1_COLOR_F color, float thickness, RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::renderTarget:
		brush->SetColor(color);
		renderTarget->DrawRectangle(area, brush, thickness);
		break;
	case RenderTarget::drawArea:
		drawBrush->SetColor(color);
		pDrawArea->DrawRectangle(area, drawBrush, thickness);
		break;
	case RenderTarget::previewArea:
		previewBrush->SetColor(color);
		pPreviewArea->DrawRectangle(area, previewBrush, thickness);
		break;
	}
}

void Graphics::DrawText(const wchar_t* text, D2D1_RECT_F area, D2D1_COLOR_F color, RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::renderTarget:
		brush->SetColor(color);
		renderTarget->DrawTextA(text, lstrlenW(text), writeFormat, area, brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		break;
	case RenderTarget::drawArea:
		drawBrush->SetColor(color);
		pDrawArea->DrawTextA(text, lstrlenW(text), writeFormat, area, drawBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		break;
	case RenderTarget::previewArea:
		previewBrush->SetColor(color);
		pPreviewArea->DrawTextA(text, lstrlenW(text), writeFormat, area, previewBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		break;
	}
}

void Graphics::BeginDraw(RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::renderTarget:
		renderTarget->BeginDraw();
		break;
	case RenderTarget::drawArea:
		pDrawArea->BeginDraw();
		break;
	case RenderTarget::previewArea:
		pPreviewArea->BeginDraw();
		break;
	}
}

void Graphics::EndDraw(RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::renderTarget:
		renderTarget->EndDraw();
		break;
	case RenderTarget::drawArea:
		pDrawArea->EndDraw();
		break;
	case RenderTarget::previewArea:
		pPreviewArea->EndDraw();
		break;
	}
}