#include "FPSPlayer.h"


FPSPlayer::FPSPlayer(void) : Camera(XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f))
{
}

FPSPlayer::FPSPlayer(
	AABBCollider aabbcollider, 
	Camera camera,  
	Movement movement) 
	: AABBCollider(aabbcollider),
	Camera(camera),
	Movement(movement)
{
	eyeHeight = 2.0f;
}

FPSPlayer::~FPSPlayer(void)
{
}

void FPSPlayer::Load()
{
	//camera
	setPitch(0.0f);
	setHeading(0.0f);
	SetLens(XM_PI / 4.0f, (float)CDXManager::instance()->getWidth() / CDXManager::instance()->getHeight(), 0.1f, 2000.0f);

	//movement
	
	setPosition(XMFLOAT3(0.0f, eyeHeight, 0.0f));
	BuildView(&getPosition());

	mForwardVelocity = 50.0f;
	mStrafeVelocity = 35.0f;
}

void FPSPlayer::Update(float dt)
{
	controlMouselookWASD(dt);
	transformToWorld();
	BuildView(&getPosition());
}

void FPSPlayer::Render()
{
}

void FPSPlayer::Release()
{
}

void FPSPlayer::controlMouselookWASD(float dt)
{
	//load all required resources.
	XMFLOAT3 dir(0.0f, 0.0f, 0.0f);
	XMVECTOR direction = XMLoadFloat3(&dir);
	XMVECTOR forward = XMLoadFloat3(&getForward());
	XMVECTOR strafe = XMLoadFloat3(&getStrafe());
	XMVECTOR position = XMLoadFloat3(&getPosition());

	//sprinting
	//TODO: remove magic numbers
	if(keyDown(DIK_LSHIFT))
	{
		mForwardVelocity = 100.0f;
		mStrafeVelocity = 70.0f;
	}
	else
	{
		mForwardVelocity = 50.0f;
		mStrafeVelocity = 35.0f;
	}

	//mouse look
	XMFLOAT2 newPitchHeading(
		getHeading() + mouseDX() * dt,
		getPitch() + mouseDY() * dt
		);

	setHeading(newPitchHeading.x);
	//x rotation is equal to x coordinate mouse movements
	setRotation(XMFLOAT3(
		newPitchHeading.x,
		getRotation().y,
		getRotation().z
		));

	if(newPitchHeading.y > MINPITCH && 
		newPitchHeading.y < MAXPITCH)
	{
		setPitch(newPitchHeading.y);
	}

	//keyboard movement
	if(keyDown(DIK_W))
		direction += forward;

	if(keyDown(DIK_S))
		direction -= forward;

	if(keyDown(DIK_D))
		direction += strafe;

	if(keyDown(DIK_A))
		direction -= strafe;

	direction = XMVector3Normalize(direction);
	position += direction * mForwardVelocity * dt;

	//set new position
	XMFLOAT3 tempPos(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&tempPos, position);
	setPosition(tempPos);
}
