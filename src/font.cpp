#include "font.h"
#include <unistd.h>
#include <fcntl.h>

FT_Library ftLib;

using namespace text;


void Font::setLoadPadding( unsigned int padPixels )
{
	padding = padPixels;
}

void Font::setLoadSize( unsigned int ptx, unsigned int pty ) 
{
	if ( ptx || pty ) {
		if ( !ptx ) {
			ptx = pty;	
		} 
		else if ( !pty ) {
			pty = ptx;	
		}
		size.x = ptx;
		size.y = pty;
	}
}

void Font::setLoadResolution( unsigned int ptx, unsigned int pty ) 
{
	if ( ptx || pty ) {
		if ( !ptx ) {
			ptx = pty;	
		} 
		else if ( !pty ) {
			pty = ptx;	
		}
		resolution.x = ptx;
		resolution.y = pty;
	}
}
std::string stripExtension( std::string& pFilename )
{
	unsigned int pos = pFilename.find_last_of( '.' );
	if( pos == std::string::npos ) {
		return "";
	}
	std::string ext = pFilename.substr( pos + 1 );
	pFilename = pFilename.substr( 0, pos );
	return ext;
}


void Font::read( std::string pFilename )
{
	std::string filepath = FONT_DIR + pFilename; 
	std::string extension = stripExtension( pFilename );
	name = pFilename;
	if ( extension == "font" ) {
		readFontfile( filepath );
	}
	else {
		readFace( filepath );
	}
}

void Font::readFace( std::string pFilepath )
{
	FT_Face face;
	ft_error = FT_New_Face( ftLib, pFilepath.c_str(), 0, &face );
	if ( ft_error || !face ) {
		printf( "Could not load font %s\n", pFilepath.c_str() ); 
		return;
	}

	FT_Set_Char_Size( face, size.x*64, size.y*64, resolution.x, resolution.y );
	glyphs.resize( face->num_glyphs );	

	unsigned int max_glyph_width = face->bbox.xMax - face->bbox.xMin;
	unsigned int row_length = std::ceil( std::sqrt( face->num_glyphs ) );
	unsigned int max_atlas_width = row_length * max_glyph_width;

	unsigned int row_max_height = 0;
	unsigned int max_row_width = 0;

	glm::uvec2 cursor = glm::uvec2( 0, 0 );
	// precalculate glyph quads and store metrics
	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		FT_Load_Char( face, gi, FT_LOAD_RENDER );
		FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
		FT_GlyphSlot glyph = face->glyph;

		Font::Glyphs::Metric& met = glyphs.metrics[ gi ];
		met = Font::Glyphs::Metric( 
				glyph->metrics.horiAdvance/64, 
				glyph->metrics.horiBearingX/64,
				glyph->metrics.horiBearingY/64 );

		glm::uvec4& quad = glyphs.quads[ gi ];


		quad = glm::uvec4( 
				cursor.x + padding, 
				cursor.y + padding, 
				glyph->bitmap.width,
				glyph->bitmap.rows );

		cursor.x += quad.z + padding * 2;
		row_max_height = std::max( quad.w + padding * 2, row_max_height );

		if ( ( gi + 1 ) % row_length == 0 ) {
			max_row_width = std::max( cursor.x, max_row_width );
			cursor.x = 0;
			cursor.y += row_max_height;
			row_max_height = 0;
		}
	}

	if( glyphs.count % row_length ) {
		cursor.y += row_max_height;
	}
	atlas.width = max_row_width;
	atlas.height = cursor.y;
	cursor = glm::uvec2( 0, 0 );
	atlas.pixels = (unsigned char*)malloc( atlas.width * atlas.height );

	printf( "Loaded Atlas: %s\nWidth %u\nHeight: %u\nSize: %u\nResolution: %u %u\n", 
			name.c_str(), atlas.width, atlas.height, size.x, resolution.x, resolution.y ); 
	//for( unsigned int m = 0; m < glyphs.metrics.size(); ++m ) {
	//	Font::Glyphs::Metric& met = glyphs.metrics[ m ];
	//	printf( "Glyph %u: \n\tadvance %u\n\tbearingx %u\n\tbearingy %u\n\n", 
	//			m, met.advance, met.bearingx, met.bearingy ); 
	//}
	// now write glyph bitmaps to glyph quads in atlas
	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		FT_Load_Char( face, gi, FT_LOAD_RENDER );
		FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
		FT_GlyphSlot glyph = face->glyph;

		glm::uvec4& quad = glyphs.quads[ gi ];

		writeGlyphBitmap( quad.x, quad.y, face->glyph->bitmap );
	}
	

	FT_Done_Face( face );
}

void Font::writeGlyphBitmap( const unsigned int x, const unsigned int y, const FT_Bitmap& bitmap )
{
	for ( unsigned int row = 0; row < bitmap.rows; ++row ) {
		glm::uvec2 cur = glm::uvec2( x, y + row );
		std::memcpy( 
				&atlas.pixels[ cur.y * atlas.width + cur.x ], 
				&bitmap.buffer[ row * bitmap.pitch ], bitmap.pitch );
	}
}

void Font::readFontfile( std::string pFilepath )
{
	FILE* file = fopen( pFilepath.c_str(), "rb" );

	if ( !file ) {
		printf( "Failed to open file %s\n", pFilepath.c_str() );
		return;
	}

	unsigned int glyphs_bytes = readGlyphs( file );
	if ( glyphs_bytes == 0 ) {
		printf( "Font file %s invalid!", pFilepath.c_str() );
		return;
	}
	fseek( file, glyphs_bytes, SEEK_SET );
	atlas.read( file );
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
	fseek( file, glyphs_size, SEEK_SET );
	atlas.write( file );

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

