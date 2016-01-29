#include "GameLevel.h"
#include "Graphics.h"

struct ballObject
{
	bool exists = 0; //Indicates that this ball should be rendered
	int yDestination = 0;
	int xDestination = 0;
	float currentLocationX = 0;
	float currentLocationY = 0;
	bool transitioningIn = 0;
	ColorTypes color;
};

enum ColorTypes
{
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	ORANGE = 3,
	YELLOW = 4,
	NUM_OPTIONS = 5
};

class Level1 : public GameLevel
{
public:
	Level1(Graphics* _gfx)
	{
		this->gfx = _gfx;
	}
	void Load() override;
	void UnLoad() override;
	void Render() override;
	void Update(double timeTotal, double timeDelta) override;
	void FireBall(float mouseX, float mouseY);
	~Level1();

private:
	void LoadInitialLevelBalls();
	void UpdateNewBall(ballObject* theBall);
	void AddBallToArray(ballObject &newBall);
	void RenderNewSpawnBall(float ballXDest, float ballYDest, float frameYPos);
	bool CheckBallShouldStop(ballObject* ball);
	void DrawGrid();
	void PrintBallArray();
	void RenderBallArray();
	void RenderFiringBall();
	void UpdateSpeedForTime(double timeTotal, double timeDelta);
	ColorTypes GetRandomColor();
};