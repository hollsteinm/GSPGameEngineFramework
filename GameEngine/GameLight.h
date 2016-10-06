#include "GameObject.h"
#include "Light.h"
#include "Movement.h"

class GameLight : public Light, public GameObject, public Movement
{
public:
	GameLight(void);
	GameLight(Light light, Movement movement);

	~GameLight(void);

private:
	bool mIsOn;
};

