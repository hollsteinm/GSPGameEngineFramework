//(c) copyright Martin Hollstein

#include "Movement.h"

Movement::Movement()
{	
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMMATRIX world = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, world);
}

Movement::Movement(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale)
{	
	mPosition = pos;
	mRotation = rot;
	mScale = scale;

	XMMATRIX world = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, world);
}

Movement::~Movement(void)
{
}

XMFLOAT3 Movement::getPosition()
{
	return mPosition;
}

XMFLOAT3 Movement::getRotation()
{
	return mRotation;
}

XMFLOAT3 Movement::getScale()
{
	return mScale;
}

XMFLOAT4X4 Movement::getWorld()
{
	return mWorld;
}

const XMFLOAT3 Movement::getPosition() const
{
	return mPosition;
}

const XMFLOAT3 Movement::getRotation() const
{
	return mRotation;
}

const XMFLOAT3 Movement::getScale() const
{
	return mScale;
}

const XMFLOAT4X4 Movement::getWorld() const
{
	return mWorld;
}

void Movement::setPosition(XMFLOAT3 pos)
{
	mPosition = pos;
}

void Movement::setRotation(XMFLOAT3 rot)
{
	mRotation = rot;
}

void Movement::setScale(XMFLOAT3 scale)
{
	mScale = scale;
}

void Movement::transformToWorld()
{
	XMMATRIX worldMatrix, scale, rotation, transform = XMMatrixIdentity();

	scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	rotation = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
	transform = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

	worldMatrix = rotation * scale * transform;

	XMStoreFloat4x4(&mWorld, worldMatrix);
}