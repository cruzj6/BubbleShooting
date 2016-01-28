#include "GameLevel.h"
#include "Level1.h"
#include <math.h>
#include <iostream>


	const float SPAWN_RATE = 1000000.0f;
	const float CIRCLE_RADIUS = 20.0f;
	const float Y_SPAWN_SPEED = 10.0f;//For entry transition and random new balls
	const int NUM_ROWS = 8;
	const int NUM_COLS = 20;
	const float FIRE_SPEED = 20.0f;
	const float GRID_SPACE_SIZE = 50.0f;
	const float X_RES = 800;
	const float Y_RES = 600;

	//Entry Transition vars
	float y = Y_RES;
	float newBallY = Y_RES;
	bool isEntry = true;

	//Vars for random new balls
	float spawnCountDown = SPAWN_RATE;
	bool renderNewBall = false;
	bool newBallCreated = false;
	
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
				if (randNum > 50) {
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
	void Level1::Update()
	{
		if (y > 0)
		{
			y -= Y_SPAWN_SPEED;
		}
		else
		{
			isEntry = false;
		}

		spawnCountDown -= 1;
		//If it is time to spawn a new ball, set the render flag to true
		if (spawnCountDown <= 0) {
			renderNewBall = true; 
			newBallY = Y_RES;
		}

		if (renderNewBall){
			if (!newBallCreated){
				float diameter = 2 * CIRCLE_RADIUS;
				float randPos = rand() % (int)(800 / diameter);//20 different possible positions in 800px

				//Transition the ball in
				bp = new ballObject();
				bp->xDestination = (CIRCLE_RADIUS * 2) * randPos;
				bp->yDestination = (CIRCLE_RADIUS * 2) * 5; //TODO add to last row
				newBallCreated = true;

				AddBallToArray(*bp);//Add it to the array so we can keep track of it after we loose the pointer
			}

			UpdateNewBall(newBallY, spawnCountDown, newBallCreated, *bp, renderNewBall);
		}
		if (firingBall)
		{
			bool shouldStop = CheckBallShouldStop();
			//TODO Temp to stop making the ball
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
						if (ball->exists)
							gfx->DrawCircle(ball->currentLocationX, ball->currentLocationY + y, circleRadius, 0.0, 0.0, 0xF, 1.0);
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
		gfx->ClearScreen(0xF, 0xF, 0xF);
		for (int i = 0; i < NUM_COLS; i++)
		{
			for (int j = 0; j < NUM_ROWS; j++)
			{
				if (balls[i][j].exists)
				{
					sprites->Draw(balls[i][j].currentLocationX - CIRCLE_RADIUS, balls[i][j].currentLocationY - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
					gfx->DrawCircle(balls[i][j].currentLocationX, balls[i][j].currentLocationY, CIRCLE_RADIUS, 0.0, 0.0, 0xF, 1.0);
				}
			}
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

	void Level1::UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject bp, bool &renderNewBall)
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

	void Level1::RenderNewSpawnBall(float ballXDest, float ballYDest, float frameYPos)
	{
		gfx->BeginDraw();

		float clearRadius = CIRCLE_RADIUS + 3.0f; //brush stroke is 3.0f on circles
		gfx->ClearZoneCircle(ballXDest, ballYDest + frameYPos + Y_SPAWN_SPEED, clearRadius);
		gfx->DrawCircle(ballXDest, ballYDest + frameYPos, CIRCLE_RADIUS, 0.0, 0.0, 0xF, 1.0);

		gfx->EndDraw();
	}

	bool Level1::CheckBallShouldStop()
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

					float distance = sqrt(pow(placedXCenter - curX, 2) + pow(placedYCenter - curY, 2));
					if (distance <= ((CIRCLE_RADIUS * 2.0f) + 6.0f))
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
	void Level1::AddBallToArray(ballObject &newBall)
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

	void Level1::PrintBallArray()
	{
		int numExistBalls = 0;
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
