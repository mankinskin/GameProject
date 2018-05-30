#include "texture.h"
#include "gldebug.h"
#include "debug.h"
#include <SOIL/SOIL.h>

#define DEFAULT_TEXTURE_DIRECTORY "textures/"
std::string texture::TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;

using namespace texture;

void Texture2D::setup( unsigned char* pData ) 
{
	glCreateTextures( GL_TEXTURE_2D, 1, &ID );
	glBindTexture( GL_TEXTURE_2D, ID );

	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, 
			width, height, 0, format, GL_UNSIGNED_BYTE, pData );

	glBindTexture( GL_TEXTURE_2D, 0 );
}

void Texture2D::loadImage( const Image& image )
{
	if ( image.width == 0 || image.height == 0 ) {
		puts ( "Texture2D: invalid Image!" );
		return;
	}
	width = image.width;
	height = image.height;

	switch ( image.channels ) {
		case 4:
			if ( image.bit_depth == 16 ) {
				internalFormat = GL_RGBA16;
			}
			else {
				internalFormat = GL_RGBA8;
			}
			format = GL_RGBA;
			break;
		case 3:
			if ( image.bit_depth == 16 ) {
				internalFormat = GL_RGB16;
			}
			else {
				internalFormat = GL_RGB8;
			}
			format = GL_RGB;
			break;
		case 1:
			if ( image.bit_depth == 16 ) {
				internalFormat = GL_R16;
			}
			else {
				internalFormat = GL_R8;
			}
			format = GL_RED;
			break;
		case 2:
			if ( image.bit_depth == 16 ) {
				internalFormat = GL_RG16;
			}
			else {
				internalFormat = GL_RG8;
			}
			format = GL_RG;
			break;
		default:
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
	}
}

Texture2D::Texture2D( 
		unsigned int pWidth, unsigned int pHeight, 
		GLenum pInternalFormat, GLenum pFormat, unsigned char* pData )
	:width( pWidth ), height( pHeight ), 
	internalFormat( pInternalFormat ), 
	format( pFormat ) 
{
	setup( pData );
}

Texture2D::Texture2D( const Image& image )
{
	loadImage( image );
	setup( image.pixels );
}

Texture2D::Texture2D( std::string pFilename )
{
	Image image;
	FILE* file = fopen( pFilename.c_str(), "rb" );
	image.read( file );
	loadImage( image );
	setup( image.pixels );
	fclose( file );
}

void texture::generateMipMap( Texture2D& texture, 
		int glMinFilter, int glMagFilter )
{
	glGenerateTextureMipmap( texture.ID );
}

void texture::setTextureWrapping( Texture2D& pTexture, unsigned int pWrapS, unsigned int pWrapT )
{
	glTextureParameteri( pTexture.ID, GL_TEXTURE_WRAP_S, pWrapS );
	glTextureParameteri( pTexture.ID, GL_TEXTURE_WRAP_T, pWrapT );
}

void texture::setTextureFilter( Texture2D& pTexture, unsigned int pMagFilter, unsigned int pMinFilter )
{
	glTextureParameteri( pTexture.ID, GL_TEXTURE_MIN_FILTER, pMinFilter );
	glTextureParameteri( pTexture.ID, GL_TEXTURE_MAG_FILTER, pMagFilter );
}

void texture::setTextureDirectory( std::string& pDirectory )
{
	TEXTURE_DIR = pDirectory;
}

void texture::resetTextureDirectory()
{
	TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;
}

