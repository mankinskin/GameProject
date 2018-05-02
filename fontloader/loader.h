#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm.hpp>
#include <string>

#include "font.h"


class Loader
{
	public:
		Loader( std::string filename );
		~Loader();
		Font loadFont();

		void setSize( unsigned int ptx, unsigned int pty );
		void setResolution( unsigned int ptx, unsigned int pty );
		void setPadding( unsigned int padPixels );
	private:
		void writeGlyphBitmap( glm::uvec4& quad, const FT_Bitmap& bitmap );
		void loadGlyph( glm::uvec4& quad, Font::Glyphs::Metric& met, FT_GlyphSlotRec* glyph );
		const std::string FONT_DIR = "../fonts/";
		glm::uvec2 resolution = glm::uvec2( 1920, 1080 );
		unsigned int padding = 0;
		glm::uvec2 cursor;
		std::string filename = "";
		FT_Face face;
		Font font;
};

int initFreeType();
