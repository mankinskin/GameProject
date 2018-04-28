#include "loader.h"
FT_Library ftLib;

const std::string FONT_DIR = "../fonts/";

int loader::FontMemory::loadFile( std::string filename ) 
{
	FT_Face ftface;
	filename = FONT_DIR + filename;
	int ft_error = FT_New_Face( ftLib, filename.c_str(), 0, &ftface );
	if ( ft_error ) {
		printf( "Could not load font %s\n", filename.c_str() ); 
		return ft_error;
	}

	FT_Set_Pixel_Sizes( ftface, 32, 0 );

	loadGlyphs( ftface );


	FT_Done_Face( ftface );
	return 0;
}

void loader::FontMemory::loadGlyphs( FT_Face& face )
{
	glyphs.resize( face->num_glyphs );	
	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		
		FT_Load_Char( face, gi, FT_LOAD_RENDER );
		FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );

		glyphs.storeGlyph( gi, face->glyph );
		
		for ( unsigned int r = 0; r < face->glyph->bitmap.rows; ++r ) {
			fwrite( face->glyph->bitmap.buffer, sizeof(char), face->glyph->bitmap.width, stdout );
			fputs("\n", stdout );
		}
	}
}

void loader::FontMemory::Glyphs::storeGlyph( size_t index, const FT_GlyphSlotRec* pGlyph )
{
	quads[ index ] = glm::uvec4( 
			pGlyph->bitmap_left, pGlyph->bitmap_top, 
			pGlyph->bitmap.width, pGlyph->bitmap.rows );
	metrics[ index ] = Metric( 
			pGlyph->metrics.horiAdvance, 
			pGlyph->metrics.horiBearingX,
			pGlyph->metrics.horiBearingY );
}

void loader::FontMemory::loadAtlas( FT_Face& face )
{
	
}

void loader::FontMemory::print()
{
	printf("Atlas %ux%u \n%u Glyphs:\n", atlas.width, atlas.height, glyphs.count );
}

int loader::initFreeType()
{
	if ( FT_Init_FreeType( &ftLib ) ) {
		puts( "Failed to init FreeType!" );
		return -1;
	}
	return 0;
}
