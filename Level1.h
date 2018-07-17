#pragma once
#include "Graphics.h"
#include "SpriteSheet.h"
#include "HPTimer.h"
#include "Level.h"
#include "inputs.h"
#include "defines.h"
#include <vector>

class Level1 : public Level
{
private:
	float scale;
	D2D1_SIZE_F translation;
	D2D1_MATRIX_3X2_F transform;
	D2D1_RECT_F selectionArea;
	D2D1::Matrix3x2F transformMatrix;
	std::vector<D2D1_RECT_F> frames;
	SpriteSheet* sprite;	
	wchar_t* wTempFilePath = NULL;
	ID2D1Bitmap* bGrid = NULL;
private:
	bool newbutton;
	D2D1_POINT_2F speed;
	float precisionSpeed;
	inputs* keys;
	float previewSize;
	long currentFrame;
public:
	Level1(Graphics* _gfx);
	~Level1();
public:
	void LoadLevel() override;
	void UnloadLevel() override;
	bool OpenImage(HWND hWnd) override;
	bool OpenImage(const wchar_t* filePath) override;
	void Render() override;
	void RenderDrawArea() override;
	void RenderPreviewArea() override;
	void Update(WPARAM wParam, LPARAM lParam) override;	
	void Export(const char *FilePath) override;
	void OpenSPR3(const char* filePath) override;
	void BuildGrid();
	void ToggleGrid() override;
public:
	void Move(Direction direction, double deltaTime, float _speed = 0.0f) override;
	void Resize(Direction direction, double deltaTime, float _speed = 0.0f);
	void ShiftSelectionArea(Direction direction, double deltaTime, float _speed = 0.0f) override;
	void Zoom(bool in = true) override;
	void ChangeSpeed(bool increase = true) override;
	HPTimer* timer, *previewTimer;
public:
	void DrawPreviewMovement(D2D1_RECT_F area);
	void DrawLastFramePreview(D2D1_RECT_F area);
	double LastTimeValue, CurrentTimeValue;
	float FrameSpeed;
	char* getExt(wchar_t* wpath);
	char* getExt(char* path);
	bool bShowGrid = false;
	bool doit = false;
};