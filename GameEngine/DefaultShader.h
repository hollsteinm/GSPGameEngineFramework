#include "Shader.h"

#ifndef DIFFUSESHADER_H
#define DIFFUSESHADER_H

class DefaultShader : public Shader
{
public:
	DefaultShader(void);
	~DefaultShader(void);

	bool Init(const HWND* hwnd);

	void setShaderLight(XMFLOAT3 lightDirection, XMFLOAT4 color);
	void setDiffuseTexture(ID3D10ShaderResourceView* pTexture);
	void setNormalTexture(ID3D10ShaderResourceView* pTexture);
	void setSpecularTexture(ID3D10ShaderResourceView* pTexture);
	void setSpecularColor(XMFLOAT4 color);
	void setCameraPosition(const XMFLOAT3 position);
	void setSpecularPower(float power);

private:
	ID3D10EffectVectorVariable* mLightDir;
	ID3D10EffectVectorVariable* mDiffuseColor; 
	ID3D10EffectVectorVariable* mSpecularColor;
	ID3D10EffectVectorVariable* mCameraPosition;
	ID3D10EffectScalarVariable* mSpecularPower;
	ID3D10EffectShaderResourceVariable* mDiffuseTextureResource;
	ID3D10EffectShaderResourceVariable* mNormalTextureResource;
	ID3D10EffectShaderResourceVariable* mSpecularTextureResource;
};

#endif