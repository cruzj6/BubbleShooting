#include "GameLevel.h"

class Level1 : public GameLevel
{
public:
	void Load() override;
	void UnLoad() override;
	void Render(Graphics* gfx) override;
	void Update() override;
};