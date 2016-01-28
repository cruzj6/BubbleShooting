#pragma once

#include <wincodec.h>
#include "Graphics.h"

class SpriteSheet
{
	Graphics* gfx;
	ID2D1Bitmap* bmp;
public: 
	SpriteSheet(LPCWSTR fileName, Graphics* gfx);

	~SpriteSheet();

	void Draw(int x, int y, float width, float height);
	void Draw();
};