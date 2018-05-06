#pragma once
#include "gl.h"
#include <unordered_map>
#include <vector>
#include "image.h"

namespace texture 
{
	struct Texture2D 
	{
		Texture2D()
		{
		}
		Texture2D( Image image, GLenum pType );
		Texture2D( std::string pFilename );
		Texture2D( unsigned int pWidth, unsigned int pHeight, 
				GLenum pInternalFormat, GLenum pFormat, GLenum pType, unsigned char* pData = nullptr );
		void loadImage( Image image );
		void setup( unsigned char* pData );
		unsigned int ID = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		GLenum internalFormat;
		GLenum format;
		GLenum type;
		operator unsigned int()
		{
			return ID;
		}
	};

	extern std::string TEXTURE_DIR;
	extern std::vector<Texture2D> all2DTextures;
	extern std::unordered_map<std::string, unsigned int> textureLookup;

    unsigned int generateMipMap( Texture2D& texture, 
            int glMinFilter, int glMagFilter );
	void setTextureWrapping( unsigned int pTextureIndex, unsigned int pWrapS, unsigned int pWrapT );
	void setTextureFilter( unsigned int pTextureIndex, unsigned int pMagFilter, unsigned int pMinFilter );
	unsigned int get2DTextureID( unsigned int pTextureIndex );

	void setTextureDirectory( std::string& pDirectory );
	void resetTextureDirectory();
}
