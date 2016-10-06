#include "Material.h"

Material::Material(void)
{
	mDiffuseColor = WHITE;
	mSpecularColor = WHITE;
	mSpecularPower = 0.0f;
}

Material::Material(XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float specularPower)
{
	mDiffuseColor = diffuseColor;
	mSpecularColor = specularColor;
	mSpecularPower = specularPower;
}

Material::~Material(void)
{
}

XMFLOAT4 Material::getDiffuseColor()
{
	return mDiffuseColor;
}

XMFLOAT4 Material::getSpecularColor()
{
	return mSpecularColor;
}

float Material::getSpecularPower()
{
	return mSpecularPower;
}

const XMFLOAT4 Material::getDiffuseColor() const
{
	return mDiffuseColor;
}

const XMFLOAT4 Material::getSpecularColor() const
{
	return mSpecularColor;
}

const float Material::getSpecularPower() const
{
	return mSpecularPower;
}

void Material::setDiffuseColor(XMFLOAT4 color)
{
	mDiffuseColor = color;
}

void Material::setSpecularColor(XMFLOAT4 color)
{
	mSpecularColor = color;
}

void Material::setSpecularPower(float power)
{
	mSpecularPower = power;
}