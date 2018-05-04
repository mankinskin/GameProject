#pragma once
#include <string>
#include <png.h>

struct Image
{
	std::string filename = "";
	FILE* file;
	unsigned int file_offset = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	png_bytep pixels;
	png_byte color_type;
	png_byte bit_depth;

	Image()
	{
	}
	Image( std::string pFilename, unsigned int pOffset = 0 )
		:filename( pFilename ), file_offset( pOffset )
	{
	}

	Image( FILE* pFile, unsigned int pOffset = 0 )
		:file( pFile ), file_offset( pOffset )
	{
	}
	~Image();


	void write( unsigned char* pPixels, unsigned int pWidth, unsigned int pHeight, 
			png_byte pType = PNG_COLOR_TYPE_GRAY, png_byte pBitDepth = 8 );
	void read();
};
