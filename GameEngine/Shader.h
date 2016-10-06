#include "DX10Util.h"

#ifndef CSHADER_H
#define CSHADER_H

class Shader
{
public:
	Shader(void);
	Shader(LPWSTR FXFileName, LPCSTR technique);
	virtual ~Shader(void);
	
	virtual bool Init(HWND* hwnd);

	bool setTechnique(LPCSTR technique);

	void RenderShader(unsigned int indexCount);

	void setShaderMatrices(XMFLOAT4X4 pWorld, XMFLOAT4X4 pViewMatrix, XMFLOAT4X4 pProjectionMatrix);

protected:
	LPCWSTR mFXFileName;
	LPCSTR  mTechName;

	ID3D10Effect* mEffect;
	ID3D10EffectTechnique* mTechnique;
	ID3D10InputLayout* mLayout;

	ID3D10EffectMatrixVariable* mWorldMatrix;
	ID3D10EffectMatrixVariable* mViewmatrix;
	ID3D10EffectMatrixVariable* mProjectionMatrix;	
};

#endif