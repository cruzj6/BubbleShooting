#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(LPCWSTR fileName, Graphics* gfx)
{
	this->gfx = gfx;
	bmp = NULL;
	HRESULT hr;

	//Create out wic factory
	IWICImagingFactory* wicFactory = NULL;
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);
	
	//Use the factory to create our wic decoder
	IWICBitmapDecoder* wicDecoder = NULL;
	hr = wicFactory->CreateDecoderFromFilename(fileName, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
	
	//Read a frame from img
	IWICBitmapFrameDecode* wicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &wicFrame);

	//Now we need a converter to convert it to direct2D
	IWICFormatConverter* wicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&wicConverter);

	//Initialize our converter with our read image
	hr = wicConverter->Initialize(wicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);

	//Now create our ID2D1Bitmap
	gfx->GetRenderTarget()->CreateBitmapFromWicBitmap(wicConverter, NULL, &bmp);

	if (wicFactory) wicFactory->Release();
	if (wicDecoder) wicDecoder->Release();
	if (wicConverter) wicConverter->Release();
	if (wicFrame) wicFrame->Release();

}

SpriteSheet::~SpriteSheet()
{
	if (bmp) bmp->Release();
}

void SpriteSheet::Draw()
{
	gfx->GetRenderTarget()->DrawBitmap(
		bmp,
		D2D1::RectF(0.0f, 0.0f,
		bmp->GetSize().width, bmp->GetSize().height),
		0.25f,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,//Stretch or shrink behavior
		D2D1::RectF(0.0f, 0.0f,
		bmp->GetSize().width, bmp->GetSize().height)
		);

}

void SpriteSheet::Draw(int x, int y, float width, float height)
{
	gfx->GetRenderTarget()->DrawBitmap(
		bmp,
		D2D1::RectF(x, y,
		x + width, y + height),//Destination
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,//Stretch or shrink behavior
		D2D1::RectF(0.0f, 0.0f,
		bmp->GetSize().width, bmp->GetSize().height)//Source
		);

}
