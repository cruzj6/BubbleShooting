#include <Windows.h>
#include <d2d1.h>
#include "Graphics.h"
#include "GameController.h"
#include "GameLevel.h"
#include "Level1.h"

Graphics* graphics;

//Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

	GameLevel* level1 = new Level1();
	GameController::LoadInitialLevel(level1);
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, mWindHandle, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
#pragma region UpdatesPerFrame
		else
		{
			GameController::Update();
			GameController::Render(graphics);
		}
#pragma endregion

	}

	delete graphics;
	delete level1;

	return 0;
}