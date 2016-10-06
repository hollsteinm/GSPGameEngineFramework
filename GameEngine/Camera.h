//(c) copyright Martin Hollstein

#include <Windows.h>
#include <xnamath.h>
#include "DX10Util.h"

#ifndef CCAMERA_H
#define CCAMERA_H

class Camera
{
public:
	Camera();
	Camera(XMFLOAT3 view, XMFLOAT3 up);
	~Camera(void);

	//name: SetLens()
	//purpose: sets up view projection matrix and initializes camera
	//return: none
	void SetLens(float FoV, float aspectRatio, float nZ, float fZ);

	//name: setPitch()
	//purpose: sets member variable pitch
	//return: none
	void setPitch(float pitch);

	//name: setHeading()
	//purpose: sets member variable heading
	//return: none
	void setHeading(float heading);

	//name: BuildView()
	//purpose: builds the camera view matrix
	//return: none
	void BuildView(XMFLOAT3* position);

	//test
	void Update(XMFLOAT3* position);

	XMFLOAT4X4 getView();
	XMFLOAT4X4 getProjection();
	XMFLOAT3 getForward();
	XMFLOAT3 getStrafe();
	float getHeading();
	float getPitch();

	const XMFLOAT4X4 getView() const;
	const XMFLOAT4X4 getProjection() const;
	const XMFLOAT3 getForward() const;
	const XMFLOAT3 getStrafe() const;
	const float getHeading() const;
	const float getPitch() const;

private:
	float mHeading;
	float mPitch;

	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjectionMatrix;
	XMFLOAT4X4 mRotationMatrix;

	static const XMFLOAT3 mDefaultView;
	static const XMFLOAT3 mDefaultUp;

	XMFLOAT3 mForward;
	XMFLOAT3 mStrafe;

	XMFLOAT3 mView;
	XMFLOAT3 mUp;

};

#endif