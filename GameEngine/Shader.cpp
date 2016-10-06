#include "Shader.h"
#include "DXManager.h"

Shader::Shader(void)
{
	mFXFileName = L"../FX/DefaultShader.fx";
	mTechName	= "tech0";
	
	mEffect = NULL;
	mTechnique = NULL;
	mLayout = NULL;

	mWorldMatrix = NULL;
	mViewmatrix = NULL;
	mProjectionMatrix = NULL;
}

Shader::Shader(LPWSTR FXFileName, LPCSTR technique)
{
	mFXFileName = FXFileName;
	mTechName = technique;

	mEffect = NULL;
	mTechnique = NULL;
	mLayout = NULL;

	mWorldMatrix = NULL;
	mViewmatrix = NULL;
	mProjectionMatrix = NULL;
}

Shader::~Shader(void)
{
	if(mEffect)
		mEffect->Release();

	if(mLayout)
		mLayout->Release();
}

bool Shader::Init(HWND* hwnd)
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

	D3D10_INPUT_ELEMENT_DESC layout[3] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0}
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
	mWorldMatrix = mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewmatrix = mEffect->GetVariableByName("viewMatrix")->AsMatrix();
	mProjectionMatrix = mEffect->GetVariableByName("projectionMatrix")->AsMatrix();
	return true;
}

bool Shader::setTechnique(LPCSTR technique)
{
	mTechnique = 0;
	mTechnique = mEffect->GetTechniqueByName(mTechName);
	if(!mTechnique)
	{
		Log::Error("Could not get technique", "Shader", this);
		return false;
	}
	else
	{
		Log::Inform("Created technique", "Shader", this);
		return true;
	}
}

void Shader::setShaderMatrices(XMFLOAT4X4 pWorld, XMFLOAT4X4 pViewMatrix, XMFLOAT4X4 pProjectionMatrix)
{
	mWorldMatrix->SetMatrix((float*)&pWorld);
	mViewmatrix->SetMatrix((float*)&pViewMatrix);
	mProjectionMatrix->SetMatrix((float*)&pProjectionMatrix);
}

void Shader::RenderShader(unsigned int indexCount)
{
	D3D10_TECHNIQUE_DESC techDesc;
	unsigned int i;

	CDXManager::instance()->getDevice()->IASetInputLayout(mLayout);

	mTechnique->GetDesc(&techDesc);

	for(i = 0; i < techDesc.Passes; ++i)
	{
		mTechnique->GetPassByIndex(i)->Apply(0);
		CDXManager::instance()->getDevice()->DrawIndexed(indexCount, 0, 0);
	}
}