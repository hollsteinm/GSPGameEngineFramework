#include "DefaultShader.h"
#include "DXManager.h"

DefaultShader::DefaultShader() : Shader()
{
}

DefaultShader::~DefaultShader()
{
}

bool DefaultShader::Init(const HWND* hwnd)
{
	unsigned int numElems;

	if(FAILED(D3DX10CreateEffectFromFile(mFXFileName,
		NULL,
		NULL,
		"fx_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		CDXManager::instance()->getDevice(),
		NULL,
		NULL,
		&mEffect,
		NULL,
		NULL)))
	{
		Log::Error("Could not create effect", "Shader", this);
		return false;
	}

	if(!setTechnique(mTechName))
		return false;

	D3D10_PASS_DESC passDescription;

	D3D10_INPUT_ELEMENT_DESC layout[5] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	numElems = sizeof(layout) / sizeof(layout[0]);
	mTechnique->GetPassByIndex(0)->GetDesc(&passDescription);

	if( FAILED ( CDXManager::instance()->getDevice()->CreateInputLayout( layout, numElems, passDescription.pIAInputSignature, passDescription.IAInputSignatureSize, &mLayout) ) )
	{
		Log::Error("Vertex Element Description failed", "Shader", NULL);
		return false;
	}
	else
	{
		Log::Inform("Vertex Element Descriptions defined", "Shader", NULL);
	}

	//set external variable pointers
	mWorldMatrix				= mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewmatrix					= mEffect->GetVariableByName("viewMatrix")->AsMatrix();
	mProjectionMatrix			= mEffect->GetVariableByName("projectionMatrix")->AsMatrix();
	mDiffuseTextureResource		= mEffect->GetVariableByName("diffuseTexture")->AsShaderResource();
	mNormalTextureResource		= mEffect->GetVariableByName("normalTexture")->AsShaderResource();
	mSpecularTextureResource	= mEffect->GetVariableByName("specularTexture")->AsShaderResource();
	mDiffuseColor				= mEffect->GetVariableByName("diffuseColor")->AsVector();
	mLightDir					= mEffect->GetVariableByName("lightDirection")->AsVector();
	mCameraPosition				= mEffect->GetVariableByName("cameraPosition")->AsVector();
	mSpecularColor				= mEffect->GetVariableByName("specularColor")->AsVector();
	mSpecularPower				= mEffect->GetVariableByName("specularPower")->AsScalar();

	return true;
}

void DefaultShader::setShaderLight(XMFLOAT3 LightDirection, XMFLOAT4 Color)
{
	mLightDir->SetFloatVector((float*)&LightDirection);

	mDiffuseColor->SetFloatVector((float*)&Color);
}

void DefaultShader::setDiffuseTexture(ID3D10ShaderResourceView* pTexture)
{
	mDiffuseTextureResource->SetResource(pTexture);
}

void DefaultShader::setNormalTexture(ID3D10ShaderResourceView* pTexture)
{
	mNormalTextureResource->SetResource(pTexture);
}

void DefaultShader::setSpecularTexture(ID3D10ShaderResourceView* pTexture)
{
	mSpecularTextureResource->SetResource(pTexture);
}

void DefaultShader::setCameraPosition(XMFLOAT3 position)
{
	mCameraPosition->SetFloatVector((float*)&position);
}

void DefaultShader::setSpecularColor(XMFLOAT4 color)
{
	mSpecularColor->SetFloatVector((float*)&color);
}

void DefaultShader::setSpecularPower(float power)
{
	mSpecularPower->SetFloat(power);
}