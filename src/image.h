#pragma once
#include <string>
#include <vector>

struct Image
{
  public:
	size_t width = 0;
	size_t height = 0;
	size_t channels = 1;
	size_t bit_depth = 8;
	unsigned char* pixels;

	Image()
	{}
	Image(std::string pFilename);
	Image(size_t pWidth, size_t pHeight,
		size_t pChannels, size_t pBitDepth, unsigned char* pData);

	~Image();
	void write(FILE* file);
	void write(std::string pFilename);
	void read(FILE* file);
	void read(std::string pFilename);
};
