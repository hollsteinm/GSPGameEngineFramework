#include "DX10Util.h"

#ifndef LIGHT_H
#define LIGHT_H

namespace light{
	enum lighttype{
		POINT = 0,
		AMBIENTLIGHT, 
		DIRECTIONAL,
		SPOT
	};
}

class Light
{
public:
	Light(void);
	Light(light::lighttype type, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT3 attentuation, float power, float range);

	~Light(void);

	light::lighttype getType();

	XMFLOAT4 getAmbientColor();
	XMFLOAT4 getDiffuseColor();
	XMFLOAT4 getSpecularColor();
	XMFLOAT3 getAttentuation();

	void setAmbientColor(XMFLOAT4 color);
	void setDiffuseColor(XMFLOAT4 color);
	void setSpecularColor(XMFLOAT4 color);
	void setAttentuation(XMFLOAT3 attentuation);

	float getSpotPower();
	float getRange();

	void setSpotPower(float power);
	void setRange(float range);

	unsigned int getNumLights();

private:
	light::lighttype mLightType;
	
	XMFLOAT4 mAmbientColor;
	XMFLOAT4 mDiffuseColor;
	XMFLOAT4 mSpecularColor;
	XMFLOAT3 mAttentuation;
	float mSpotPower;
	float mRange;

	static unsigned int mNumLights;
};

#endif