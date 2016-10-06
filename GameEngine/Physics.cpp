//(c) copyright Martin Hollstein

#include "Physics.h"

Physics::Physics(void)
{
	mMass = 0.0f;
	mVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Physics::Physics(float mass, XMFLOAT3 velocity, XMFLOAT3 acceleration)
{
	mMass = mass;
	mVelocity = velocity;
	mAcceleration = acceleration;
}

Physics::~Physics(void)
{
}
