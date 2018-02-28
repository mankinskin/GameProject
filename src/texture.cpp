#include "texture.h"
#include "gldebug.h"
#include "debug.h"
#include <SOIL.h>
#define DEFAULT_TEXTURE_DIRECTORY "assets//textures//"
std::vector<texture::Texture2D> texture::all2DTextures;
std::unordered_map<std::string, unsigned int> texture::textureLookup;
std::string texture::TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;



texture::Texture2D::Texture2D(size_t pWidth, size_t pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType)
	:width(pWidth), height(pHeight), internalFormat(pInternalFormat), format(pFormat), type(pType) {}


void texture::loadTextureBuffer(TextureBuffer & pBuffer, std::string pFilename, int pForceChannels)
{
	pBuffer.data = SOIL_load_image((TEXTURE_DIR + pFilename).c_str(), &pBuffer.width, &pBuffer.height, &pBuffer.channels, pForceChannels);

	if (!pBuffer.data) {
		debug::pushError("texture not found: " + (TEXTURE_DIR + pFilename) + " !!!");
	}
}

size_t texture::createTexture2D(size_t pWidth, size_t pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType, const void* pData)
{
	Texture2D texture(pWidth, pHeight, pInternalFormat, pFormat, pType);

	glGenTextures(1, &texture.ID);
	glBindTexture(GL_TEXTURE_2D, texture.ID);


	glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, texture.width, texture.height, 0, texture.format, texture.type, pData);
	glTextureParameteri(texture.ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTextureParameteri(texture.ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(texture.ID, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTextureParameteri(texture.ID, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	all2DTextures.push_back(texture);
	return all2DTextures.size() - 1;
}

size_t texture::createTexture2D(std::string pFilename)
{
	TextureBuffer buffer;
	loadTextureBuffer(buffer, pFilename, SOIL_LOAD_AUTO);
	return createTexture2D(buffer);
}
size_t texture::createTexture2D(TextureBuffer pBuffer)
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

void texture::setTextureWrapping(size_t pTextureIndex, size_t pWrapS, size_t pWrapT)
{
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_S, pWrapS);
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_T, pWrapT);
}

void texture::setTextureFilter(size_t pTextureIndex, size_t pMagFilter, size_t pMinFilter)
{
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_MIN_FILTER, pMinFilter);
	glTextureParameteri(all2DTextures[pTextureIndex].ID, GL_TEXTURE_MAG_FILTER, pMagFilter);
}

size_t texture::get2DTextureID(size_t pTextureIndex)
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
