#include "texture.h"
#include "gldebug.h"
#include "debug.h"
#include <SOIL/SOIL.h>

#define DEFAULT_TEXTURE_DIRECTORY "textures/"
std::vector<texture::Texture2D> texture::all2DTextures;
std::unordered_map<std::string, unsigned int> texture::textureLookup;
std::string texture::TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;

using namespace texture;

Texture2D::Texture2D( 
		unsigned int pWidth, unsigned int pHeight, 
		GLenum pInternalFormat, GLenum pFormat, GLenum pType, unsigned char* pData )
	:width( pWidth ), height( pHeight ), 
	internalFormat( pInternalFormat ), 
	format( pFormat ), type( pType ) 
{
	setup( pData );
}

void Texture2D::setup( unsigned char* pData ) 
{
	glCreateTextures( GL_TEXTURE_2D, 1, &ID );
	glBindTexture( GL_TEXTURE_2D, ID );

	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, 
			width, height, 0, format, type, pData );

	glBindTexture( GL_TEXTURE_2D, 0 );
}

void Texture2D::loadImage( Image image )
{
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

Texture2D::Texture2D( Image image, GLenum pType )
	:type( pType )
{
	loadImage( image );
	setup( image.pixels );
}

Texture2D::Texture2D( std::string pFilename )
{
	Image image;
	FILE* file = fopen( pFilename.c_str(), "wb" );
	image.read( file );
	loadImage( image );
	setup( image.pixels );
	fclose( file );
}

unsigned int texture::generateMipMap( Texture2D& texture, 
		int glMinFilter = GL_NEAREST_MIPMAP_LINEAR, 
		int glMagFilter = GL_NEAREST )
{
	glTextureParameteri( texture.ID, GL_TEXTURE_MIN_FILTER, glMinFilter );
	glTextureParameteri( texture.ID, GL_TEXTURE_MAG_FILTER, glMagFilter );
	glGenerateMipmap( GL_TEXTURE_2D );
}

void texture::setTextureWrapping( unsigned int pTextureIndex, unsigned int pWrapS, unsigned int pWrapT )
{
	glTextureParameteri( all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_S, pWrapS );
	glTextureParameteri( all2DTextures[pTextureIndex].ID, GL_TEXTURE_WRAP_T, pWrapT );
}

void texture::setTextureFilter( unsigned int pTextureIndex, unsigned int pMagFilter, unsigned int pMinFilter )
{
	glTextureParameteri( all2DTextures[pTextureIndex].ID, GL_TEXTURE_MIN_FILTER, pMinFilter );
	glTextureParameteri( all2DTextures[pTextureIndex].ID, GL_TEXTURE_MAG_FILTER, pMagFilter );
}

unsigned int texture::get2DTextureID( unsigned int pTextureIndex )
{
	return all2DTextures[pTextureIndex].ID;
}

void texture::setTextureDirectory( std::string& pDirectory )
{
	TEXTURE_DIR = pDirectory;
}

void texture::resetTextureDirectory()
{
	TEXTURE_DIR = DEFAULT_TEXTURE_DIRECTORY;
}

