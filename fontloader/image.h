#pragma once
#include <string>
#include <png.h>

struct Image
{
	std::string filename;
	unsigned int width = 0;
	unsigned int height = 0;
	png_bytep pixels;
	png_byte color_type;
	png_byte bit_depth;

	Image( std::string pFilename )
		:filename( pFilename )
	{
	}

	void write( unsigned char* pPixels, unsigned int pWidth, unsigned int pHeight, 
			png_byte pType = PNG_COLOR_TYPE_GRAY, png_byte pBitDepth = 8 );
	void read();
};
