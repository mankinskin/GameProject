#pragma once
#include "gl.h"
#include <unordered_map>
#include <vector>

namespace texture {

	struct Texture2D {
		Texture2D(unsigned int pWidth, unsigned int pHeight, GLenum pInternalFormat, GLenum pFormat, GLenum pType);
		unsigned int ID = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		GLenum internalFormat;
		GLenum format;
		GLenum type;
	};

	struct TextureBuffer {
		TextureBuffer() :data(nullptr), width(0), height(0), channels(0) {}
		unsigned char* data;
		int width;
		int height;
		int channels;
	};

	struct TexData2D {
		std::vector<unsigned char> data;
		unsigned int width;
		unsigned int height;
	};
	extern std::vector<Texture2D> all2DTextures;

	void loadTextureBuffer(TextureBuffer& pBuffer, std::string pFilename, int pForceChannels = 0);
	unsigned int createTexture2D(unsigned int pWidth, unsigned int pHeight, 
            GLenum pInternalFormat, GLenum pFormat, 
            GLenum pType, const void* pData);
    unsigned int generateMipMap(Texture2D& texture, 
            int glMinFilter, 
            int glMagFilter);
	unsigned int createTexture2D(std::string pFilename);
	unsigned int createTexture2D(TextureBuffer pBuffer);
	void setTextureWrapping(unsigned int pTextureIndex, unsigned int pWrapS, unsigned int pWrapT);
	void setTextureFilter(unsigned int pTextureIndex, unsigned int pMagFilter, unsigned int pMinFilter);
	unsigned int get2DTextureID(unsigned int pTextureIndex);
	void setTextureDirectory(std::string& pDirectory);
	void resetTextureDirectory();
	extern std::string TEXTURE_DIR;
	extern std::unordered_map<std::string, unsigned int> textureLookup;
}
