#pragma once
#include <string>
#include <vector>

struct Image
{
	public:
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int channels = 1;
	unsigned int bit_depth = 8;
	unsigned char* pixels;

	Image()
	{
	}
	Image( std::string pFilename );
	Image( unsigned int pWidth, unsigned int pHeight, 
			unsigned int pChannels, unsigned int pBitDepth, unsigned char* pData );

	~Image();

	void write( FILE* file );
	void write( std::string pFilename );
	void read( FILE* file );
	void read( std::string pFilename );
};
