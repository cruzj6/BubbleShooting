#include "GameLevel.h"
#include "Graphics.h"

enum ColorTypes
{
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	ORANGE = 3,
	YELLOW = 4,
	NUM_OPTIONS = 5
};

struct ballObject
{
	bool exists = 0; //Indicates that this ball should be rendered and accounted for in hit detection
	int yDestination = 0;//Where it will ultimately end up (if needed)
	int xDestination = 0;
	float currentLocationX = 0;
	float currentLocationY = 0;
	bool transitioningIn = 0;//if it is in motion and should not be accounted for in hit detection
	ColorTypes color = ColorTypes::BLUE;
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
	bool CheckBallShouldStop(ballObject* ball);
	void DrawGrid();
	void PrintBallArray();
	void RenderBallArray();
	void RenderFiringBall();
	void RenderUI();
	void RenderNextColorDisplay();
	void UpdateSpeedForTime(double timeTotal, double timeDelta);
	ColorTypes GetRandomColor();
	void GetColorRBG(ColorTypes colorNum, float* r, float* b, float* g);
	wchar_t* GetStringFromColorType(ColorTypes color);
	void UpdatePopBalls(ballObject* firedBall, ballObject* lastBall);
	void RenderTimeToNewBall();
};