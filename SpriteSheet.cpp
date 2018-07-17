#include "SpriteSheet.h"
#include <vector>

SpriteSheet::SpriteSheet(const wchar_t* filename, Graphics* gfx, D2D1_SIZE_F SpriteSize)
{
	FilePath = new wchar_t[lstrlenW(filename) + 1];
	memcpy(FilePath, filename, (lstrlenW(filename) + 1) * sizeof(wchar_t));

	this->gfx = gfx;		//save gfx
	bmp = NULL;

	//create factory
	IWICImagingFactory *wicFactory = NULL;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create WIC factory!", "Error", MB_OK | MB_ICONERROR);
	}

	//create decoder
	IWICBitmapDecoder *wicDecoder = NULL;
	hr = wicFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);

	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create wic decoder!", "Error", MB_OK | MB_ICONERROR);
	}

	//read frame from teh image
	IWICBitmapFrameDecode* wicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &wicFrame);			//0 = number frame, in this case there is only 1 frame so zero
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to get WIC deocder frame!", "Error", MB_OK | MB_ICONERROR);
	}

	//create a converter
	IWICFormatConverter *wicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&wicConverter);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create WIC Format Converter!", "Error", MB_OK | MB_ICONERROR);
	}

	hr = wicConverter->Initialize(
		wicFrame,						//frame
		GUID_WICPixelFormat32bppPBGRA,	//output Pixel format (P means packed; pixels are represented by 4 byts packed into an int: 0xRRGGBBAA
		WICBitmapDitherTypeNone,		//Irrelevant
		NULL,							//no palette needed
		0.0,							//alpha transparency % irrelevent
		WICBitmapPaletteTypeCustom		//irrelevant
	);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to initialize WIC Converter!", "Error", MB_OK | MB_ICONERROR);
	}

	//use converter to create D2D1Bitmap
	hr = gfx->GetRenderTarget()->CreateBitmapFromWicBitmap(
		wicConverter,					//converter
		NULL,							//D2D1_BITMAP_PROPERTIES
		&bmp							//destination D2D1 bitmap
	);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create D2D1Bitmap!", "Error", MB_OK | MB_ICONERROR);
	}

	if (wicFactory) wicFactory->Release();
	if (wicDecoder) wicDecoder->Release();
	if (wicConverter) wicConverter->Release();
	if (wicFrame) wicFrame->Release();

	if (bmp)
	{
		size.width = bmp->GetSize().width;
		size.height = bmp->GetSize().height;
	}

	if (SpriteSize.height && SpriteSize.width)
	{
		this->size.width = SpriteSize.width;
		this->size.height = SpriteSize.height;
	}
}

SpriteSheet::SpriteSheet(char* buffer, size_t bufferlen, Graphics* gfx, D2D1_SIZE_F SpriteSize)
{
	//FilePath = new wchar_t[lstrlenW(filename) + 1];
	//memcpy(FilePath, filename, (lstrlenW(filename) + 1) * sizeof(wchar_t));

	IWICStream *pIWICStream = NULL;
	DataBuffer = new char[bufferlen];
	Bufferlen = bufferlen;
	memcpy(DataBuffer, buffer, bufferlen);

	this->gfx = gfx;		//save gfx
	bmp = NULL;

	//create factory
	IWICImagingFactory *wicFactory = NULL;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create WIC factory!", "Error", MB_OK | MB_ICONERROR);
	}

	hr = wicFactory->CreateStream(&pIWICStream);
	if (hr != S_OK)
		MessageBox(NULL, "Failed to create stream", "Error", MB_OK | MB_ICONERROR);
	hr = pIWICStream->InitializeFromMemory(reinterpret_cast<BYTE*>(buffer), bufferlen);
	if (hr != S_OK)
		MessageBox(NULL, "InitFromMemory Failed", "Error", MB_OK | MB_ICONERROR);

	//create decoder
	IWICBitmapDecoder *wicDecoder = NULL;
	//hr = wicFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
	hr = wicFactory->CreateDecoderFromStream(pIWICStream, NULL, WICDecodeMetadataCacheOnLoad, &wicDecoder);

	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create wic decoder!", "Error", MB_OK | MB_ICONERROR);
	}

	//read frame from teh image
	IWICBitmapFrameDecode* wicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &wicFrame);			//0 = number frame, in this case there is only 1 frame so zero
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to get WIC deocder frame!", "Error", MB_OK | MB_ICONERROR);
	}

	//create a converter
	IWICFormatConverter *wicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&wicConverter);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create WIC Format Converter!", "Error", MB_OK | MB_ICONERROR);
	}

	hr = wicConverter->Initialize(
		wicFrame,						//frame
		GUID_WICPixelFormat32bppPBGRA,	//output Pixel format (P means packed; pixels are represented by 4 byts packed into an int: 0xRRGGBBAA
		WICBitmapDitherTypeNone,		//Irrelevant
		NULL,							//no palette needed
		0.0,							//alpha transparency % irrelevent
		WICBitmapPaletteTypeCustom		//irrelevant
	);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to initialize WIC Converter!", "Error", MB_OK | MB_ICONERROR);
	}

	//use converter to create D2D1Bitmap
	hr = gfx->GetRenderTarget()->CreateBitmapFromWicBitmap(
		wicConverter,					//converter
		NULL,							//D2D1_BITMAP_PROPERTIES
		&bmp							//destination D2D1 bitmap
	);
	if (hr != S_OK)
	{
		//error control
		MessageBox(NULL, "Unable to create D2D1Bitmap!", "Error", MB_OK | MB_ICONERROR);
	}

	if (wicFactory) wicFactory->Release();
	if (wicDecoder) wicDecoder->Release();
	if (wicConverter) wicConverter->Release();
	if (wicFrame) wicFrame->Release();
	if (pIWICStream) pIWICStream->Release();

	if (bmp)
	{
		size.width = bmp->GetSize().width;
		size.height = bmp->GetSize().height;
	}

	if (SpriteSize.height && SpriteSize.width)
	{
		this->size.width = SpriteSize.width;
		this->size.height = SpriteSize.height;
	}
}

SpriteSheet::~SpriteSheet()
{
	if (FilePath) delete[] FilePath;
	if (DataBuffer) delete[] DataBuffer;
	if (bmp) bmp->Release();
}

D2D1_RECT_F SpriteSheet::Draw(D2D1_RECT_F src, D2D1_RECT_F dest, bool keepRatio, RenderTarget target)
{
	float xMod = 0.0f;
	float yMod = 0.0f;
	if (src.left > src.right) std::swap(src.left, src.right);
	if (src.top > src.bottom) std::swap(src.top, src.bottom);
	D2D1_RECT_F size = { 0.0f, 0.0f, dest.right - dest.left, dest.bottom - dest.top };

	if (keepRatio)
	{
		if ((src.bottom - src.top) > (src.right - src.left))
		{
			float ratio = (src.right - src.left) / (src.bottom - src.top);
			if (ratio < 0.0f) ratio *= -1;
			xMod = (dest.right - dest.left) - (dest.right - dest.left) * ratio;
			xMod /= 2;

			if (xMod < 0.0f) xMod *= -1;

			size.right = (size.right - size.left) * ratio;
			size.left = 0.0f;
		}
		else
		{
			float ratio = (src.bottom - src.top) / (src.right - src.left);
			if (ratio < 0.0f) ratio *= -1;
			yMod = (size.bottom - size.top) - (size.bottom - size.top) * ratio;
			yMod /= 2;

			if (yMod < 0.0f) yMod *= -1;

			size.bottom = (size.bottom - size.top) * ratio;
			size.top = 0.0f;
		}
	}

	dest = { dest.left + xMod, dest.top + yMod, dest.left + size.right + xMod, dest.top + size.bottom + yMod };

	switch (target)
	{
	case RenderTarget::renderTarget:
		gfx->GetRenderTarget()->DrawBitmap(
			bmp,
			dest,
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			src
		);
		break;
	case RenderTarget::drawArea:
		gfx->GetDrawArea()->DrawBitmap(
			bmp,
			dest,
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			src
		);
		break;
	case RenderTarget::previewArea:
		gfx->GetPreviewArea()->DrawBitmap(
			bmp,
			dest,
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			src
		);
	}

	return dest;
}