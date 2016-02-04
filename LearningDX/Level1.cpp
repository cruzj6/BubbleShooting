#include "GameLevel.h"
#include "Level1.h"
#include <math.h>
#include <cmath>
#include <sstream>
#include <iostream>

const float SPAWN_TIME= 5.0f;//In Seconds
float X_RES = 800;//Keep these Res 4:3
float Y_RES = 600;//----TODO: Changed these to get render target size-----
				  //Test it
				  //This needs to account for different native res than render tar res when using mouse coords
				  //Maybe window at start to request Res selection 4:3
const int NUM_ROWS = 20;
const int NUM_COLS = 20;
const float CIRCLE_RADIUS = (X_RES / NUM_COLS)/2;
const float Y_SPAWN_RATE = 1000.0f;//For entry transition and random new balls
const float FIRE_RATE = 1000.0f;
const float GRID_SPACE_SIZE = 50.0f;
const float PERCENT_OF_SCREEN_INIT_BALLS = 0.3f;//What percent of Y space do they go to
const float pi = 3.14;

//Values that need to be updated based on time, not frames
//These are set at runtime each frame
float FIRE_SPEED;
float Y_SPAWN_SPEED;
float SPAWN_RATE;

//Entry Transition vars
float y = Y_RES;
float newBallY = Y_RES;
bool isEntry = true;

//Random ball entry
float spawnCountDown;
float score = 0;
	
//Vars for a fired ball
float ballFireXDirection;
float ballFireYDirection;
bool firingBall;
bool isNoChain = true; //Used for the UpdatePopBalls() Recursive method
	
//Ball object pointers
ballObject* bp = { 0 }; //Used for creating new balls after the initial balls
ballObject** balls; //The array of balls in their final destination
ballObject* bpFire = { 0 }; //Pointer to a ball object that is being fired as it is being fired
ColorTypes nextShootColor;

//Temp random num funcs
float randomNum(float low, float high);
float hash3(float num0, float num1, float num2);

//Sets initial values, initializes initial ball objects to be
//rendered at the start of the level, and allocates memory
//for possible ones to come
void Level1::Load()
{
	spawnCountDown = SPAWN_TIME;
	sprites = new SpriteSheet(L"test.png", gfx);
	UIArrow = new SpriteSheet(L"Aim_Arrow.png", gfx);
	UIImage = new SpriteSheet(L"UIBar.png", gfx);

	balls = new ballObject *[NUM_ROWS];
	for (int i = 0; i < NUM_COLS; i++)
	{
		balls[i] = new ballObject[NUM_ROWS];
	}

	LoadInitialLevelBalls();
	PrintBallArray();
}

//Loads the ball objects for starting out into the array
void Level1::LoadInitialLevelBalls()
{
	//Initial next shoot color
	nextShootColor = GetRandomColor();

	for (int i = 0; i < NUM_COLS; i++)//For each col
	{
		for (int j = 0; j < NUM_ROWS; j++)//for each row
		{
			//Generate a random number to see if a ball will be generated for this spot on the screen
			float randNum = randomNum(0, 100); //% 100;

			//50% chance that it will be created
			if (randNum > 50 && j < NUM_ROWS * PERCENT_OF_SCREEN_INIT_BALLS) {
				//Create the new ball and initialize it's properties, setting exist to true so it will render
				ballObject newBall;
				newBall.yDestination = (CIRCLE_RADIUS * 2) * (j + 1);
				newBall.xDestination = (CIRCLE_RADIUS * 2) * (i + 1);
				newBall.currentLocationX = newBall.xDestination;
				newBall.currentLocationY = newBall.yDestination;
				newBall.exists = true;
				newBall.color = GetRandomColor();
				balls[i][j] = newBall;
			}
			else {
				//initialize memory for ball that does not exist yet
				ballObject newBall;
				newBall.exists = false;
				balls[i][j] = newBall;
			}
		}
	}
}

//Release resources
void Level1::UnLoad()
{
	delete bp;
	delete bpFire;
	delete balls;
	delete sprites;
}

float randomNum(float low, float high)
{
	float pid = GetCurrentProcessId();
	SYSTEMTIME tod;
	GetSystemTime(&tod);
	return static_cast<int>(hash3(tod.wMilliseconds, pid, rand()))% 100;
}

//TODO: Make hash better
float hash3(float num0, float num1, float num2)
{
	int num = static_cast<int>(num0 + num1) << static_cast<int>(num2);
	num ^= static_cast<int>(num0);
	return num;
}

//Update values every frame
void Level1::Update(double timeTotal, double timeDelta)
{
	//Update X and Y resolutions in the context of rendering
	X_RES = gfx->GetRenderTarget()->GetSize().width;
	Y_RES = gfx->GetRenderTarget()->GetSize().height;

	UpdateSpeedForTime(timeTotal, timeDelta);
	spawnCountDown -= timeDelta;

	//If the spawn counter is <= 0 create a new ball to spawn
	if (spawnCountDown <= 0)
	{
		ballObject* newBall = new ballObject();
		int max = (X_RES / NUM_COLS);
		float randomNum = (rand() % max);

		//Deduct from timer
		//TODO: Display timer?
		spawnCountDown = SPAWN_TIME;

		newBall->currentLocationX = randomNum * NUM_COLS;
		newBall->currentLocationY = Y_RES;
		newBall->transitioningIn = true;
		newBall->exists = true;
		AddBallToArray(*newBall);
		delete newBall;
	}

	if (y > 0)
	{
		y -= Y_SPAWN_SPEED;
	}
	else
	{
		isEntry = false;
	}

	if (firingBall)
	{
		bool shouldStop = CheckBallShouldStop(bpFire);
		//Stop making the ball if it is in a good spot
		if (bpFire->currentLocationX >= X_RES || bpFire->currentLocationY <= 0.0f || shouldStop)
		{
			firingBall = false;
			bpFire->exists = true;
			UpdatePopBalls(bpFire, NULL);
			isNoChain = true;
			if (bpFire->exists)
			{
				AddBallToArray(*bpFire);
			}
			delete bpFire;
		}
		else
		{
			//Update the ball being fired
			bpFire->currentLocationX += ballFireXDirection * (FIRE_SPEED);
			bpFire->currentLocationY -= ballFireYDirection * (FIRE_SPEED);
		}
	}	
	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			if (balls[i][j].transitioningIn)
			{
				UpdateNewBall(&balls[i][j]);
			}
		}
	}

}

//Updates numbers, scaling them with the amount of time passed since last frame
void Level1::UpdateSpeedForTime(double timeTotal, double timeDelta)
{
	FIRE_SPEED = FIRE_RATE * timeDelta;
	Y_SPAWN_SPEED = Y_SPAWN_RATE * timeDelta;
	SPAWN_RATE = SPAWN_TIME * timeDelta;
}

void Level1::Render()
{
		//If no message lets update
		gfx->BeginDraw();
		if (isEntry){//Render our entry transition if this is the entry time
			gfx->ClearScreen(0xF, 0xF, 0xF);
			for (int j = 0; j < NUM_COLS; j++)//For each row
			{
				for (int i = 0; i < NUM_ROWS; i++)//for each col
				{
					float r = 0;
					float b = 0;
					float g = 0;
					ballObject* ball = &balls[j][i];
					float circleRadius = CIRCLE_RADIUS;
					if (ball->exists) {
						GetColorRBG(balls[i][j].color, &r, &b, &g);
						sprites->Draw(ball->currentLocationX - CIRCLE_RADIUS, ball->currentLocationY + y - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
						gfx->DrawCircle(ball->currentLocationX, ball->currentLocationY + y, circleRadius,r, g, b, 1.0);
					}
				}
			}
		}
		else
		{//Render things for playing the level
			RenderBallArray();
			RenderFiringBall();
		}
		DrawGrid();
		RenderUI();
	//	PrintBallArray();
		gfx->EndDraw();	
}

//Renders the ball that is in the process on being fired
void Level1::RenderFiringBall()
{
	if (firingBall)
	{
		float r;
		float b;
		float g;
		GetColorRBG(bpFire->color, &r, &b, &g);
		sprites->Draw(bpFire->currentLocationX - CIRCLE_RADIUS, bpFire->currentLocationY - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
		gfx->DrawCircle(bpFire->currentLocationX, bpFire->currentLocationY, CIRCLE_RADIUS, r, g, b, 1.0);
	}
}

//Calls each function that renders the UI for each frame
void Level1::RenderUI()
{
	UIImage->Draw(0, Y_RES / 5 * 4, X_RES, Y_RES / 5);
	RenderNextColorDisplay();
	RenderTimeToNewBall();
	RenderUIArrow();
	RenderScore();
}

void Level1::RenderUIArrow()
{
	float xVecStart = X_RES / 2;
	float yVecStart = Y_RES;
	float vectorLen = sqrt(pow(xVecStart - mouseXPos, 2) + pow(Y_RES - mouseYPos, 2));

	float xNormDirec = (xVecStart - mouseXPos) / vectorLen;
	float yNormDirec = (yVecStart - mouseYPos) / vectorLen;

	float arrowLen = Y_RES * (0.5);
	float scale = arrowLen / UIArrow->GetBmpHeight();

	//Rotate bitmap of arrow, width is NULL as the method will scale with height if x is null
	//See: http://www.gamedev.net/topic/605182-direct2d-flipping-bitmaps/
	UIArrow->DrawRotatedBitmap(X_RES / 2, Y_RES, (atan2(yNormDirec, xNormDirec)/pi * 180) - 90, NULL, arrowLen);

}

void Level1::RenderTimeToNewBall()
{
	//Build our string
	std::wostringstream secs;
	secs << static_cast<int>(spawnCountDown);
	secs << " ";
	secs << "Seconds till random ball";

	//Convert it to WCHAR_T string
	std::wstring ws = secs.str();
	const WCHAR* secsWChar = ws.c_str();

	//Cast it to non-const
	WCHAR* nonConstStr = const_cast<WCHAR*>(secsWChar);

	//Finally draw it
	gfx->WriteText(nonConstStr, 500, 300, 800, 600);
}

void Level1::RenderScore()
{
	//Build our string
	std::wostringstream scoreTxt;

	scoreTxt << "Score: " << static_cast<int>(score);

	//Convert it to WCHAR_T string
	std::wstring ws = scoreTxt.str();
	const WCHAR* scoreWChar = ws.c_str();

	//Cast it to non-const
	WCHAR* nonConstStr = const_cast<WCHAR*>(scoreWChar);

	//Finally draw it
	gfx->WriteText(nonConstStr, 100, 300, 300, 600);
}

//Renders the text and the Color diplay itself stating the color of the next ball
//that will be shot
void Level1::RenderNextColorDisplay()
{
	float r;
	float b;
	float g;
	float xlen = 75.0f;
	float ylen = 75.0f;
	float xlocation = X_RES - 200 - (xlen / 2);
	float ylocation = Y_RES - 10 - (ylen / 2);	
	wchar_t* text = GetStringFromColorType(nextShootColor);

	//Write next color header and the color name itself
	gfx->WriteText(L"Next Color: ", xlocation - (xlen), ylocation - (ylen * 2.5), xlocation + 100, ylocation + 50);
	gfx->WriteText(text, xlocation - (xlen * 1.5), ylocation - (ylen * 2), xlocation + 100, ylocation + 50);
	GetColorRBG(nextShootColor, &r, &b, &g);
	gfx->DrawBox(xlocation, ylocation, ylen, xlen, r, g, b, 1.0);
}

wchar_t* Level1::GetStringFromColorType(ColorTypes color)
{
	switch (color)
	{
		case ColorTypes::BLUE:
			return L"Blue";
		case ColorTypes::GREEN:
			return L"Green";
		case ColorTypes::ORANGE:
			return L"Orange";
		case ColorTypes::RED:
			return L"Red";
		case ColorTypes::YELLOW:
			return L"Yellow";
		default:
			return L"Error";
	}
}

void Level1::RenderBallArray()
{
	float r = 0;
	float g = 0;
	float b = 0;

	gfx->ClearScreen(0xF, 0xF, 0xF);
	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			if (balls[i][j].exists)
			{
				GetColorRBG(balls[i][j].color, &r, &b, &g);
				sprites->Draw(balls[i][j].currentLocationX - CIRCLE_RADIUS, balls[i][j].currentLocationY - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
				gfx->DrawCircle(balls[i][j].currentLocationX, balls[i][j].currentLocationY, CIRCLE_RADIUS, r, g, b, 1.0);
			}
		}
	}
}

void Level1::GetColorRBG(ColorTypes colorNum, float* r, float* b, float* g)
{
	//Colors are 0 - 1 scale per color (RGB)
	switch (colorNum)
	{
		case ColorTypes::BLUE:
			*r = 0x0;
			*g = 0x0;
			*b = 1;
			break;

		case ColorTypes::GREEN:
			*r = 0x0;
			*g = 0xF;
			*b = 0x0;
			break;
		case ColorTypes::ORANGE:
			*r = 1;
			*g = 0.4;
			*b = 0;
			break;
		case ColorTypes::RED:
			*r = 0xF;
			*g = 0x0;
			*b = 0x0;
			break;
		case ColorTypes::YELLOW:
			*r = .8;
			*g = 1;
			*b = 0x0;
			break;

		default:
			*r = 0x0;
			*b = 0x0;
			*g = 0x0;
			return;
	}
}

void Level1::FireBall(float mouseX, float mouseY)
{
	float xCenter = X_RES / 2;
	float vectorLen = sqrt(pow(xCenter - mouseX, 2) + pow(Y_RES - mouseY, 2));

	firingBall = true;
	ballFireXDirection =  (mouseX - xCenter) / vectorLen;
	ballFireYDirection = (Y_RES - mouseY) / vectorLen;
	bpFire = new ballObject();
	bpFire->currentLocationX = X_RES/2;
	bpFire->currentLocationY = Y_RES;

	//Copy the memory because were about to change the value at that address
	memcpy(&bpFire->color, &nextShootColor, sizeof(ColorTypes));

	//Randomize next color
	nextShootColor = GetRandomColor();
	
}

void Level1::UpdateNewBall(ballObject* theBall)
{
	theBall->currentLocationY -= Y_SPAWN_SPEED;
	if (CheckBallShouldStop(theBall))
	{
		//It is done moving in now
		theBall->transitioningIn = false;
	}
}	

bool Level1::CheckBallShouldStop(ballObject* ball)
{
	//TODO: DISTANCE JOEY YOU DUMB SHIT
	bool isXNear = false;
	bool isYNear = false;

	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			ballObject* placedBall = &balls[i][j];//The ball in the array we are checking against
			float placedXCenter = placedBall->currentLocationX;//The X center location of that ball
			float placedYCenter = placedBall->currentLocationY;//" Y "
			float buffer = 2.0f; //2px buffer in the zone
			float diameter = CIRCLE_RADIUS * 2;//distance from center of one ball to center of the other when next to each other

			if (balls[i][j].exists )
			{
				float curX = ball->currentLocationX;
				float curY = ball->currentLocationY;

				float distance = sqrt(pow(placedXCenter - curX, 2) + pow(placedYCenter - curY, 2));

				bool shouldStop = distance <= ((CIRCLE_RADIUS * 2.0f) + 6.0f) && &balls[i][j] != ball;
				shouldStop = shouldStop | curY <= 0; //Stop at top of screen
				if (shouldStop)
				{
					isXNear = true;
					isYNear = true;
				}
				if (!(isXNear && isYNear))
				{
					isXNear = false;
					isYNear = false;
				}
			}
		}
	}

	return isXNear && isYNear;
}

//Adds the ball to the array, and sets it to existing
//Being in this array means final position for the ball
//==========================================================================================
//TODO: TEMP! This function sets the color of the ball, I need to set
//up a new function that all balls are initialized in to funnel them into one place before
//they have a set position
void Level1::AddBallToArray(ballObject &newBall)
{
	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			if (!balls[i][j].exists){
				balls[i][j] = newBall;

				return;
			}
				
		}
	}
}

void Level1::PrintBallArray()
{
	int numExistBalls = 0;
	std::cout << "--------------Ball Locations---------------\r\n";
	for (int i = 0; i < NUM_COLS; i++)
	{

		for (int j = 0; j < NUM_ROWS; j++)
		{

			if (balls[i][j].exists){
					
				/*std::cout << "\r\nX: ";
				std::cout << balls[i][j].currentLocationX;
				std::cout << "\r\nY: ";
				std::cout << balls[i][j].currentLocationY;
				std::cout << "\r\n";*/
				numExistBalls++;
			}
		}
	}

	std::cout << numExistBalls;
	std::cout << " balls exist in array";
}

//Draw the debug grid
void Level1::DrawGrid()
{
	float numVert = X_RES / GRID_SPACE_SIZE;
	for (int i = 0; i < numVert; i++)
	{
		gfx->DrawLine(GRID_SPACE_SIZE * (i + 1), GRID_SPACE_SIZE * (i + 1), 0, Y_RES, 0xF, 0x0, 0x0);
	}

	float numHoriz = Y_RES / GRID_SPACE_SIZE;
	for (int j = 0; j < numHoriz; j++)
	{
		gfx->DrawLine(0.0f, X_RES, GRID_SPACE_SIZE * (j + 1), GRID_SPACE_SIZE * (j + 1), 0xF, 0x0, 0x0);
	}
}

ColorTypes Level1::GetRandomColor()
{
	int randNum = rand() % ColorTypes::NUM_OPTIONS;
	return (ColorTypes)randNum;
}

void Level1::UpdatePopBalls(ballObject* firedBall, ballObject* lastBall)
{
	bool isNear = false;
	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			ballObject* placedBall = &balls[i][j];//The ball in the array we are checking against
			if (placedBall != lastBall && placedBall != firedBall)
			{
				float placedXCenter = placedBall->currentLocationX;//The X center location of that ball
				float placedYCenter = placedBall->currentLocationY;//" Y "
				float buffer = 2.0f; //2px buffer in the zone
				float diameter = CIRCLE_RADIUS * 2;//distance from center of one ball to center of the other when next to each other

				if (placedBall->exists)
				{
					float curX = firedBall->currentLocationX;
					float curY = firedBall->currentLocationY;

					float distance = sqrt(pow(placedXCenter - curX, 2) + pow(placedYCenter - curY, 2));

					bool shouldStop = distance <= ((CIRCLE_RADIUS * 2.0f) + 6.0f) && &balls[i][j] != firedBall;
					shouldStop = shouldStop | curY <= 0; //Stop at top of screen
					if (shouldStop)
					{
						isNear = true;
					}
					else
					{
						isNear = false;
					}

					//If same color and close, recursively check that one
					if (isNear && placedBall->color == firedBall->color)
					{
						isNoChain = false;
						UpdatePopBalls(&balls[i][j], firedBall);
					}
				}


			}
		}
	}
	if (!isNoChain)
	{
		score += 500;//TODO: Combo multi
		firedBall->exists = false;
	}
}

void Level1::SetCurrentMousePos(float x, float y)
{
	mouseXPos = x;
	mouseYPos = y;
}

Level1::~Level1()
{
	delete balls;
	delete bp;
	delete bpFire;
}
