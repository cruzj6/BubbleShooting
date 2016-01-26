#include "GameController.h"

GameLevel* GameController::currentLevel;
bool GameController::levelLoading;

void GameController::LoadInitialLevel(GameLevel* level)
{
	levelLoading = true;
	currentLevel = level;
	level->Load();
	levelLoading = false;
}

void GameController::Update()
{
	currentLevel->Update();
}

void GameController::Render(Graphics* gfx)
{
	currentLevel->Render(gfx);
}

void  GameController::SwitchLevel(GameLevel* newLevel)
{
	levelLoading = true;
	currentLevel->UnLoad();
	delete currentLevel;
	currentLevel = newLevel;
	currentLevel->Load();
	levelLoading = false;
}