#include "GameLevel.h"
#include "Level1.h"

	const float SPAWN_RATE = 1000000.0f;
	const float CIRCLE_RADIUS = 20.0f;
	const float Y_SPAWN_SPEED = 10.0f;//TODO: change later
	const int NUM_ROWS = 10;
	const int NUM_COLS = 10;

	struct ballObject
	{
		int isMake;
		int yDestination;
		int xDestination;
	};

	//y Speed!
	float y = 600;
	float newBallY = 600;
	float spawnCountDown = SPAWN_RATE;
	bool renderNewBall = false;
	bool newBallCreated = false;
	float ballFireXDirection;
	float ballFireYDirection;
	bool fireingBall;
	ballObject* bp = { 0 }; //Used for creating new balls after the initial balls
	ballObject** balls;


	void UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject bp, bool &renderNewBall);
	void AddBallToArray(ballObject** ballArray, ballObject newBall);
	void RenderNewSpawnBall(Graphics* gfx, float ballXDest, float ballYDest, float frameYPos);

	//Sets initial values, initializes initial ball objects to be
	//rendered at the start of the level, and allocates memory
	//for possible ones to come

	void Level1::Load()
	{
		balls = new ballObject *[NUM_COLS];
		for (int i = 0; i < NUM_COLS; i++)
		{
			balls[i] = new ballObject[NUM_ROWS];
		}

		for (int j = 0; j < 5; j++)//For each col
		{
			for (int i = 0; i < 10; i++)//for each row
			{
				float randNum = rand() % 100;
				if (randNum > 50) {
					ballObject newBall;
					newBall.isMake = 1;
					newBall.yDestination = 40 * (j + 1);
					newBall.xDestination = 40 * (i + 1);
					balls[i][j] = newBall;
				}
				else {
					ballObject newBall;
					newBall.isMake = 0;
					balls[i][j] = newBall;
				}
			}
		}

	}

	//TODO: release resources
	void Level1::UnLoad()
	{
		delete bp;
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
			renderNewBall = true; newBallY = 600;
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

				AddBallToArray(balls, *bp);//Add it to the array so we can keep track of it after we loose the pointer
			}

			UpdateNewBall(newBallY, spawnCountDown, newBallCreated, *bp, renderNewBall);
		}
	}

	//TODO: Render the frame (Move non-render stuff to update())
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
						gfx->DrawCircle(ball->xDestination, ball->yDestination + y, circleRadius, 0.0, 0.0, 0xF, 1.0);
				}
			}
			gfx->EndDraw();
		}
		else if (renderNewBall)//if it is time to spawn one move a new one in
		{
			RenderNewSpawnBall(gfx, bp->xDestination, bp->yDestination, newBallY);
		}

	}

	void Level1::FireBall(float mouseX, float mouseY)
	{
		fireingBall = true;
		//TODO Get x and y diretion from vector
		//float xDirection = 
	}

	void UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject bp, bool &renderNewBall)
	{
		newBallY -= Y_SPAWN_SPEED;
		spawnCountDown = SPAWN_RATE;//Reset the countdown

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
		gfx->DrawCircle(ballXDest, ballYDest + frameYPos, 20, 0.0, 0.0, 0xF, 1.0);

		gfx->EndDraw();
	}

	void AddBallToArray(ballObject** ballArray, ballObject newBall)
	{
		for (int i = 0; i < NUM_COLS; i++)
		{
			for (int j = 0; j < NUM_ROWS; j++)
			{
				ballArray[i][j] = newBall;
			}
		}
	}
