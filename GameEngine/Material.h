#include "DX10Util.h"

#ifndef MATERIAL_H
#define MATERIAL_H

class Material
{
public:
	Material(void);
	Material(XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float specularPower);

	~Material(void);

	XMFLOAT4 getDiffuseColor();
	XMFLOAT4 getSpecularColor();
	float getSpecularPower();

	const XMFLOAT4 getDiffuseColor() const;
	const XMFLOAT4 getSpecularColor() const;
	const float getSpecularPower() const;

	void setDiffuseColor(XMFLOAT4 color);
	void setSpecularColor(XMFLOAT4 color);
	void setSpecularPower(float power);

private:
	XMFLOAT4 mDiffuseColor;
	XMFLOAT4 mSpecularColor;
	float mSpecularPower;
};

#endif

