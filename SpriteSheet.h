#pragma once

#include <wincodec.h>
#include "Graphics.h"

class SpriteSheet
{
private:
	Graphics * gfx;
	ID2D1Bitmap* bmp;	
public:
	char* DataBuffer = NULL;
	size_t Bufferlen = 0;
	D2D1_SIZE_F size = {};
	wchar_t* FilePath = NULL;

	//constructor
	SpriteSheet(const wchar_t* filename, Graphics* gfx, D2D1_SIZE_F SpriteSize = {});
	SpriteSheet(char* buffer, size_t bufferlen, Graphics* gfx, D2D1_SIZE_F SpriteSize = {});

	//deconstructor
	~SpriteSheet();

	//draw
	D2D1_RECT_F Draw(D2D1_RECT_F _src, D2D1_RECT_F _dest, bool keepRatio = false, RenderTarget target = RenderTarget::renderTarget);
};