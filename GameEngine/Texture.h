#include <D3DX10.h>
#include "DX10Util.h"

#ifndef TEXTURE_H
#define TEXTURE_H

enum texturetype{
	diffuse = 0,
	normal,
	specular
};

class Texture
{
public:
	Texture(void);
	Texture(std::string TexFile);
	Texture(std::vector<std::string> TexFiles);

	~Texture(void);

	bool loadTextureFromFile();

	ID3D10ShaderResourceView* getTextureView(unsigned int index);

private:
	std::vector<std::string> mTextureFiles;
	std::vector<ID3D10ShaderResourceView*> mTextures;
};

#endif