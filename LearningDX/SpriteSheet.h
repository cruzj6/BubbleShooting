#pragma once

#include <wincodec.h>
#include "Graphics.h"

class SpriteSheet
{
	Graphics* gfx;
	ID2D1Bitmap* bmp;
	ID2D1BitmapRenderTarget *pCompatibleRenderTarget;

public: 
	SpriteSheet(LPCWSTR fileName, Graphics* gfx);

	~SpriteSheet();

	void Draw(int x, int y, float width, float height);
	void Draw();
	void DrawRotatedBitmap(float x, float y, float angle, float width, float height);
	float GetBmpWidth();
	float GetBmpHeight();
};