#include "GameLevel.h"
#include "Level1.h"
#include <math.h>

#include <iostream>

	const float SPAWN_RATE = 1000000.0f;
	const float CIRCLE_RADIUS = 20.0f;
	const float Y_SPAWN_SPEED = 10.0f;//TODO: change later
	const int NUM_ROWS = 10;
	const int NUM_COLS = 10;
	const float FIRE_SPEED = 0.1f;
	const float GRID_SPACE_SIZE = 10.0f;

	struct ballObject
	{
		bool exists;
		int isMake;
		int yDestination;
		int xDestination;
		float currentLocationX;
		float currentLocationY;
	};

	//y Speed!
	float y = 600;
	float newBallY = 600;
	float spawnCountDown = SPAWN_RATE;
	bool renderNewBall = false;
	bool newBallCreated = false;
	float ballFireXDirection;
	float ballFireYDirection;
	bool firingBall;
	ballObject* bp = { 0 }; //Used for creating new balls after the initial balls
	ballObject** balls;
	ballObject* bpFire = { 0 };


	void UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject bp, bool &renderNewBall);
	void AddBallToArray(ballObject newBall);
	void RenderNewSpawnBall(Graphics* gfx, float ballXDest, float ballYDest, float frameYPos);
	bool CheckBallShouldStop();
	void DrawGrid(Graphics* gfx);
	void PrintBallArray();

	//Sets initial values, initializes initial ball objects to be
	//rendered at the start of the level, and allocates memory
	//for possible ones to come

	void Level1::Load()
	{
		balls = new ballObject *[NUM_ROWS];
		for (int i = 0; i < NUM_ROWS; i++)
		{
			balls[i] = new ballObject[NUM_COLS];
		}

		for (int i = 0; i < NUM_COLS; i++)//For each col
		{
			for (int j = 0; j < NUM_ROWS; j++)//for each row
			{
				float randNum = rand() % 100;
				if (randNum > 50) {
					ballObject newBall;
					newBall.isMake = 1;
					newBall.yDestination = 40 * (i + 1);
					newBall.xDestination = 40 * (j + 1);
					newBall.currentLocationX = newBall.xDestination;
					newBall.currentLocationY = newBall.yDestination;
					newBall.exists = true;
					balls[i][j] = newBall;
				}
				else {
					ballObject newBall;
					newBall.isMake = 0;
					newBall.exists = false;
					balls[i][j] = newBall;
				}
			}
		}

	}

	//TODO: release resources
	void Level1::UnLoad()
	{
		delete bp;
		delete bpFire;
		delete balls;
	}

	//TODO: Update values every frame
	void Level1::Update()
	{
		spawnCountDown -= 1;

		if (y > 0)
		{
			y -= Y_SPAWN_SPEED;
		}

		//If it is time to spawn a new ball, set the render flag to true
		if (spawnCountDown <= 0) {
			renderNewBall = true; 
			newBallY = 600;
		}

		if (renderNewBall){
			if (!newBallCreated){
				float diameter = 2 * CIRCLE_RADIUS;
				float randPos = rand() % (int)(800 / diameter);//20 different possible positions in 800px

				//Transition the ball in
				bp = new ballObject();
				bp->xDestination = 40 * randPos;
				bp->yDestination = 40 * 5; //TODO add to last row
				newBallCreated = true;

				AddBallToArray(*bp);//Add it to the array so we can keep track of it after we loose the pointer
			}

			UpdateNewBall(newBallY, spawnCountDown, newBallCreated, *bp, renderNewBall);
		}
		if (firingBall)
		{
			//Update the ball being fired
			bpFire->currentLocationX += ballFireXDirection * (FIRE_SPEED);
			bpFire->currentLocationY -= ballFireYDirection * (FIRE_SPEED);

			bool shouldStop = CheckBallShouldStop();
			//TODO Temp to stop making the ball
			if (bpFire->currentLocationX >= 800.0f || bpFire->currentLocationY <= 0.0f || shouldStop)
			{
				AddBallToArray(*bpFire);
				firingBall = false;
				delete bpFire;
			}
		}

		
	}

	void Level1::Render(Graphics* gfx)
	{
		//Initial Entry for the balls
		if (y > 0)
		{
			//If no message lets update
			gfx->BeginDraw();
			gfx->ClearScreen(0xF, 0xF, 0xF);

			for (int j = 0; j < 5; j++)//For each row
			{
				for (int i = 0; i < 10; i++)//for each col
				{
					ballObject* ball = &balls[i][j];
					float circleRadius = CIRCLE_RADIUS;
					if (ball->isMake != 0)
						gfx->DrawCircle(ball->currentLocationX, ball->currentLocationY + y, circleRadius, 0.0, 0.0, 0xF, 1.0);
				}
			}
			gfx->EndDraw();
		}
		else if (renderNewBall)//if it is time to spawn one move a new one in
		{
			RenderNewSpawnBall(gfx, bp->xDestination, bp->yDestination, newBallY);
		}

		if (firingBall)
		{
			gfx->BeginDraw();
			//Clear the previous
			gfx->ClearZoneCircle(bpFire->currentLocationX - (ballFireXDirection * FIRE_SPEED), bpFire->currentLocationY + (ballFireYDirection * FIRE_SPEED), (CIRCLE_RADIUS + 3.0f));
			//Draw the one for this frame
			gfx->DrawCircle(bpFire->currentLocationX, bpFire->currentLocationY, CIRCLE_RADIUS, 0.0, 0.0, 0xF, 1.0);
			gfx->EndDraw();
			PrintBallArray();
		}

		DrawGrid(gfx);

	}

	void Level1::FireBall(float mouseX, float mouseY)
	{
		firingBall = true;
		ballFireXDirection = (mouseX - 400);
		ballFireYDirection = (600 - mouseY);
		bpFire = new ballObject();
		bpFire->isMake = true;
		bpFire->currentLocationX = 400;
		bpFire->currentLocationY = 600;
	}
	Level1::~Level1()
	{
		delete balls;
		delete bp;
		delete bpFire;
	}

	void UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject bp, bool &renderNewBall)
	{
		bp.currentLocationY = newBallY - Y_SPAWN_SPEED;
		newBallY -= Y_SPAWN_SPEED;
		spawnCountDown = SPAWN_RATE;//Reset the countdown

		bp.currentLocationX = bp.xDestination;
		
		//If we are at our destination set the render flag to false
		if (newBallY <= 0) {
			renderNewBall = false;
			newBallCreated = false;
		}
	}

	void RenderNewSpawnBall(Graphics* gfx, float ballXDest, float ballYDest, float frameYPos)
	{
		gfx->BeginDraw();

		float clearRadius = CIRCLE_RADIUS + 3.0f; //brush stroke is 3.0f on circles
		gfx->ClearZoneCircle(ballXDest, ballYDest + frameYPos + Y_SPAWN_SPEED, clearRadius);
		gfx->DrawCircle(ballXDest, ballYDest + frameYPos, CIRCLE_RADIUS, 0.0, 0.0, 0xF, 1.0);

		gfx->EndDraw();
	}

	bool CheckBallShouldStop()
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

				if (balls[i][j].exists)
				{
					float curX = bpFire->currentLocationX;
					float curY = bpFire->currentLocationY;
					/*if ((curX <= stopZoneXRight) && (bpFire->currentLocationX >= stopZoneXLeft))
					{
						isXNear = true;
					}
					if ((curY <= stopZoneYBottom) && (curY >= stopZoneYTop))
					{
						isYNear = true;
					}*/
					float distance = sqrt(pow(placedXCenter - curX, 2) + pow(placedYCenter - curY, 2));
					if (distance <= (CIRCLE_RADIUS * 2) + 10)
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

	void AddBallToArray(ballObject newBall)
	{
		newBall.exists = true;
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

	void PrintBallArray()
	{
		std::cout << "--------------Ball Locations---------------\r\n";
		for (int i = 0; i < NUM_COLS; i++)
		{
			for (int j = 0; j < NUM_ROWS; j++)
			{

				if (balls[i][j].exists){
					
					std::cout << "\r\nX: ";
					std::cout << balls[i][j].currentLocationX;
					std::cout << "\r\nY: ";
					std::cout << balls[i][j].currentLocationY;
					std::cout << "\r\n";
				}
			}
		}
	}

	void DrawGrid(Graphics* gfx)
	{
		gfx->BeginDraw();
		float numVert = 800.0f / GRID_SPACE_SIZE;
		for (int i = 0; i < numVert; i++)
		{
			gfx->DrawLine(GRID_SPACE_SIZE * (i + 1), GRID_SPACE_SIZE * (i + 1), 0, 600, 0xF, 0x0, 0x0);
		}

		float numHoriz = 600.0f / GRID_SPACE_SIZE;
		for (int j = 0; j < numHoriz; j++)
		{
			gfx->DrawLine(0.0f, 800, GRID_SPACE_SIZE * (j + 1), GRID_SPACE_SIZE * (j + 1), 0xF, 0x0, 0x0);
		}
		gfx->EndDraw();
	}
