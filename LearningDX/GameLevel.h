#pragma once
#include "Graphics.h"
#include "SpriteSheet.h"

class GameLevel
{
protected: 
	SpriteSheet* sprites;
	SpriteSheet* UIArrow;
	static Graphics* gfx;
	float mouseXPos;
	float mouseYPos;

public:
	virtual void Load() = 0;
	virtual void UnLoad() = 0;
	virtual void Render() = 0;
	virtual void Update(double timeTotal, double timeDelta) = 0;
	virtual void SetCurrentMousePos(float x, float y) = 0;
};