#include "Shader.h"

#ifndef TERRAIN_SHADER_H
#define TERRAIN_SHADER_H

class TerrainShader : public Shader
{
public:
	TerrainShader(void);
	~TerrainShader(void);

	bool Init(const HWND* hwnd);

	void setShaderLight(XMFLOAT3 lightDirection, XMFLOAT4 color);

	//Currently supports 3 blends
	void setBlendTexture0(ID3D10ShaderResourceView* pTexture);
	void setBlendTexture1(ID3D10ShaderResourceView* pTexture);
	void setBlendTexture2(ID3D10ShaderResourceView* pTexture);
	void setBlendTextureMaster(ID3D10ShaderResourceView* pTexture);

	//Reason I am not inheriting DefaultShader is that the terrain rendering may change in new iterations
	//void setNormalTexture(ID3D10ShaderResourceView* pTexture);
	//void setSpecularTexture(ID3D10ShaderResourceView* pTexture);
	//void setSpecularColor(XMFLOAT4 color);
	//void setSpecularPower(float power);

	void setCameraPosition(const XMFLOAT3 position);

private:
	ID3D10EffectVectorVariable* mLightDir;
	//ID3D10EffectVectorVariable* mDiffuseColor; 
	//ID3D10EffectVectorVariable* mSpecularColor;
	ID3D10EffectVectorVariable* mCameraPosition;
	//ID3D10EffectScalarVariable* mSpecularPower;
	//ID3D10EffectShaderResourceVariable* mDiffuseTextureResource;
	//ID3D10EffectShaderResourceVariable* mNormalTextureResource;
	//ID3D10EffectShaderResourceVariable* mSpecularTextureResource;
	ID3D10EffectShaderResourceVariable* mBlendTextureResource0;
	ID3D10EffectShaderResourceVariable* mBlendTextureResource1;
	ID3D10EffectShaderResourceVariable* mBlendTextureResource2;
	ID3D10EffectShaderResourceVariable* mBlendTextureMaster;

};

#endif

