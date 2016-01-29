#include "GameController.h"

GameLevel* GameController::currentLevel;
bool GameController::levelLoading;
HPTimer* GameController::hpTimer;

void GameController::Init()
{
	currentLevel = 0;
	hpTimer = new HPTimer();
}

void GameController::LoadInitialLevel(GameLevel* level)
{
	levelLoading = true;
	currentLevel = level;
	level->Load();
	levelLoading = false;
}

void GameController::Update()
{
	hpTimer->Update();
	currentLevel->Update(hpTimer->GetTimeTotal(), hpTimer->GetTimeDelta());
}

void GameController::Render()
{
	currentLevel->Render();
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