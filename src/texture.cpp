#include "texture.h"
#include "gldebug.h"
#include "debug.h"
#include <SOIL.h>
#define DEFAULT_TEXTURE_DIRECTORY "assets//textures//"
std::vector<texture::Texture2D> texture::all2DTextures;
std::unordered_map<std::string, unsigned int> texture::textureLookup;
std::string texture::TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;



texture::Texture2D::Texture2D(unsigned int pWidth, unsigned int pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType)
	:width(pWidth), height(pHeight), internalFormat(pInternalFormat), format(pFormat), type(pType) {}


void texture::loadTextureBuffer(TextureBuffer & pBuffer, std::string pFilename, int pForceChannels)
{
	pBuffer.data = SOIL_load_image((TEXTURE_DIR + pFilename).c_str(), &pBuffer.width, &pBuffer.height, &pBuffer.channels, pForceChannels);

	if (!pBuffer.data) {
		debug::pushError("texture not found: " + (TEXTURE_DIR + pFilename) + " !!!");
	}
}

unsigned int texture::createTexture2D(unsigned int pWidth, unsigned int pHeight, 
        GLenum pInternalFormat, GLenum pFormat, GLenum pType, const void* pData)
{
	Texture2D texture(pWidth, pHeight, pInternalFormat, pFormat, pType);
	glBindTexture(GL_TEXTURE_2D, texture.ID);
	glCreateTextures(GL_TEXTURE_2D, 1, &texture.ID);


	glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, texture.width, texture.height, 0, texture.format, texture.type, pData);
	glTextureParameteri(texture.ID, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTextureParameteri(texture.ID, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	all2DTextures.push_back(texture);
	return all2DTextures.size() - 1;
}

unsigned int texture::generateMipMap(Texture2D& texture, 
        int glMinFilter = GL_NEAREST_MIPMAP_LINEAR, 
        int glMagFilter = GL_NEAREST)
{
	glTextureParameteri(texture.ID, GL_TEXTURE_MIN_FILTER, glMinFilter);
	glTextureParameteri(texture.ID, GL_TEXTURE_MAG_FILTER, glMagFilter);
	glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned int texture::createTexture2D(std::string pFilename)
{
	TextureBuffer buffer;
	loadTextureBuffer(buffer, pFilename, SOIL_LOAD_AUTO);
	return createTexture2D(buffer);
}
unsigned int texture::createTexture2D(TextureBuffer pBuffer)
{
	int internal_format = 0;
	int format = 0;
	switch (pBuffer.channels) {
	case 4:
		internal_format = GL_RGBA8;
		format = GL_RGBA;
		break;
	case 3:
		internal_format = GL_RGB8;
		format = GL_RGB;
		break;
	case 1:
		internal_format = GL_R8;
		format = GL_RED;
		break;
	case 2:
		internal_format = GL_RG8;
		format = GL_RG;
		break;
	default:
		internal_format = GL_RGBA8;
		format = GL_RGBA;
	}
	return createTexture2D(pBuffer.width, pBuffer.height, internal_format, format, GL_UNSIGNED_BYTE, pBuffer.data);
}

void texture::setTextureWrapping(unsigned int pTextureIndex, unsigned int pWrapS, unsigned int pWrapT)
{
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_S, pWrapS);
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_T, pWrapT);
}

void texture::setTextureFilter(unsigned int pTextureIndex, unsigned int pMagFilter, unsigned int pMinFilter)
{
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_MIN_FILTER, pMinFilter);
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_MAG_FILTER, pMagFilter);
}

unsigned int texture::get2DTextureID(unsigned int pTextureIndex)
{
	return all2DTextures[pTextureIndex].ID;
}

void texture::setTextureDirectory(std::string& pDirectory)
{
	TEXTURE_DIR = pDirectory;
}

void texture::resetTextureDirectory()
{
	TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;
}
