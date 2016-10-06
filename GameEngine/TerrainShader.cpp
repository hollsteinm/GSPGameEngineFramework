#include "TerrainShader.h"
#include "DXManager.h"

TerrainShader::TerrainShader(void) : Shader(L"../FX/TerrainShader.fx", "tech0")
{
}

TerrainShader::~TerrainShader(void)
{
}

bool TerrainShader::Init(const HWND* hwnd)
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
	mWorldMatrix				= mEffect->GetVariableByName("gWorld")->AsMatrix();
	mViewmatrix					= mEffect->GetVariableByName("gView")->AsMatrix();
	mProjectionMatrix			= mEffect->GetVariableByName("gProjection")->AsMatrix();
	//mDiffuseTextureResource		= mEffect->GetVariableByName("diffuseTexture")->AsShaderResource();
	//mNormalTextureResource		= mEffect->GetVariableByName("normalTexture")->AsShaderResource();
	//mSpecularTextureResource	= mEffect->GetVariableByName("specularTexture")->AsShaderResource();
	//mDiffuseColor				= mEffect->GetVariableByName("diffuseColor")->AsVector();
	mLightDir					= mEffect->GetVariableByName("gLightDirection")->AsVector();
	mCameraPosition				= mEffect->GetVariableByName("gPosition")->AsVector();
	//mSpecularColor				= mEffect->GetVariableByName("specularColor")->AsVector();
	//mSpecularPower				= mEffect->GetVariableByName("specularPower")->AsScalar();
	mBlendTextureResource0		= mEffect->GetVariableByName("gBlend0")->AsShaderResource();
	mBlendTextureResource1		= mEffect->GetVariableByName("gBlend1")->AsShaderResource();
	mBlendTextureResource2		= mEffect->GetVariableByName("gBlend2")->AsShaderResource();
	mBlendTextureMaster			= mEffect->GetVariableByName("gBlendMaster")->AsShaderResource();

	return true;
}

void TerrainShader::setShaderLight(XMFLOAT3 lightDirection, XMFLOAT4 color)
{
	mLightDir->SetFloatVector((float*)&lightDirection);
	//mDiffuseColor->SetFloatVector((float*)&color);
}

void TerrainShader::setBlendTexture0(ID3D10ShaderResourceView* pTexture)
{
	mBlendTextureResource0->SetResource(pTexture);
}

void TerrainShader::setBlendTexture1(ID3D10ShaderResourceView* pTexture)
{
	mBlendTextureResource1->SetResource(pTexture);
}

void TerrainShader::setBlendTexture2(ID3D10ShaderResourceView* pTexture)
{
	mBlendTextureResource2->SetResource(pTexture);
}

void TerrainShader::setBlendTextureMaster(ID3D10ShaderResourceView* pTexture)
{
	mBlendTextureMaster->SetResource(pTexture);
}
/*
void TerrainShader::setNormalTexture(ID3D10ShaderResourceView* pTexture)
{
	mNormalTextureResource->SetResource(pTexture);
}

void TerrainShader::setSpecularTexture(ID3D10ShaderResourceView* pTexture)
{
	mSpecularTextureResource->SetResource(pTexture);
}

void TerrainShader::setSpecularColor(XMFLOAT4 color)
{
	mSpecularColor->SetFloatVector((float*)&color);
}

void TerrainShader::setSpecularPower(float power)
{
	mSpecularPower->SetFloat(power);
}
*/
void TerrainShader::setCameraPosition(const XMFLOAT3 position)
{
	mCameraPosition->SetFloatVector((float*)&position);
}