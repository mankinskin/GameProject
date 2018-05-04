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

		Image atlas;
		Glyphs glyphs;

		Font( std::string pFilename );

		Font()
		{
		}

		std::string write();
		void read();

		void setLoadSize( unsigned int ptx, unsigned int pty );
		void setLoadResolution( unsigned int ptx, unsigned int pty );
		void setLoadPadding( unsigned int padPixels );
		private:
		void readFontfile();
		void readFace();
		void writeGlyphBitmap( glm::uvec4& quad, const FT_Bitmap& bitmap );
		void loadGlyph( glm::uvec4& quad, Font::Glyphs::Metric& met, FT_GlyphSlotRec* glyph );
		const std::string FONT_DIR = "fonts/";
		glm::uvec2 size = glm::uvec2( 4, 4 );
		glm::uvec2 resolution = glm::uvec2( 1920, 1080 );
		unsigned int padding = 0;
		std::string filepath = "";
		std::string name = "";
		std::string extension = "";
		glm::uvec2 cursor;
		int ft_error = 0;
		unsigned int writeGlyphs( FILE* file );
		unsigned int readGlyphs( FILE* file );
	};

	int initFreeType();
}

