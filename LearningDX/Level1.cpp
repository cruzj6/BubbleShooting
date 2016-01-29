#include "GameLevel.h"
#include "Level1.h"
#include <math.h>
#include <iostream>

const float SPAWN_TIME= 5.0f;//In Seconds
const float CIRCLE_RADIUS = 20.0f;
const float Y_SPAWN_RATE = 1000.0f;//For entry transition and random new balls
const int NUM_ROWS = 20;
const int NUM_COLS = 20;
const float FIRE_RATE = 1000.0f;
const float GRID_SPACE_SIZE = 50.0f;
const float X_RES = 800;
const float Y_RES = 600;
const float PERCENT_OF_SCREEN_INIT_BALLS = 0.3f;

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
	
//Vars for a fired ball
float ballFireXDirection;
float ballFireYDirection;
bool firingBall;
	
//Ball object pointers
ballObject* bp = { 0 }; //Used for creating new balls after the initial balls
ballObject** balls; //The array of balls in their final destination
ballObject* bpFire = { 0 }; //Pointer to a ball object that is being fired as it is being fired

//Sets initial values, initializes initial ball objects to be
//rendered at the start of the level, and allocates memory
//for possible ones to come
void Level1::Load()
{
	spawnCountDown = SPAWN_TIME;
	sprites = new SpriteSheet(L"test.png", gfx);

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
	for (int i = 0; i < NUM_COLS; i++)//For each col
	{
		for (int j = 0; j < NUM_ROWS; j++)//for each row
		{
			float randNum = rand() % 100;

			if (randNum > 50 && j < NUM_ROWS * PERCENT_OF_SCREEN_INIT_BALLS) {
				ballObject newBall;
				newBall.yDestination = (CIRCLE_RADIUS * 2) * (j + 1);
				newBall.xDestination = (CIRCLE_RADIUS * 2) * (i + 1);
				newBall.currentLocationX = newBall.xDestination;
				newBall.currentLocationY = newBall.yDestination;
				newBall.exists = true;
				balls[i][j] = newBall;
			}
			else {
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

//Update values every frame
void Level1::Update(double timeTotal, double timeDelta)
{
	UpdateSpeedForTime(timeTotal, timeDelta);
	spawnCountDown -= timeDelta;

	if (spawnCountDown <= 0)
	{
		spawnCountDown = SPAWN_TIME;
		ballObject* newBall = new ballObject();
		int max = (X_RES / NUM_COLS);
		float randomNum = (rand() % max);
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
			AddBallToArray(*bpFire);
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
					ballObject* ball = &balls[j][i];
					float circleRadius = CIRCLE_RADIUS;
					if (ball->exists) {
						sprites->Draw(ball->currentLocationX - CIRCLE_RADIUS, ball->currentLocationY + y - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
						gfx->DrawCircle(ball->currentLocationX, ball->currentLocationY + y, circleRadius, 0.0, 0.0, 0xF, 1.0);
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
		PrintBallArray();
		gfx->EndDraw();
}

void Level1::RenderFiringBall()
{
	if (firingBall)
	{
		sprites->Draw(bpFire->currentLocationX - CIRCLE_RADIUS, bpFire->currentLocationY - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
		gfx->DrawCircle(bpFire->currentLocationX, bpFire->currentLocationY, CIRCLE_RADIUS, 0.0, 0.0, 0xF, 1.0);
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

void GetColorRBG(ColorTypes colorNum, float* r, float* b, float* g)
{
	switch (colorNum)
	{
		case ColorTypes::BLUE:
			*r = 0x0;
			*g = 0x0;
			*b = 0xF;
			break;

		case ColorTypes::GREEN:
			*r = 0x0;
			*g = 0xF;
			*b = 0x0;
			break;
		case ColorTypes::ORANGE:
			*r = 0xF;
			*g = 0xF;
			*b = 0x0;
			break;
		case ColorTypes::RED:
			*r = 0xF;
			*g = 0x0;
			*b = 0x0;
			break;
		case ColorTypes::YELLOW:
			*r = 0x0;
			*g = 0x5;
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
	bpFire->currentLocationX = 400;
	bpFire->currentLocationY = 600;
}
Level1::~Level1()
{
	delete balls;
	delete bp;
	delete bpFire;
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

void Level1::RenderNewSpawnBall(float ballXDest, float ballYDest, float frameYPos)
{
	gfx->BeginDraw();

	float clearRadius = CIRCLE_RADIUS + 3.0f; //brush stroke is 3.0f on circles
	gfx->ClearZoneCircle(ballXDest, ballYDest + frameYPos + Y_SPAWN_SPEED, clearRadius);
	gfx->DrawCircle(ballXDest, ballYDest + frameYPos, CIRCLE_RADIUS, 0.0, 0.0, 0xF, 1.0);

	gfx->EndDraw();
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
	newBall.exists = true;
	newBall.color = GetRandomColor();
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
		gfx->DrawLine(GRID_SPACE_SIZE * (i + 1), GRID_SPACE_SIZE * (i + 1), 0, 600, 0xF, 0x0, 0x0);
	}

	float numHoriz = Y_RES / GRID_SPACE_SIZE;
	for (int j = 0; j < numHoriz; j++)
	{
		gfx->DrawLine(0.0f, 800, GRID_SPACE_SIZE * (j + 1), GRID_SPACE_SIZE * (j + 1), 0xF, 0x0, 0x0);
	}
}

ColorTypes Level1::GetRandomColor()
{
	float randNum = rand() % ColorTypes::NUM_OPTIONS;
	return (ColorTypes)randNum;
}
