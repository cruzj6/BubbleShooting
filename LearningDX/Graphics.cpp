#include "Graphics.h"

Graphics::Graphics()
{
	factory = NULL;
	rendertarget = NULL;
}

Graphics::~Graphics()
{
	//Release all COM interfaces
	if (factory)
		factory->Release();
	if (rendertarget)
		rendertarget->Release();
	if (brush)
		brush->Release();
}

bool Graphics::Init(HWND windowHandle)
{
	//Pass pointer to the pointer to the factory so it can create it
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (res != S_OK)
		return false;

	//Get the client window's rect size
	RECT rect;
	GetClientRect(windowHandle, &rect);

	//Set the render target up for the factory
	res = factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom)),
		&rendertarget);

	if (res != S_OK)
		return false;

	return true;
}

void Graphics::ClearScreen(float r, float g, float b)
{
	rendertarget->Clear(D2D1::ColorF(r, g, b));
}

void Graphics::ClearZoneCircle(float x, float y, float radius)
{
	D2D1_ELLIPSE elip;
	elip.point = D2D1::Point2F(x, y);
	elip.radiusX = radius;
	elip.radiusY = radius;

	brush->SetColor(D2D1::ColorF(0xA, 0xB, 0x2, 3.0));

	rendertarget->FillEllipse(elip, brush);
}

void Graphics::ClearZone(float x, float y, float len, float width)
{
	D2D1_RECT_F theRect;
	theRect.bottom = y + width/2;
	theRect.top = y - width/2;
	theRect.left = x - len / 2;
	theRect.right = x + len/2;

	brush->SetColor(D2D1::ColorF(0xF, 0xF, 0xF, 3.0));

	rendertarget->FillRectangle(theRect, brush);
}
void Graphics::DrawCircle(float x, float y, float radius, float r, float g, float b, float a)
{
	rendertarget->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a), &brush);
	rendertarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush, 3.0f);
}


void Graphics::DrawLine(float xStart, float xFin, float yStart, float yFin, float r, float b, float g)
{
	D2D1_POINT_2F startPoint = D2D1::Point2F(xStart, yStart);
	D2D1_POINT_2F endPoint = D2D1::Point2F(xFin, yFin);
	brush->SetColor(D2D1::ColorF(r, b, g, 1.0f));
	rendertarget->DrawLine(startPoint, endPoint, brush);
}