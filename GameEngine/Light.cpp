#include "Light.h"

using namespace light;

unsigned int Light::mNumLights = 0;

Light::Light(void)
{
	mLightType = DIRECTIONAL;
	mDiffuseColor = WHITE;
	mAmbientColor = WHITE;
	mSpecularColor = WHITE;

	mAttentuation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mSpotPower = 0.0f;
	mRange = 1.0f;

	mNumLights++;
}

Light::Light(lighttype type, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT3 attentuation, float power, float range)
{
	mLightType = type;
	mDiffuseColor = diffuse;
	mAmbientColor = ambient;
	mSpecularColor = specular;

	mAttentuation = attentuation;

	mSpotPower = power;
	mRange = range;

	mNumLights++;
}

Light::~Light(void)
{
	mNumLights--;
}

lighttype Light::getType()
{
	return mLightType;
}

unsigned int Light::getNumLights()
{
	return mNumLights;
}

XMFLOAT4 Light::getAmbientColor()
{
	return mAmbientColor;
}

XMFLOAT4 Light::getDiffuseColor()
{
	return mDiffuseColor;
}

XMFLOAT4 Light::getSpecularColor()
{
	return mSpecularColor;
}

XMFLOAT3 Light::getAttentuation()
{
	return mAttentuation;
}

void Light::setAmbientColor(XMFLOAT4 color)
{
	mAmbientColor = color;
}

void Light::setDiffuseColor(XMFLOAT4 color)
{
	mDiffuseColor = color;
}

void Light::setSpecularColor(XMFLOAT4 color)
{
	mSpecularColor = color;
}

void Light::setAttentuation(XMFLOAT3 attentuation)
{
	mAttentuation = attentuation;
}

float Light::getSpotPower()
{
	return mSpotPower;
}

float Light::getRange()
{
	return mRange;
}

void Light::setSpotPower(float power)
{
	mSpotPower = power;
}

void Light::setRange(float range)
{
	mRange = range;
}