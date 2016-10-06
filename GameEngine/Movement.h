//(c) copyright Martin Hollstein

#include <Windows.h>
#include <xnamath.h>

#ifndef MOVEMENT_H
#define MOVEMENT_H

class Movement
{
public:
	Movement();
	Movement(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale);
	~Movement(void);

	XMFLOAT3 getPosition();
	XMFLOAT3 getRotation();
	XMFLOAT3 getScale();
	XMFLOAT4X4 getWorld();

	const XMFLOAT3 getPosition() const;
	const XMFLOAT3 getRotation() const;
	const XMFLOAT3 getScale() const;
	const XMFLOAT4X4 getWorld() const;

	void setPosition(XMFLOAT3 pos);
	void setRotation(XMFLOAT3 rot);
	void setScale(XMFLOAT3 scale);

	void transformToWorld();

private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mRotation;
	XMFLOAT3 mScale;

	XMFLOAT4X4 mWorld;
};

#endif