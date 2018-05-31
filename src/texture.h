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
		Texture2D( const Image& image );
		Texture2D( std::string pFilename );
		Texture2D( unsigned int pWidth, unsigned int pHeight, 
				GLenum pInternalFormat, GLenum pFormat, unsigned char* pData = nullptr );
		void loadImage( const Image& image );
		void setup( unsigned char* pData );
		unsigned int ID = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		GLenum internalFormat;
		GLenum format;
		operator unsigned int()
		{
			return ID;
		}
	};

	extern std::string TEXTURE_DIR;

    void generateMipMap( Texture2D& texture, 
            int glMinFilter = GL_NEAREST_MIPMAP_LINEAR, 
			int glMagFilter = GL_NEAREST );
	void setTextureWrapping( Texture2D& pTexture, unsigned int pWrapS, unsigned int pWrapT );
	void setTextureFilter( Texture2D& pTexture, unsigned int pMagFilter, unsigned int pMinFilter );

	void setTextureDirectory( std::string& pDirectory );
	void resetTextureDirectory();
}
