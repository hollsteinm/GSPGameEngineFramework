//(c) copyright Martin Hollstein

#include "Camera.h"

const XMFLOAT3 Camera::mDefaultUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
const XMFLOAT3 Camera::mDefaultView = XMFLOAT3(0.0f, 0.0f, 1.0f);

Camera::Camera()
{
	mView = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mUp = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mHeading = 0.0f;
	mPitch = 0.0f;

	mForward = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mStrafe = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMMATRIX identity = XMMatrixIdentity();
	XMStoreFloat4x4(&mProjectionMatrix, identity);
	XMStoreFloat4x4(&mViewMatrix, identity);
	XMStoreFloat4x4(&mRotationMatrix, identity);
}

Camera::Camera(XMFLOAT3 view, XMFLOAT3 up)
{
	mView = view;
	mUp = up;

	mHeading = 0.0f;
	mPitch = 0.0f;

	mForward = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mStrafe = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMMATRIX identity = XMMatrixIdentity();
	XMStoreFloat4x4(&mProjectionMatrix, identity);
	XMStoreFloat4x4(&mViewMatrix, identity);
	XMStoreFloat4x4(&mRotationMatrix, identity);
}

Camera::~Camera(void)
{
}

void Camera::SetLens(float FoV, float aspectRatio, float nZ, float fZ)
{
	XMMATRIX projection = XMMatrixPerspectiveFovLH(FoV, aspectRatio, nZ, fZ);
	XMStoreFloat4x4(&mProjectionMatrix, projection);
}

void Camera::setPitch(float pitch)
{
	mPitch = pitch;
}

void Camera::setHeading(float heading)
{
	mHeading = heading;
}

void Camera::BuildView(XMFLOAT3* position)
{
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(mPitch, mHeading, 0.0f);
	
	XMVECTOR defaultViewVec = XMLoadFloat3(&mDefaultView);
	XMVECTOR defaultUpVec = XMLoadFloat3(&mDefaultUp);
	XMVECTOR eyeVec = XMLoadFloat3(position);

	XMVECTOR viewVec = XMVector3TransformCoord(defaultViewVec, rotationMat);
	XMVECTOR upVec = XMVector3TransformCoord(defaultUpVec, rotationMat);

	XMVECTOR forwardVec = XMVector3Normalize(viewVec);
	XMVECTOR strafeVec = XMVector3Cross(upVec, viewVec);
	strafeVec = XMVector3Normalize(strafeVec);

	viewVec = eyeVec + viewVec;

	XMMATRIX viewMat = XMMatrixLookAtLH(eyeVec, viewVec, upVec);

	XMStoreFloat3(&mForward, forwardVec);
	XMStoreFloat3(&mStrafe, strafeVec);
	XMStoreFloat4x4(&mViewMatrix, viewMat);
}

void Camera::Update(XMFLOAT3* position)
{
	BuildView(position);	
}

XMFLOAT4X4 Camera::getProjection()
{
	return mProjectionMatrix;
}

XMFLOAT4X4 Camera::getView()
{
	return mViewMatrix;
}

XMFLOAT3 Camera::getForward()
{
	return mForward;
}

XMFLOAT3 Camera::getStrafe()
{
	return mStrafe;
}

float Camera::getHeading()
{
	return mHeading;
}

float Camera::getPitch()
{
	return mPitch;
}

const XMFLOAT4X4 Camera::getProjection() const
{
	return mProjectionMatrix;
}

const XMFLOAT4X4 Camera::getView() const
{
	return mViewMatrix;
}

const XMFLOAT3 Camera::getForward() const
{
	return mForward;
}

const XMFLOAT3 Camera::getStrafe() const
{
	return mStrafe;
}

const float Camera::getHeading() const
{
	return mHeading;
}

const float Camera::getPitch() const
{
	return mPitch;
}