#include "font.h"
#include <unistd.h>
#include <fcntl.h>

FT_Library ftLib;

using namespace text;

void Font::loadGlyph( glm::uvec4& quad, Font::Glyphs::Metric& met, FT_GlyphSlotRec* glyph )
{
	quad = glm::uvec4( 
			cursor.x + padding, cursor.y + padding, 
			glyph->bitmap.width, glyph->bitmap.rows );

	met = Font::Glyphs::Metric( 
			glyph->metrics.horiAdvance, 
			glyph->metrics.horiBearingX,
			glyph->metrics.horiBearingY );
}

void Font::writeGlyphBitmap( glm::uvec4& quad, const FT_Bitmap& bitmap )
{
	for ( unsigned int row = 0; row < quad.w; ++row ) {
		glm::uvec2 cur = glm::uvec2( quad.x, quad.y + row );
		std::memcpy( 
				&atlas.pixels[ cur.y * atlas.width + cur.x ], 
				&bitmap.buffer[ row * bitmap.pitch ],
				bitmap.pitch );
	}
}

void Font::setLoadPadding( unsigned int padPixels )
{
	padding = padPixels;
}

void Font::setLoadSize( unsigned int ptx, unsigned int pty ) 
{
	size.x = ptx;
	size.y = pty;
}

void Font::setLoadResolution( unsigned int ptx, unsigned int pty ) 
{
	resolution.x = ptx;
	resolution.y = pty;
}
std::string stripExtension( std::string& pFilename )
{
	unsigned int pos = pFilename.find_last_of( '.' );
	if( pos == std::string::npos ) {
		return "";
	}
	std::string ext = pFilename.substr( pos );
	pFilename = pFilename.substr( 0, pos );
	return ext;
}

Font::Font( std::string pFilename )
{
	filepath = FONT_DIR + pFilename; 
	extension = stripExtension( pFilename );
	name = pFilename;

	printf( " Extension: %s\n", extension.c_str() );
}

void Font::read()
{
	if ( extension == ".font" ) {
		readFontfile();
	}
	else {
		readFace();
	}
}

void Font::readFace()
{
	FT_Face face;
	ft_error = FT_New_Face( ftLib, filepath.c_str(), 0, &face );
	if ( ft_error || !face ) {
		printf( "Could not load font %s\n", filepath.c_str() ); 
		return;
	}

	FT_Set_Char_Size( face, size.x*64, size.y*64, resolution.x, resolution.y );
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
	atlas.pixels = (png_byte*)malloc( atlas.width * atlas.height );

	// now write glyph bitmaps to glyph quads in atlas
	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		FT_Load_Char( face, gi, FT_LOAD_RENDER );
		FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );

		writeGlyphBitmap( glyphs.quads[ gi ], face->glyph->bitmap );
	}

	FT_Done_Face( face );
}

void Font::readFontfile()
{
	FILE* file = fopen( filepath.c_str(), "rb" );

	if ( !file ) {
		printf( "Failed to open file %s\n", filepath.c_str() );
		return;
	}

	unsigned int glyphs_bytes = readGlyphs( file );
	if ( glyphs_bytes == 0 ) {
		printf( "Font file %s invalid!", filepath.c_str() );
		return;
	}
	atlas = Image( file, glyphs_bytes  );
	atlas.read();

	fclose( file );
}

unsigned int Font::readGlyphs( FILE* file )
{
       fread( &glyphs.count, sizeof( unsigned int ), 1, file ); 
       if ( glyphs.count == 0 ) {
               return 0;
       }
       glyphs.quads.resize( glyphs.count );
       glyphs.metrics.resize( glyphs.count );
       fread( &glyphs.quads[0], sizeof( glm::uvec4 ), glyphs.count, file ); 
       fread( &glyphs.metrics[0], sizeof( Glyphs::Metric ), glyphs.count, file ); 

       unsigned int read_size = sizeof( unsigned int );
       read_size += sizeof( glm::uvec4 ) * glyphs.quads.size();
       read_size += sizeof( Glyphs::Metric ) * glyphs.metrics.size();

       return read_size;
}

std::string Font::write()
{
	std::string outname = std::string( name + ".font" );
	FILE* file = fopen( outname.c_str(), "wb" );

	unsigned int glyphs_size = writeGlyphs( file );
	Image atlas_image( file, glyphs_size );
	atlas_image.write( &atlas.pixels[0], atlas.width, atlas.height );

	fclose( file );
	return outname;
}

unsigned int Font::writeGlyphs( FILE* file )
{
	fwrite( &glyphs.count, sizeof( unsigned int ), 1, file ); 
	fwrite( &glyphs.quads[0], sizeof( glm::uvec4 ), glyphs.count, file ); 
	fwrite( &glyphs.metrics[0], sizeof( Glyphs::Metric ), glyphs.count, file ); 

	unsigned int write_size = sizeof( unsigned int );
	write_size += sizeof( glm::uvec4 ) * glyphs.quads.size();
	write_size += sizeof( Glyphs::Metric ) * glyphs.metrics.size();

	return write_size;
}

int text::initFreeType()
{
	if ( FT_Init_FreeType( &ftLib ) ) {
		puts( "Failed to init FreeType!" );
		return -1;
	}
	return 0;
}

