#include "Texture.h"
#include <assert.h>
#include "DXManager.h"

Texture::Texture(void)
{
	mTextureFiles.push_back("../Textures/stonewall.dds");
	mTextureFiles.push_back("../Textures/stonewallnormal.dds");
	mTextureFiles.push_back("../Textures/stonewallspecular.png");
}

Texture::Texture(std::string TexFile)
{
	mTextureFiles.push_back(TexFile);
}

Texture::Texture(std::vector<std::string> TexFiles)
{
	for(unsigned int i = 0; i < TexFiles.size(); ++i)
	{
		mTextureFiles.push_back(TexFiles[i]);
	}
}

Texture::~Texture(void)
{
	for(unsigned int i = 0; i < mTextures.size(); ++i)
	{
		if(mTextures[i])
		{
			mTextures[i]->Release();
		}
	}
}

bool Texture::loadTextureFromFile()
{
	mTextures.resize(mTextureFiles.size());
	for(unsigned int i = 0; i < mTextureFiles.size(); ++i)
	{
		if( FAILED(D3DX10CreateShaderResourceViewFromFileA(CDXManager::instance()->getDevice(), mTextureFiles[i].c_str(), NULL, NULL, &mTextures[i], NULL)))
		{
			std::string err;
			printf_s(err.c_str(), "Could not Create Texture: %s!", mTextureFiles[i].c_str());
			Log::Error(err, "Texture", this);
			return false;
		}
	}
	return true;
}

ID3D10ShaderResourceView* Texture::getTextureView(unsigned int index)
{
	assert(index >= 0);
	assert(index < mTextures.size());
	return mTextures[index];
}