#include "Level1.h"
#include<string>

Level1::Level1(Graphics* _gfx)
{
	gfx = _gfx;
	LoadLevel();
}

Level1::~Level1()
{
	UnloadLevel();
}

void Level1::LoadLevel()
{
	scale = 1.0f;
	translation = {};
	selectionArea = {};
	sprite = NULL;	
	timer = new HPTimer();
	previewTimer = new HPTimer();
	speed.x = 50.0f;
	speed.y = speed.x * (1080.0f / 1920.0f);
	precisionSpeed = 10.0f;
	keys = new inputs;
	previewSize = 128.0f;
	currentFrame = 0;
	FrameSpeed = 0.10f;
	LastTimeValue = CurrentTimeValue = timer->GetTimeTotal();
	wTempFilePath = NULL;
	BuildGrid();
}

void Level1::UnloadLevel()
{
	scale = 1.0f;
	translation = {};
	selectionArea = {};
	if (sprite) delete sprite;
	sprite = NULL;
	delete timer;
	delete previewTimer;
	delete keys;
	if (wTempFilePath)					//legacy, should never be called
	{
		_wremove(wTempFilePath);
		delete[] wTempFilePath;
		wTempFilePath = NULL;
	}
	if (bGrid) bGrid->Release();
}

bool Level1::OpenImage(HWND hWnd)
{
	wchar_t imagePath[512] = {};
	OPENFILENAMEW ofn = {};
	imagePath[0] = L'\0';
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = L"Image Files (*.bmp, *.png, *.jpg)\0*.bmp;*.png;*.jpg\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = imagePath;
	ofn.nMaxFile = 512;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = L"bmp";
	if (GetOpenFileNameW(&ofn))
	{
		if (imagePath[wcslen(imagePath) - 1] == '3')
		{
			//spr3 decode
		}
		else
		{
			if (sprite)	delete sprite;
			FILE* file;
			_wfopen_s(&file, imagePath, L"rb");
			fseek(file, 0, SEEK_END);
			size_t len = ftell(file);
			fseek(file, 0, SEEK_SET);
			char* buffer = new char[len];
			fread(buffer, len, 1, file);
			fclose(file);
			//sprite = new SpriteSheet(imagePath, gfx);
			sprite = new SpriteSheet(buffer, len, gfx);
			delete[] buffer;
			doit = true;
			if (sprite) return true;
		}
	}
	return false;
}

bool Level1::OpenImage(const wchar_t* filePath)
{
	if (sprite) delete sprite;
	sprite = new SpriteSheet(filePath, gfx);
	if (sprite) return true;
	return false;
}

void Level1::RenderDrawArea()
{
	D2D1_MATRIX_3X2_F scaling, translating;

	D2D1_POINT_2F pp = transformMatrix.TransformPoint({ 1920.0f * 0.75f / 2.0f, 1080.0f / 2.0f });

	scaling = D2D1::Matrix3x2F::Scale(D2D1::Size(scale, scale), pp);
	translating = D2D1::Matrix3x2F::Translation(translation);
	transform = scaling * translating;

	gfx->BeginDraw(RenderTarget::drawArea);
	gfx->GetDrawArea()->SetTransform(transform);
	gfx->GetDrawArea()->GetTransform(&transformMatrix);

	transformMatrix.Invert();

	gfx->ClearScreen({ 0.0f, 0.0f, 0.33f, 1.0f }, RenderTarget::drawArea);
	if (sprite)
	{
		gfx->DrawRect({ 0.0f, 0.0f, sprite->size.width, sprite->size.height }, { 0.0f, 0.0f, 0.0f, 0.8f }, 1.0f, RenderTarget::drawArea);
		sprite->Draw({ 0.0f, 0.0f, sprite->size.width, sprite->size.height }, { 0.0f, 0.0f, sprite->size.width, sprite->size.height }, false, RenderTarget::drawArea);
		gfx->DrawRect(selectionArea, { 0.66f, 0.1f, 0.33f, 1.0f }, 1.0f, RenderTarget::drawArea);
	}
	if (bShowGrid && bGrid)
		gfx->GetDrawArea()->DrawBitmap(bGrid, { 0.0f, 0.0f, 1920.0f * 0.75f, 1080.0f }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, { 0.0f, 0.0f, 1920.0f * 0.75f, 1080.0f });
	gfx->EndDraw(RenderTarget::drawArea);
}

void Level1::RenderPreviewArea()
{
	gfx->BeginDraw(RenderTarget::previewArea);
	gfx->ClearScreen({ 1.0f, 0.0f, 0.33f, 1.0f }, RenderTarget::previewArea);

	if (sprite)
	{
		D2D1_RECT_F tArea = { 0.0f, 0.0f, 1920.0f * 0.25f, 1920.0f * 0.25f };
		tArea = sprite->Draw(selectionArea, tArea, true, RenderTarget::previewArea);
		gfx->DrawRect(tArea, { 0.0f, 0.0f, 0.0f, 0.80f }, 2.0f, RenderTarget::previewArea);
		gfx->DrawLine({ (tArea.right + tArea.left) / 2.0f, tArea.top }, { (tArea.right + tArea.left) / 2.0f, tArea.bottom }, { 0.0f, 0.0f, 0.0f, 0.80f }, 2.0f, RenderTarget::previewArea);
		gfx->DrawLine({ tArea.left, (tArea.bottom + tArea.top) / 2.0f }, { tArea.right, (tArea.bottom + tArea.top) / 2.0f }, { 0.0f, 0.0f, 0.0f, 0.80f }, 2.0f, RenderTarget::previewArea);

		tArea = { 0.0f, 0.0f, 1920.0f * 0.25f, 1920.0f * 0.25f };
		DrawPreviewMovement({ 5.0f, tArea.bottom, previewSize + 5.0f, tArea.bottom + previewSize });
		DrawLastFramePreview({ previewSize + 8.0f, tArea.bottom, previewSize + 8.0f + previewSize, tArea.bottom + previewSize });

		wchar_t sText[512];
		swprintf_s(sText, 512, L"%.2f x %.2f", selectionArea.right - selectionArea.left, selectionArea.bottom - selectionArea.top);
		gfx->DrawText(sText, { previewSize + 8.0f + previewSize + 2.0f, tArea.bottom + previewSize - 16.0f, 1920.0f * 0.25f, tArea.bottom + previewSize }, { 0.0f, 0.0f, 0.0f, 1.0f }, RenderTarget::previewArea);

		D2D1_RECT_F textRect = { 1.0f, tArea.bottom + previewSize, 1920.0f * 0.25f, 0.0f };
		textRect.bottom = textRect.top + 16.0f;

		for (size_t i = 0; i < frames.size(); i++)
		{
			wchar_t coord[512];
			swprintf_s(coord, 512, L"%.2f, %.2f, %.2f, %.2f", frames[i].left, frames[i].top, frames[i].right, frames[i].bottom);
			gfx->DrawText(coord, { textRect.left, textRect.top + i * 16.0f, textRect.right, textRect.bottom + i * 33.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, RenderTarget::previewArea);
		}
	}
	gfx->EndDraw(RenderTarget::previewArea);
}

void Level1::Render()
{
	//graphic stuff

	RenderDrawArea();
	RenderPreviewArea();

	ID2D1Bitmap* o = NULL;
	ID2D1Bitmap* oo = NULL;
	gfx->GetDrawArea()->GetBitmap(&o);
	gfx->GetPreviewArea()->GetBitmap(&oo);
	/*************************experimental area***************************************/
	D2D1_POINT_2U rect2 = D2D1::Point2U(0, 0);
	D2D1_SIZE_U size = D2D1::SizeU(o->GetSize().width, o->GetSize().height);
	D2D1_BITMAP_PROPERTIES1 bp = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_CPU_READ, o->GetPixelFormat());
	UINT32 pitch = o->GetSize().width > o->GetSize().height ? o->GetSize().width * 4 : o->GetSize().height * 4;

	if (doit)
	{
		size_t s = o->GetSize().width > o->GetSize().height ? o->GetSize().width : o->GetSize().height;
		s *= 4;
		char* w = new char[s];
		memcpy(w, o, s);
		/*
		FILE* file = NULL;
		if (fopen_s(&file, "thisonedumbass.txt", "wb")) MessageBox(NULL, "Failed to open file","",MB_OK);		
		fwrite(w, s, 1, file);
		fclose(file);*/
		//MessageBox(NULL, std::to_string(o->GetSize().width).c_str(), std::to_string(o->GetSize().height).c_str(), MB_OK);
	}
	/***********************end experimental area************************************/
	gfx->BeginDraw();
	gfx->ClearScreen({ 0.0f, 1.0f, 0.0f, 1.0f });
	gfx->GetRenderTarget()->DrawBitmap(o, { 0.0f, 0.0f, 1920.0f * 0.75f, 1080.0f }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { 0.0f, 0.0f, 1920.0f * 0.75f, 1080.0f });
	gfx->GetRenderTarget()->DrawBitmap(oo, { 1920.0f * 0.75f, 0.0f, 1920.0f, 1080.0f }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, { 0.0f, 0.0f, 1920.0f * 0.25f, 1080.0f });
	gfx->EndDraw();

	o->Release();
	oo->Release();
}

void Level1::DrawPreviewMovement(D2D1_RECT_F area)
{
	if (frames.empty())
		return;
	if (currentFrame + 1 > frames.size())
		currentFrame = 0;

	sprite->Draw(frames[currentFrame], area, true, RenderTarget::previewArea);
	gfx->DrawRect(area, { 0.0f, 0.0f, 0.0f, 1.0f }, 1.0f, RenderTarget::previewArea);
	
	if (CurrentTimeValue - LastTimeValue > FrameSpeed)
	{
		LastTimeValue = CurrentTimeValue;
		currentFrame++;
	}
}
void Level1::DrawLastFramePreview(D2D1_RECT_F area)
{
	if (frames.empty())
		return;

	sprite->Draw(frames.back(), area, true, RenderTarget::previewArea);

	gfx->DrawRect(area, { 0.0f, 0.0f, 0.0f, 1.0f }, 1.0f, RenderTarget::previewArea);
	gfx->DrawLine({ (area.left + area.right) / 2.0f, area.top }, { (area.left + area.right) / 2.0f, area.bottom }, { 0.0f, 0.0f, 0.0f, 0.8f }, 1.0f, RenderTarget::previewArea);
	gfx->DrawLine({ area.left, (area.top + area.bottom) / 2.0f }, { area.right, (area.top + area.bottom) / 2.0f }, { 0.0f, 0.0f, 0.0f, 0.8f }, 1.0f, RenderTarget::previewArea);
}

void Level1::Resize(Direction direction, double deltaTime, float _speed)
{
	D2D1_POINT_2F cspeed = speed;
	if (_speed)
	{
		cspeed.x = _speed;
		cspeed.y = cspeed.x * (1080.0f / 1920.0f);
	}

	if (keys->getKeyStateImmediate(VK_RBUTTON))
	{
		switch (direction)
		{
		case Direction::up:
			selectionArea.bottom -= (float)(cspeed.y * deltaTime);
			break;
		case Direction::down:
			selectionArea.bottom += (float)(cspeed.y * deltaTime);
			break;
		case Direction::left:
			selectionArea.right -= (float)(cspeed.x * deltaTime);
			break;
		case Direction::right:
			selectionArea.right += (float)(cspeed.x * deltaTime);
			break;
		}
	}
	else
	{
		switch (direction)
		{
		case Direction::up:
			selectionArea.top -= (float)(cspeed.y * deltaTime);
			break;
		case Direction::down:
			selectionArea.top += (float)(cspeed.y * deltaTime);
			break;
		case Direction::left:
			selectionArea.left -= (float)(cspeed.x * deltaTime);
			break;
		case Direction::right:
			selectionArea.left += (float)(cspeed.x * deltaTime);
			break;
		}
	}
}

void Level1::Update(WPARAM wParam, LPARAM lParam)
{
	timer->Update();
	CurrentTimeValue = timer->GetTimeTotal();
	D2D1_POINT_2F p = GetMousePositionForCurrentDpi(lParam, gfx->GetFactory());
	D2D1_POINT_2F pp = transformMatrix.TransformPoint(p);
	float cSpeed = 0.0f;

	if (keys->getKeyStateImmediate(VK_CONTROL))
		cSpeed = precisionSpeed;
	else
		cSpeed = 0.0f;

	if (keys->getKeyStateImmediate(VK_MBUTTON))
	{
		if (keys->getKeyStateImmediate(VK_LEFT))
			Resize(Direction::left, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_RIGHT))
			Resize(Direction::right, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_UP))
			Resize(Direction::up, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_DOWN))
			Resize(Direction::down, timer->GetTimeDelta(), cSpeed);
	}
	else if (keys->getKeyStateImmediate(VK_SHIFT))
	{
		if (keys->getKeyStateImmediate(VK_LEFT))
			ShiftSelectionArea(Direction::left, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_RIGHT))
			ShiftSelectionArea(Direction::right, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_UP))
			ShiftSelectionArea(Direction::up, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_DOWN))
			ShiftSelectionArea(Direction::down, timer->GetTimeDelta(), cSpeed);
	}
	else
	{
		if (keys->getKeyStateImmediate(VK_LEFT))
			Move(Direction::left, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_RIGHT))
			Move(Direction::right, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_UP))
			Move(Direction::up, timer->GetTimeDelta(), cSpeed);
		if (keys->getKeyStateImmediate(VK_DOWN))
			Move(Direction::down, timer->GetTimeDelta(), cSpeed);
	}
	if (keys->getKeyStateImmediate(VK_MULTIPLY))
		Zoom();
	if (keys->getKeyStateImmediate(VK_DIVIDE))
		Zoom(false);

	if (keys->getKeyStateDelay(VK_ADD))
		ChangeSpeed();
	if (keys->getKeyStateDelay(VK_SUBTRACT))
		ChangeSpeed(false);

	if (keys->getKeyStateImmediate(VK_LBUTTON))
	{
		if (newbutton)
		{
			selectionArea.left = selectionArea.right = pp.x;
			selectionArea.top = selectionArea.bottom = pp.y;
			newbutton = false;
		}
		else
		{
			selectionArea.right = pp.x;
			selectionArea.bottom = pp.y;
		}
		if (selectionArea.left < 0) selectionArea.left = 0;
		if (selectionArea.top < 0) selectionArea.top = 0;
		if (selectionArea.right < 0) selectionArea.right = 0;
		if (selectionArea.bottom < 0) selectionArea.bottom = 0;

		if (sprite)
		{
			if (selectionArea.left > sprite->size.width)
				selectionArea.left = sprite->size.width;
			if (selectionArea.right > sprite->size.width)
				selectionArea.right = sprite->size.width;
			if (selectionArea.top > sprite->size.height)
				selectionArea.top = sprite->size.height;
			if (selectionArea.bottom > sprite->size.height)
				selectionArea.bottom = sprite->size.height;
		}
	}
	else if (!newbutton)
		newbutton = true;

	if (keys->getKeyStateDelay(VK_RETURN, 0.33f))
	{
		if (selectionArea.left > selectionArea.right) std::swap(selectionArea.left, selectionArea.right);
		if (selectionArea.top > selectionArea.bottom) std::swap(selectionArea.bottom, selectionArea.top);
		frames.push_back(selectionArea);
	}

	if (keys->getKeyStateDelay(VK_BACK))
	{
		if (!frames.empty())
			frames.pop_back();
	}

	if (wParam >= MENU_25 && wParam < MENU_25 + 40)
	{
		Level::ClearChecks();
		CheckMenuItem(GetMenu(Level::hWnd), (UINT)wParam, MF_CHECKED);
		FrameSpeed = (((wParam - MENU_25) + 1) * 25.0f) / 1000.0f;
	}

	if (wParam == MENU_EXPORT)
	{
		char sFilePath[512] = {};
		sFilePath[0] = '\0';

		OPENFILENAME ofn = {};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = "SpriteFile (*.spr3)\0*.spr3\0";
		ofn.lpstrFile = sFilePath;
		ofn.nMaxFile = 512;
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = "spr3";
		if (GetSaveFileName(&ofn))
		{
			Export(sFilePath);
		}
	}

	if (wParam == MENU_EDITSPR3)
	{
		char sFilePath[512] = {};
		sFilePath[0] = '\0';

		OPENFILENAME ofn = {};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = "SpriteFile (*.spr3)\0*.spr3\0";
		ofn.lpstrFile = sFilePath;
		ofn.nMaxFile = 512;
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = "spr3";
		if (GetOpenFileName(&ofn))
		{
			OpenSPR3(sFilePath);
		}
	}
}

void Level1::ToggleGrid()
{
	if (bShowGrid) bShowGrid = false;
	else bShowGrid = true;
}

char* Level1::getExt(char* path)
{
	char* ext = NULL;
	for (int i = (int)strlen(path); i > 0; i--)
	{
		if (path[i] == '.')
		{
			ext = new char[strlen(path) - i + 1];
			memcpy(ext, &path[i], strlen(path) - i + 1);
			return ext;
		}
	}
	return ext;
}

char* Level1::getExt(wchar_t* wpath)
{
	if (!wpath)
	{
		char* w = new char[1];
		w[0] = '\0';
		return w;
	}
	int len = (int)wcslen(wpath);
	len++;
	size_t numConverted;
	char* path = new char[len];
	wcstombs_s(&numConverted, path, len, wpath, len);
	char* result = getExt(path);
	delete[] path;
	return result;
}

void Level1::OpenSPR3(const char* filePath)
{
	FILE* file;
	errno_t error = fopen_s(&file, filePath, "rb");
	if (error)	return;
	if (sprite) delete sprite;

	fseek(file, 0, SEEK_END);
	size_t bufferLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = new char[bufferLen];
	fread(buffer, bufferLen, 1, file);
	fclose(file);

	size_t position = 0;
	position += sizeof(float) * 4;
	memcpy(&FrameSpeed, buffer + position, sizeof(float));
	Level::ClearChecks();
	CheckMenuItem(GetMenu(Level::hWnd), MENU_25 + (UINT)(FrameSpeed / 25) - 1, MF_CHECKED);
	FrameSpeed /= 1000.0f;

	position += sizeof(float);
	double dVersion;
	memcpy(&dVersion, buffer + position, sizeof(double));		//this is in case i need the version information for some reason
	position += sizeof(double);
	int numFrames = 0;
	memcpy(&numFrames, buffer + position, sizeof(int));
	position += sizeof(int);
	position += sizeof(bool);									//legacy, not used

	std::vector<D2D1_RECT_F> empty;
	std::swap(empty, frames);
	for (int i = 0; i < numFrames; i++)
	{
		D2D1_RECT_F tFrames;
		memcpy(&tFrames.left, buffer + position, sizeof(float));
		position += sizeof(float);
		memcpy(&tFrames.top, buffer + position, sizeof(float));
		position += sizeof(float);
		memcpy(&tFrames.right, buffer + position, sizeof(float));
		position += sizeof(float);
		memcpy(&tFrames.bottom, buffer + position, sizeof(float));
		position += sizeof(float);
		frames.push_back(tFrames);
	}

	int extLen = 0;
	memcpy(&extLen, buffer + position, sizeof(int));
	position += sizeof(int);
	char* ext = new char[extLen + 1];
	ext[extLen] = '\0';
	memcpy(ext, buffer + position, extLen);
	position += extLen;
	memcpy(&bufferLen, buffer + position, sizeof(int));
	position += sizeof(int);
	
	/* old method
	std::string tempFilePath = filePath;
	tempFilePath.append(ext);
	delete[] ext;

	fopen_s(&file, tempFilePath.c_str(), "wb");
	fwrite(buffer + position, bufferLen, 1, file);
	fclose(file);
	delete[] buffer;

	if (wTempFilePath)
	{
		delete[] wTempFilePath;
	}

	size_t numConverted;
	int len = (int)tempFilePath.size() + 1;
	wTempFilePath = new wchar_t[len];
	mbstowcs_s(&numConverted, wTempFilePath, len, tempFilePath.c_str(), len);
	
	sprite = new SpriteSheet(wTempFilePath, Level::gfx);*/
	sprite = new SpriteSheet(buffer + position, bufferLen, Level::gfx);
}

void Level1::Export(const char *FilePath)
{
	/*
	FILE* file;	
	std::wstring wFilePath = sprite->FilePath;
	delete sprite;

	errno_t error = _wfopen_s(&file, wFilePath.c_str(), L"rb");	
	
	if (error)
	{
		MessageBox(NULL, "Failed to open file.", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	fseek(file, 0, SEEK_END);
	int bufferLen = (int)ftell(file);
	fseek(file, 0, SEEK_SET);
	char* databuffer = new char[bufferLen];
	fread(databuffer, bufferLen, 1, file);
	fclose(file);
	sprite = new SpriteSheet(wFilePath.c_str(), gfx);
	*/
	FILE* file = NULL;
	errno_t error = fopen_s(&file, FilePath, "wb");
	if (error)
	{
		MessageBox(NULL, "Failed to open file.", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	
	if (!sprite->DataBuffer)
	{
		MessageBox(NULL, "Failed to export the file. DataBuffer is NULL.", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	char* buffer = new char[sizeof(double) + sizeof(int) + sizeof(float) * 5 + sizeof(bool)];
	size_t position = 0;
	float null = 0;
	memcpy(buffer, &null, sizeof(float));
	position += sizeof(float);
	memcpy(buffer + position, &null, sizeof(float));
	position += sizeof(float);
	memcpy(buffer + position, &null, sizeof(float));
	position += sizeof(float);
	memcpy(buffer + position, &null, sizeof(float));
	position += sizeof(float);

	float FrameSpeedConverted = FrameSpeed * 1000.0f;
	memcpy(buffer + position, &FrameSpeedConverted, sizeof(float));
	position += sizeof(float);
	memcpy(buffer + position, &version, sizeof(double));
	position += sizeof(double);
	int maxframe = (int)frames.size();
	memcpy(buffer + position, &maxframe, sizeof(int));
	position += sizeof(int);
	bool custom = true;
	memcpy(buffer + position, &custom, sizeof(bool));
	position += sizeof(bool);

	fwrite(buffer, position, 1, file);
	delete[] buffer;

	size_t totalPosition = position;
	position = 0;
	buffer = new char[maxframe * 4 * sizeof(float)];
	for (int i = 0; i < maxframe; i++)
	{
		memcpy(buffer + position, &frames[i].left, sizeof(float));
		position += sizeof(float);
		memcpy(buffer + position, &frames[i].top, sizeof(float));
		position += sizeof(float);
		memcpy(buffer + position, &frames[i].right, sizeof(float));
		position += sizeof(float);
		memcpy(buffer + position, &frames[i].bottom, sizeof(float));
		position += sizeof(float);
	}
	fwrite(buffer, position, 1, file);
	delete[] buffer;
	char* ext = getExt(sprite->FilePath);
	int extLen = (int)strlen(ext);
	fwrite(&extLen, sizeof(int), 1, file);
	fwrite(ext, extLen, 1, file);
	delete[] ext;
	fwrite(&sprite->Bufferlen, sizeof(int), 1, file);
	fwrite(sprite->DataBuffer, sprite->Bufferlen, 1, file);
	fclose(file);
}

void Level1::ShiftSelectionArea(Direction direction, double deltaTime, float _speed)
{
	D2D1_POINT_2F cspeed = speed;
	if (_speed)
	{
		cspeed.x = _speed;
		cspeed.y = cspeed.x * (1080.0f / 1920.0f);
	}
	switch (direction)
	{
	case Direction::up:
		selectionArea.top -= (float)(cspeed.y * deltaTime);
		selectionArea.bottom -= (float)(cspeed.y * deltaTime);
		break;
	case Direction::down:
		selectionArea.top += (float)(cspeed.y * deltaTime);
		selectionArea.bottom += (float)(cspeed.y * deltaTime);
		break;
	case Direction::left:
		selectionArea.left -= (float)(cspeed.x * deltaTime);
		selectionArea.right -= (float)(cspeed.x * deltaTime);
		break;
	case Direction::right:
		selectionArea.left += (float)(cspeed.x * deltaTime);
		selectionArea.right += (float)(cspeed.x * deltaTime);
		break;
	}
}

void Level1::Move(Direction direction, double deltaTime, float _speed)
{
	D2D1_POINT_2F cspeed = speed;
	if (_speed)
	{
		cspeed.x = _speed;
		cspeed.y = cspeed.x * (1080.0f / 1920.0f);
	}
	switch (direction)
	{
	case Direction::up:
		translation.height += (float)(cspeed.y * deltaTime);
		break;
	case Direction::down:
		translation.height -= (float)(cspeed.y * deltaTime);
		break;
	case Direction::left:
		translation.width += (float)(cspeed.x * deltaTime);
		break;
	case Direction::right:
		translation.width -= (float)(cspeed.x * deltaTime);
		break;
	}
}

void Level1::Zoom(bool in)
{
	if (in)
		scale += 0.01f;
	else
		scale -= 0.01f;
}

void Level1::ChangeSpeed(bool increase)
{
	if (increase)
	{
		speed.x += 50.0f;
	}
	else
	{
		speed.x -= 50.0f;
	}
	if (speed.x < 0)
		speed.x = 0;
	speed.y = speed.x * (1080.0f / 1920.0f);
}

void Level1::BuildGrid()
{
	if (bGrid) bGrid->Release();
	ID2D1BitmapRenderTarget* t = NULL;
	ID2D1SolidColorBrush* tbrush = NULL;
	gfx->GetDrawArea()->CreateCompatibleRenderTarget(&t);
	t->CreateSolidColorBrush({ 0.0f, 0.0f, 0.0f,1.0f }, &tbrush);
	t->BeginDraw();
	t->Clear({});
	for (long i = 0; i < 1920; i += 5)
	{
		t->DrawLine(D2D1::Point2F((float)i, 0), D2D1::Point2F((float)i, 1080), tbrush, 0.25f);
	}
	for (long i = 0; i < 1080; i += 5)
	{
		t->DrawLine(D2D1::Point2F(0, (float)i), D2D1::Point2F(1920, (float)i), tbrush, 0.25f);
	}
	t->EndDraw();
	t->GetBitmap(&bGrid);
	tbrush->Release();
	t->Release();
}