#include "loader.h"
#include <cstring>

FT_Library ftLib;

void Loader::loadGlyph( glm::uvec4& quad, Font::Glyphs::Metric& met, FT_GlyphSlotRec* glyph )
{
	quad = glm::uvec4( 
			cursor.x + padding, cursor.y + padding, 
			glyph->bitmap.width, glyph->bitmap.rows );

	met = Font::Glyphs::Metric( 
			glyph->metrics.horiAdvance, 
			glyph->metrics.horiBearingX,
			glyph->metrics.horiBearingY );
}

void Loader::writeGlyphBitmap( glm::uvec4& quad, const FT_Bitmap& bitmap )
{
	for ( unsigned int row = 0; row < quad.w; ++row ) {
		glm::uvec2 cur = glm::uvec2( quad.x, quad.y + row );
		std::memcpy( 
				&font.atlas.pixels[ cur.y * font.atlas.width + cur.x ], 
				&bitmap.buffer[ row * bitmap.pitch ],
				bitmap.pitch );
	}
}

std::string extractFilename( std::string pFilepath )
{
	unsigned int sep_pos = pFilepath.find_last_of( '/' );
	unsigned int start = sep_pos == std::string::npos ? 0 : sep_pos + 1;
	return pFilepath.substr( start, pFilepath.substr( start ).find_first_of( '.' ) );
}

Font Loader::loadFont() 
{
	Font::Glyphs& glyphs = font.glyphs;
	Font::Atlas& atlas = font.atlas;
	glyphs.resize( face->num_glyphs );	

	unsigned int max_glyph_width = face->bbox.xMax - face->bbox.xMin;
	unsigned int row_length = std::ceil( std::sqrt( face->num_glyphs ) );
	unsigned int max_atlas_width = row_length * max_glyph_width;

	unsigned int row_max_height = 0;
	unsigned int max_row_width = 0;

	// precalculate glyph quads and store metrics
	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		FT_Load_Char( face, gi, FT_LOAD_RENDER );
		FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );

		glm::uvec4& quad = glyphs.quads[ gi ];
		Font::Glyphs::Metric& met = glyphs.metrics[ gi ];

		loadGlyph( quad, met, face->glyph );

		cursor.x += quad.z + padding * 2;
		row_max_height = std::max( quad.w + padding * 2, row_max_height );
		if ( ( gi + 1 ) % row_length == 0 ) {
			max_row_width = std::max( cursor.x, max_row_width );
			cursor.x = 0;
			cursor.y += row_max_height;
			row_max_height = 0;
		}
	}

	cursor.y += row_max_height;
	atlas.width = max_row_width;
	atlas.height = cursor.y;
	cursor = glm::uvec2( 0, 0 );
	atlas.pixels.resize( atlas.width * atlas.height );

	// now write glyph bitmaps to glyph quads in atlas
	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		FT_Load_Char( face, gi, FT_LOAD_RENDER );
		FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );

		writeGlyphBitmap( glyphs.quads[ gi ], face->glyph->bitmap );
	}

	font.name = extractFilename( filename );
	printf( "Loaded Font %s\n", font.name.c_str() );
	return font;
}


void Loader::setPadding( unsigned int padPixels )
{
	padding = padPixels;
}

void Loader::setSize( unsigned int ptx, unsigned int pty ) 
{
	FT_Set_Char_Size( face, ptx*64, pty*64, 1920, 1080 );
}

void Loader::setResolution( unsigned int ptx, unsigned int pty ) 
{
	resolution.x = ptx;
	resolution.y = pty;
}

Loader::~Loader()
{
	FT_Done_Face( face );
}

Loader::Loader( std::string pFilename )
	:filename( FONT_DIR + pFilename )
{
	int ft_error = FT_New_Face( ftLib, filename.c_str(), 0, &face );
	if ( ft_error ) {
		printf( "Could not load font %s\n", filename.c_str() ); 
		return;
	}
}

int initFreeType()
{
	if ( FT_Init_FreeType( &ftLib ) ) {
		puts( "Failed to init FreeType!" );
		return -1;
	}
	return 0;
}
