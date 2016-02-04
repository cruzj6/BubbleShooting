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

	//Set up out bitmap rendertarget, setting it to the height by height of the bitmap for rotations
	hr = gfx->GetRenderTarget()->CreateCompatibleRenderTarget(
		D2D1::SizeF(bmp->GetSize().height, bmp->GetSize().height),
		&pCompatibleRenderTarget
		);


	if (wicFactory) wicFactory->Release();
	if (wicDecoder) wicDecoder->Release();
	if (wicConverter) wicConverter->Release();
	if (wicFrame) wicFrame->Release();

}

SpriteSheet::~SpriteSheet()
{
	if (bmp) bmp->Release();
	if (pCompatibleRenderTarget) pCompatibleRenderTarget->Release();
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

//NULL width to scale with height
//X and Y center location, width of final and height of final, and the angle to rotate original
void SpriteSheet::DrawRotatedBitmap(float x, float y, float angle, float width, float height)
{
	//Rotate transform around the center
	pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(bmp->GetSize().height/2, bmp->GetSize().height/2)));

	pCompatibleRenderTarget->BeginDraw();

	pCompatibleRenderTarget->Clear(D2D1::ColorF(0xF, 0xF, 0xF, 0));

	//Draw to the bitmap Render target
	pCompatibleRenderTarget->DrawBitmap(
		bmp,
		D2D1::RectF(bmp->GetSize().height/2 - (bmp->GetSize().width / 2), 0,
		bmp->GetSize().width/2 + bmp->GetSize().height / 2, bmp->GetSize().height),//Destination
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,//Stretch or shrink behavior
		D2D1::RectF(0.0f, 0.0f,
		bmp->GetSize().width, bmp->GetSize().height)//Source
		);

	pCompatibleRenderTarget->EndDraw();

	//Get out bitmap from the bitmap render target
	ID2D1Bitmap* bitMap;
	pCompatibleRenderTarget->GetBitmap(&bitMap);

	//Finally draw rotated bitmap to our window's render target
	if (width != NULL){
		gfx->GetRenderTarget()->DrawBitmap(
			bitMap,
			D2D1::RectF(x - width / 2, y - height / 2,
			x + width / 2, y + height / 2),//Destination
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,//Stretch or shrink behavior
			D2D1::RectF(0.0f, 0.0f,
			bitMap->GetSize().width, bitMap->GetSize().height)//Source
			);
	}
	if (width == NULL)
	{
		float scale = height / bitMap->GetSize().height;
		float scaleXWidth = scale * bitMap->GetSize().width;
		gfx->GetRenderTarget()->DrawBitmap(
			bitMap,
			D2D1::RectF(x - scaleXWidth / 2, y - height / 2,
			x + scaleXWidth / 2, y + height / 2),//Destination
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,//Stretch or shrink behavior
			D2D1::RectF(0.0f, 0.0f,
			bitMap->GetSize().width, bitMap->GetSize().height)//Source
			);
	}

	//Release com bitmap object
	if(bitMap) bitMap->Release();
}

float SpriteSheet::GetBmpHeight()
{
	return bmp->GetSize().height;
}

float SpriteSheet::GetBmpWidth()
{
	return bmp->GetSize().width;
}