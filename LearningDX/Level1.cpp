#include "GameLevel.h"
#include "Level1.h"
#include <math.h>
#include <cmath>
#include <sstream>
#include <iostream>

/**
*Created by Joseph Cruz
*
*This class will be refactored into 2 classes, one for rendering,
*one for updating the model...
*
*/

//Constants
const float SPAWN_TIME= 5.0f;//In Seconds
const int NUM_ROWS = 20;
const int NUM_COLS = 17;
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
float X_RES = 800;//Keep these Res 4:3
float Y_RES = 600;//----TODO: Changed these to get render target size-----
float CIRCLE_RADIUS = (X_RES / NUM_COLS) / 2;
//Test it
//This needs to account for different native res than render tar res when using mouse coords
//Maybe window at start to request Res selection 4:3

//Entry Transition vars
float y = Y_RES;
float newBallY = Y_RES;
bool isEntry = true;

//Random ball entry
float spawnCountDown;
float score = 0;
	
//Vars for a fired ball
double ballFireXDirection;
double ballFireYDirection;
bool firingBall;
bool isNoChain = true; //Used for the UpdatePopBalls() Recursive method
bool isNoMultiChain = true;
int colorCounter = 0;
	
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
	redImg = new SpriteSheet(L"lipstick.png",gfx);
	UIArrow = new SpriteSheet(L"Aim_Arrow.png", gfx);
	greenImg = new SpriteSheet(L"avacado.png", gfx);
	orangeImg = new SpriteSheet(L"squirrel.png", gfx);
	blueImg = new SpriteSheet(L"seagull.png", gfx);
	UIImage = new SpriteSheet(L"UIBar.png", gfx);
	backGround = new SpriteSheet(L"unicornbkg.jpg", gfx);

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
	nextShootColor = GetNextColor();

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
				newBall.xDestination = (CIRCLE_RADIUS * 2) * (i + 0.5f);
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
	delete redImg;
	delete backGround;
	delete orangeImg;
	delete UIArrow;
	delete greenImg;
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
	//Update X and Y resolutions dynamically with render target resolution
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
		spawnCountDown = SPAWN_TIME;

		newBall->currentLocationX = randomNum * (NUM_COLS + .5);
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

		if (bpFire->currentLocationX <= 0 && ballFireXDirection < 0)
		{
			//If we hit a wall bounce
			ballFireXDirection = 0 - ballFireXDirection;
		}
		if (bpFire->currentLocationX >= X_RES && ballFireXDirection > 0)
		{
			ballFireXDirection = 0 - ballFireXDirection;
		}

		//Stop making the ball if it is in a good spot
		if (bpFire->currentLocationY <= 0.0f || shouldStop)
		{
			firingBall = false;
			bpFire->exists = true;

			//Pop balls if we need to and check if they won
			//TODO: do something if they win
			UpdatePopBalls(bpFire, NULL);
			bool isWinner = CheckIfWinner();

			isNoChain = true;
			isNoMultiChain = true;

			if (bpFire->exists)
			{
				AddBallToArray(*bpFire);
			}
			delete bpFire;
		}
		else
		{
			//TODO: refactor direction into ballObject, can also be used for random
			//spawn then

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

		gfx->ClearScreen(0xF, 0xF, 0xF);
		backGround->Draw(0, 0, X_RES, Y_RES);
		if (isEntry){//Render our entry transition if this is the entry time
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
						gfx->DrawCircle(ball->currentLocationX, ball->currentLocationY + y, circleRadius, r, g, b, 1.0);
						DrawImageColor(ball->color, ball->currentLocationX - CIRCLE_RADIUS, ball->currentLocationY + y - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
					}
				}
			}
		}
		else
		{//Render things for playing the level
			RenderBallArray();
			RenderFiringBall();
		}
		//DrawGrid();
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
		gfx->DrawCircle(bpFire->currentLocationX, bpFire->currentLocationY, CIRCLE_RADIUS, r, g, b, 1.0);
		DrawImageColor(bpFire->color, bpFire->currentLocationX - CIRCLE_RADIUS,
			bpFire->currentLocationY - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
	}
}

void Level1::RenderWinner()
{
	//TODO
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
	//Get the middle of the screen coords and length of arrow
	float xVecStart = X_RES / 2;
	float yVecStart = Y_RES;
	float vectorLen = sqrt(pow(xVecStart - mouseXPos, 2) + pow(Y_RES - mouseYPos, 2));

	//Get the direction based on mouse position (normalized vector)
	float xNormDirec = (xVecStart - mouseXPos) / vectorLen;
	float yNormDirec = (yVecStart - mouseYPos) / vectorLen;
	float arrowLen = Y_RES * (0.5);

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
	gfx->WriteText(nonConstStr, X_RES - 150, Y_RES - Y_RES/6, X_RES -10, Y_RES);
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
	gfx->WriteText(nonConstStr, (X_RES/9) * 2, Y_RES - Y_RES/5, ((X_RES/9) * 2) + 100, Y_RES);
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
	gfx->WriteText(L"Fire-Color ", xlocation - (xlen), ylocation - (ylen * 2.5), xlocation + 100, ylocation + 50);
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

	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			if (balls[i][j].exists)
			{
				//Get the rgb based on color enum assignment
				GetColorRBG(balls[i][j].color, &r, &b, &g);

				//Draw the ball and the image associated with that color
				gfx->DrawCircle(balls[i][j].currentLocationX, balls[i][j].currentLocationY, CIRCLE_RADIUS, r, g, b, 1.0);
				DrawImageColor(balls[i][j].color, balls[i][j].currentLocationX - CIRCLE_RADIUS,
					balls[i][j].currentLocationY - CIRCLE_RADIUS, CIRCLE_RADIUS * 2 , CIRCLE_RADIUS * 2);
			}
		}
	}
}

void Level1::DrawImageColor(ColorTypes color,float x,float y, float width, float height)
{
	switch (color)
	{
		case ColorTypes::RED:
			redImg->Draw(x, y, width, height);
			break;

		case ColorTypes::BLUE:
			blueImg->Draw(x, y, width, height);
			break;

		case ColorTypes::ORANGE:
			orangeImg->Draw(x, y, width, height);
			break;

		case ColorTypes::YELLOW:

		case ColorTypes::GREEN:
			greenImg->Draw(x, y, width, height);
			break;

		default:
			return;
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
			*g = 0x1;
			*b = 0x0;
			break;
		case ColorTypes::ORANGE:
			*r = 1;
			*g = 0.4;
			*b = 0;
			break;
		case ColorTypes::RED:
			*r = 0x1;
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
	//Only occur if one if not currently in progress
	if (!firingBall)
	{
		float xCenter = X_RES / 2;
		float vectorLen = sqrt(pow(xCenter - mouseX, 2) + pow(Y_RES - mouseY, 2));

		firingBall = true;
		ballFireXDirection = (mouseX - xCenter) / vectorLen;
		ballFireYDirection = (Y_RES - mouseY) / vectorLen;

		//Create new ball object
		bpFire = new ballObject();
		bpFire->currentLocationX = X_RES / 2;
		bpFire->currentLocationY = Y_RES;

		//Copy the memory because were about to change the value at that address
		memcpy(&bpFire->color, &nextShootColor, sizeof(ColorTypes));

		//Get the next color
		nextShootColor = GetNextColor();
	}
}

//TODO: refactor this, we can do this elsewhere
void Level1::UpdateNewBall(ballObject* theBall)
{
	theBall->currentLocationY -= Y_SPAWN_SPEED;
	if (CheckBallShouldStop(theBall))
	{
		//It is done moving in now
		theBall->transitioningIn = false;
	}
}	

//Checks if ball is colliding with another ball (in ball array, that exists = true)
bool Level1::CheckBallShouldStop(ballObject* ball)
{
	bool isNear = false;

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

				bool shouldStop = distance <= ((CIRCLE_RADIUS * 2.0f)) && &balls[i][j] != ball;
				shouldStop = shouldStop || curY <= 0; //Stop at top of screen

				if (shouldStop)
				{
					isNear = true;
					//Make sure we are not rendering colliding balls
					if (distance < (CIRCLE_RADIUS * 2) + 1.0f)
					{
						//Get how far off ideal resting place we are
						float distDiff = ((CIRCLE_RADIUS * 2) + 1.0f) - distance;
						float distX = placedXCenter - curX;
						float distY = placedYCenter - curY;

						//While we need to adjust distance loop through
						for (int i = 1; i <= distDiff; i++)
						{
							//Even, adjust x
							if (i % 2 == 0)
							{
								if (distX < 0)
								{
									ball->currentLocationX += 1.0f;
								}
								else
									ball->currentLocationX -= 1.0f;
							}
							else //Odd adjust Y
							{
								if (distY < 0)
								{
									ball->currentLocationY += 1.0f;
								}
								else
									ball->currentLocationX -= 1.0f;
							}
						}
					}
				}
				if (!(isNear))
				{
					isNear = false;
				}
			}
		}
	}

	return isNear;
}

//Adds the ball to the array of collision detection (existing in world)
//Being in this array GENERALLY means final position for the ball
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

//For debug, uncomment code in main for showing console to see default in and out stream displayed
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

ColorTypes Level1::GetNextColor()
{
	if (colorCounter == ColorTypes::NUM_OPTIONS)
	{
		colorCounter = 0;
	}
	ColorTypes curColor = (ColorTypes)colorCounter;
	colorCounter++;

	return curColor;
}

ColorTypes Level1::GetRandomColor()
{
	int randNum = rand() % ColorTypes::NUM_OPTIONS;
	return (ColorTypes)randNum;
}

//Pop the balls that should be popped this frame
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

					bool shouldStop = distance <= ((CIRCLE_RADIUS * 2.0f) + 15.0f) && &balls[i][j] != firedBall;
					shouldStop = shouldStop || curY <= 0; //Stop at top of screen
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
						if (isNoChain == false)
						{
							isNoMultiChain = false;
						}
						isNoChain = false;
						UpdatePopBalls(&balls[i][j], firedBall);
					}
				}


			}
		}
	}
	if (!isNoMultiChain)
	{
		score += 500;//TODO: Combo multi
		firedBall->exists = false;
	}
}

//Returns true if there are no more balls to pop
bool Level1::CheckIfWinner()
{
	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			if (balls[i][j].exists == true)
			{
				return false;
			}
		}
	}

	return true;
}

void Level1::SetCurrentMousePos(float x, float y)
{
	mouseXPos = x;
	mouseYPos = y;
}

Level1::~Level1()
{
	delete bp;
	delete bpFire;
	delete balls;
	delete redImg;
	delete backGround;
	delete orangeImg;
	delete blueImg;
	delete UIArrow;
	delete greenImg;
}
