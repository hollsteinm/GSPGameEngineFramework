//(c) copyright Martin Hollstein

#include <Windows.h>
#include <xnamath.h>

#ifndef PHYSICS_H
#define PHYSICS_H

#ifndef GRAVITY_ACCELERATION
#define GRAVITY_ACCELERATION -9.8f
#endif

class Physics
{
public:
	Physics(void);
	Physics(float mass, XMFLOAT3 velocity, XMFLOAT3 acceleration);
	~Physics(void);

	float getMass();
	XMFLOAT3 getVelocity();
	XMFLOAT3 getAcceleration();

	const float getMass() const;
	const XMFLOAT3 getVelocity() const;
	const XMFLOAT3 getAcceleration() const;

	void setMass(float mass);
	void setVelocity(XMFLOAT3 velocity);
	void setAcceleration(XMFLOAT3 acceleration);

private:
	float mMass;
	
	XMFLOAT3 mVelocity;
	XMFLOAT3 mAcceleration;
};

#endif