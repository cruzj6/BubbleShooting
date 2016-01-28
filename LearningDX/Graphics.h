#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

class Graphics
{
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* rendertarget;
	ID2D1SolidColorBrush* brush;
	IDWriteFactory* writeFactory;

	public:
		Graphics();
		~Graphics();

		ID2D1RenderTarget* GetRenderTarget()
		{
			return rendertarget;
		}
		bool Init(HWND windowHandle);

		void BeginDraw() 
		{ 
			rendertarget->BeginDraw();
		}
		void EndDraw()
		{
			rendertarget->EndDraw();
		}
		void ClearScreen(float r, float g, float b);
		void DrawCircle(float x, float y, float radius, float r, float g, float b, float a);
		void ClearZone(float x, float y, float len, float width);
		void ClearZoneCircle(float x, float y, float radius);
		void DrawLine(float xStart, float xFin, float yStart, float yFin, float r, float b, float g);
		void WriteText(WCHAR text, float l, float t, float ri, float bot);
	private:

};