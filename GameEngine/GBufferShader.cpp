#include "GBufferShader.h"


GBufferShader::GBufferShader(void)
{
}


GBufferShader::~GBufferShader(void)
{
}

void GBufferShader::CreateRenderTextureTargetResource()
{
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = CDXManager::instance()->getWidth();
	desc.Height = CDXManager::instance()->getHeight();
	
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	CDXManager::instance()->getDevice()->CreateTexture2D(&desc, NULL, &mRenderTarget);

	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	CDXManager::instance()->getDevice()->CreateRenderTargetView(mRenderTarget, &rtDesc, &mRenderTargetView);

	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	CDXManager::instance()->getDevice()->CreateShaderResourceView(mRenderTarget, &srDesc, &mShaderResourceView);
}

bool GBufferShader::Init(HWND* hwnd)
{
	//render texture
	CreateRenderTextureTargetResource();
	return true;
}
