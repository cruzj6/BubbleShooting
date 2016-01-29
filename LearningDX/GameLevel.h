#pragma once
#include "Graphics.h"
#include "SpriteSheet.h"

class GameLevel
{
protected: 
	SpriteSheet* sprites;
	static Graphics* gfx;

public:
	virtual void Load() = 0;
	virtual void UnLoad() = 0;
	virtual void Render() = 0;
	virtual void Update(double timeTotal, double timeDelta) = 0;
};