#include "Collision.h"
#include "Camera.h"
#include "Controller.h"
#include "GameObject.h"
#include "DXManager.h"
#include "Movement.h"

#ifndef FPSPLAYER_H
#define FPSPLAYER_H

//clamps in degrees
#define MAXPITCH XMConvertToRadians(90.0f)
#define MINPITCH XMConvertToRadians(-90.0f)

class FPSPlayer : public AABBCollider, public Camera, public Controller, public GameObject, public Movement
{
public:
	FPSPlayer(void);
	FPSPlayer(
		AABBCollider aabbcollider, 
		Camera camera, 
		Movement movement);

	~FPSPlayer(void);

	void Load();
	void Update(float dt);
	void Render();
	void Release();

private:
	//private methods
	void controlMouselookWASD(float dt);

	//for movement
	float mForwardVelocity;
	float mStrafeVelocity;
	float eyeHeight;

};

#endif

