#include "GameLevel.h"
#include "Graphics.h"

struct ballObject
{
	bool exists; //Indicates that this ball should be rendered
	int yDestination;
	int xDestination;
	float currentLocationX;
	float currentLocationY;
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
	void Update() override;
	void FireBall(float mouseX, float mouseY);
	~Level1();

private:
	void LoadInitialLevelBalls();
	void UpdateNewBall(float &newBallY, float &spawnCountDown, bool &newBallCreated, ballObject bp, bool &renderNewBall);
	void AddBallToArray(ballObject &newBall);
	void RenderNewSpawnBall(float ballXDest, float ballYDest, float frameYPos);
	bool CheckBallShouldStop();
	void DrawGrid();
	void PrintBallArray();
	void RenderBallArray();
	void RenderFiringBall();
};