#include "shader.h"
#include "DXManager.h"

#ifndef GBUFFER_H
#define GBUFFER_H

class GBufferShader : public Shader
{
public:
	GBufferShader(void);
	~GBufferShader(void);

	bool Init(HWND* hwnd);

private:
	void CreateRenderTextureTargetResource();

	ID3D10Texture2D* mRenderTarget;
	ID3D10RenderTargetView* mRenderTargetView;
	ID3D10ShaderResourceView* mShaderResourceView;

	ID3D10EffectShaderResourceVariable* mShaderResourceVariable;

	//variables for the shader file
	ID3D10EffectScalarVariable*	mSpecularIntensity;
	ID3D10EffectScalarVariable*	mSpecularPower;
	
	ID3D10EffectShaderResourceVariable* mGBufferTexture;
	ID3D10EffectShaderResourceVariable* mSpecularMap;
	ID3D10EffectShaderResourceVariable* mNormalMap;

};

#endif