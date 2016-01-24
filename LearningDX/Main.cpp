#include <Windows.h>
#include <d2d1.h>
#include "Graphics.h"

const float SPAWN_RATE = 1000000.0f;
const float CIRCLE_RADIUS = 20.0f;
const float Y_SPAWN_SPEED = 10.0f;//TODO: change later
const int NUM_ROWS = 10;
const int NUM_COLS = 10;

Graphics* graphics;

struct ballObject
{
	int isMake;
	int yDestination;
	int xDestination;
};

//Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void RenderNewSpawnBall(float xDest, float yDest, float frameYPos);
void UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject newBallProp, bool &renderNewBall);
void AddBallToArray(ballObject** ballArray, ballObject newBall);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default: //Default window processing procedure, ensures all messages are processed
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow)
{
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	//Set up our WNDCLASSEX struct to hand to the window
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.lpszClassName = "MainWindow";

	RegisterClassEx(&windowClass);

	RECT rect = { 0, 0, 800, 600 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND mWindHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Learning DirectX", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, 0);

	if (!mWindHandle)
	{
		return -1;
	}

	graphics = new Graphics();
	if (!graphics->Init(mWindHandle))
	{
		delete graphics;
		return -1;
	}

	ShowWindow(mWindHandle, nCmdShow);
	MSG message;
	message.message = WM_NULL;

	ballObject** balls = new ballObject *[NUM_COLS];
	for (int i = 0; i < NUM_COLS; i++)
	{
		balls[i] = new ballObject[NUM_ROWS];
	}

	for (int j = 0; j < 5; j++)//For each col
	{
		for (int i = 0; i < 10; i++)//for each row
		{
			float randNum = rand() % 100;
			if (randNum > 50) {
				ballObject bp;
				bp.isMake = 1;
				bp.yDestination = 40 * (j + 1);
				bp.xDestination = 40 * (i + 1);
				balls[i][j] = bp;
			}
			else {
				ballObject bp;
				bp.isMake = 0;
				balls[i][j] = bp;
			}
		}
	}

	//y Speed!
	float y = 600;
	float newBallY = 600;
	float spawnCountDown = SPAWN_RATE;
	bool renderNewBall = false;
	bool newBallCreated = false;
	ballObject* bp = {0}; //Used for creating new balls after the initial balls

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, mWindHandle, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
#pragma region UpdatesPerFrame
		else
		{
			spawnCountDown -= 1;

			//Initial Entry for the balls
			if (y > 0)
			{
				y -= Y_SPAWN_SPEED;

				//If no message lets update
				graphics->BeginDraw();
				graphics->ClearScreen(0xF, 0xF, 0xF);

				for (int j = 0; j < 5; j++)//For each row
				{
					for (int i = 0; i < 10; i++)//for each col
					{
						ballObject* ball = &balls[i][j];
						float circleRadius = CIRCLE_RADIUS;
						if (ball->isMake != 0)
							graphics->DrawCircle(ball->xDestination, ball->yDestination + y, circleRadius, 0.0, 0.0, 0xF, 1.0);
					}
				}
				graphics->EndDraw();
			}
			else if (renderNewBall)//if it is time to spawn one move a new one in
			{
				if (!newBallCreated){
					float diameter = 2 * CIRCLE_RADIUS;
					float randPos = rand() % (int)(800 / diameter);//20 different possible positions in 800px

					//Transition the ball in
					bp = new ballObject();
					bp->xDestination = 40 * randPos;
					bp->yDestination = 40 * 5; //TODO add to last row
					newBallCreated = true;

					AddBallToArray(balls, *bp);
				}

				
				UpdateNewBall(newBallY, spawnCountDown, newBallCreated, *bp, renderNewBall);
			}

			//If it is time to spawn a new ball, set the render flag to true
			if (spawnCountDown <= 0) {
				renderNewBall = true; newBallY = 600;
			}
		}
	}
#pragma endregion
	delete graphics;
	return 0;
}

void UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject bp, bool &renderNewBall)
{
	newBallY -= Y_SPAWN_SPEED;
	spawnCountDown = SPAWN_RATE;//Reset the countdown

	//If we are at our destination set the render flag to false
	if (newBallY <= 0) {
		renderNewBall = false;
		newBallCreated = false;
	}

	RenderNewSpawnBall(bp.xDestination, bp.yDestination, newBallY);
}

void RenderNewSpawnBall(float ballXDest, float ballYDest, float frameYPos)
{
	graphics->BeginDraw();

	float clearRadius = CIRCLE_RADIUS + 3.0f; //brush stroke is 3.0f on circles
	graphics->ClearZoneCircle(ballXDest, ballYDest + frameYPos + Y_SPAWN_SPEED, clearRadius);
	graphics->DrawCircle(ballXDest, ballYDest + frameYPos, 20, 0.0, 0.0, 0xF, 1.0);

	graphics->EndDraw();
}

void AddBallToArray(ballObject** ballArray, ballObject newBall)
{
	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			ballArray[i][j] = newBall;
		}
	}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                