#pragma once
#include <string>
#include <vector>

struct Image
{
	std::string filename = "";
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int channels = 1;
	unsigned int bit_depth = 8;
	unsigned char* pixels;

	Image()
	{
	}
	Image( std::string pFilename )
		:filename( pFilename )
	{
	}

	~Image();

	void write( FILE* file, unsigned char* pPixels, unsigned int pWidth, unsigned int pHeight, 
			unsigned int pChannels = 1, unsigned char pBitDepth = 8 );
	void read( FILE* file );
};
