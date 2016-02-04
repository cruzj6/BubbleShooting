#include <Windows.h>
#include <d2d1.h>
#include "Graphics.h"
#include "GameController.h"
#include "GameLevel.h"
#include "Level1.h"
#include <windowsx.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

Graphics* graphics;
GameLevel* level1;

//Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_LBUTTONDOWN:
			((Level1*)level1)->FireBall(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_MOUSEMOVE:
			level1->SetCurrentMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

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

	/*AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;*/

	graphics = new Graphics();
	if (!graphics->Init(mWindHandle))
	{
		delete graphics;
		return -1;
	}

	ShowWindow(mWindHandle, nCmdShow);
	MSG message;
	message.message = WM_NULL;

	GameController::Init();

	level1 = new Level1(graphics);
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
			GameController::Render();
		}
#pragma endregion

	}

	level1->UnLoad();
	delete graphics;
	delete level1;

	return 0;
}