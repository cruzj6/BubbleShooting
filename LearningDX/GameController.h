#pragma once
#include "GameLevel.h"
#include "Graphics.h"

class GameController
{
private:
	static GameLevel* currentLevel;
	GameController();

public:
	static bool levelLoading;
	static void LoadInitialLevel(GameLevel* level);
	static void SwitchLevel(GameLevel* newLevel);
	static void Render();
	static void Update();
};