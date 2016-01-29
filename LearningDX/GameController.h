#pragma once
#include "GameLevel.h"
#include "Graphics.h"
#include "HPTimer.h"

class GameController
{
private:
	static GameLevel* currentLevel;
	static HPTimer* hpTimer;
	GameController();

public:
	static void Init();
	static bool levelLoading;
	static void LoadInitialLevel(GameLevel* level);
	static void SwitchLevel(GameLevel* newLevel);
	static void Render();
	static void Update();
};