#pragma once
#include <vector>
#include <glm.hpp>
#include <string>
#include <cstring>
#include "image.h"

struct Font
{
	struct Glyphs
	{
		struct Metric
		{
			unsigned int advance;
			unsigned int bearx;
			unsigned int beary;
			unsigned int _pad;

			Metric()
			{
			}
			Metric( unsigned int adv, unsigned int bx, unsigned int by )
				:advance ( adv ), bearx( bx ), beary( by )
			{
			}
		};
		unsigned int count;
		std::vector<glm::uvec4> quads;
		std::vector<Metric> metrics;
		void resize( size_t size )
		{
			count = size;
			quads.resize( count );
			metrics.resize( count );
		}
		Glyphs()
			:count( 0 )
		{
		}
	};
	struct Atlas
	{
		unsigned int width;
		unsigned int height;
		std::vector<unsigned char> pixels;
		Atlas()
			:width( 0 ), height( 0 ) 
		{
		}

		Atlas& operator=( const Image& pImage )
		{
			this->width = pImage.width;
			this->height = pImage.height;
			this->pixels.resize( this->width * this->height );
			std::memcpy( &this->pixels[0], (unsigned char*)pImage.pixels, this->width * this->height );  
			return *this;
		}
	};
	Atlas atlas;
	Glyphs glyphs;
	std::string name;

	Font( std::string pFilename )
	{
		read( pFilename );
	}
	Font()
	{
	}

	std::string write();
	void read( std::string pFilename );
private:
	unsigned int writeGlyphs( FILE* file );
	unsigned int readGlyphs( FILE* file );
};
