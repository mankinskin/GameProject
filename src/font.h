#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm.hpp>
#include <vector>
#include <string>
#include <cstring>
#include "image.h"

namespace text 
{
	struct Font
	{
		struct Glyphs
		{
			struct Metric
			{
				unsigned int advance;
				unsigned int bearingx;
				unsigned int bearingy;

				Metric()
				{
				}
				Metric( unsigned int adv, unsigned int bx, unsigned int by )
					:advance ( adv ), bearingx( bx ), bearingy( by )
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

		Image atlas;
		Glyphs glyphs;

		Font()
		{
		}

		std::string write();
		void read( std::string pFilename );

		void setLoadSize( unsigned int ptx, unsigned int pty );
		void setLoadResolution( unsigned int ptx, unsigned int pty );
		void setLoadPadding( unsigned int padPixels );
		private:
		void readFontfile( std::string pFilepath );
		void readFace( std::string pFilepath );
		void writeGlyphBitmap( const unsigned int x, const unsigned int y, const FT_Bitmap& bitmap );
		const std::string FONT_DIR = "fonts/";
		glm::uvec2 size = glm::uvec2( 4, 4 );
		glm::uvec2 resolution = glm::uvec2( 1920, 1080 );
		unsigned int padding = 0;
		std::string name = "";
		int ft_error = 0;
		unsigned int writeGlyphs( FILE* file );
		unsigned int readGlyphs( FILE* file );
	};

	int initFreeType();
}

