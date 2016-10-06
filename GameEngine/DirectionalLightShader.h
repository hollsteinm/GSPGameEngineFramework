#include "shader.h"

#ifndef DIRECTIONALIGHTSHADER_H
#define DIRECTIONALIGHTSHADER_H

class DirectionalLightShader :
	public Shader
{
public:
	DirectionalLightShader(void);
	~DirectionalLightShader(void);

	void setDirection(XMFLOAT3 direction);
	void setColor(XMFLOAT4 color);
	void setCameraPosition(XMFLOAT3 position);

	void setInvertViewProjection(XMFLOAT4X4 view, XMFLOAT4X4 projection);
	
	void setColorMap();
	void setNormalMap();
	void setDepthMap();

private:
	ID3D10EffectVariable* mDirection;
	ID3D10EffectVariable* mColor;
	ID3D10EffectVariable* mCameraPosition;

	ID3D10EffectMatrixVariable* mInvertViewProjection;

	ID3D10EffectShaderResourceVariable* mColorMap;
	ID3D10EffectShaderResourceVariable* mNormalMap;
	ID3D10EffectShaderResourceVariable* mDepthMap;
};

#endif

