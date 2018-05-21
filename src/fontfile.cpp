#include "fontfile.h"
#include "path.h"

FT_Library ftLib;

void text::FontFile::setLoadPadding( unsigned int padPixels )
{
	padding = padPixels;
}

void text::FontFile::setLoadSize( unsigned int ptx, unsigned int pty ) 
{
	if ( !ptx ) {
		ptx = pty;	
	} 
	else if ( !pty ) {
		pty = ptx;	
	}
	size.x = ptx;
	size.y = pty;
}

void text::FontFile::setLoadDpi( glm::uvec2 pDpi ) 
{
	setLoadDpi( pDpi.x, pDpi.y );
}

void text::FontFile::setLoadDpi( unsigned int ptx, unsigned int pty ) 
{
	if ( !ptx ) {
		ptx = pty;	
	} 
	else if ( !pty ) {
		pty = ptx;	
	}
	dpi.x = ptx;
	dpi.y = pty;
}

void text::FontFile::read( std::string pFilename )
{
	std::string filepath = FONT_DIR + pFilename; 
	std::string extension = getExtension( pFilename );
	name = pFilename;
	if ( extension == "font" ) {
		readFontfile( filepath );
	}
	else {
		readFace( filepath );
	}
}

void text::FontFile::readFace( std::string pFilepath )
{
	FT_Face face;
	ft_error = FT_New_Face( ftLib, pFilepath.c_str(), 0, &face );
	if ( ft_error || !face ) {
		printf( "Could not load font %s\n", pFilepath.c_str() ); 
		return;
	}

	FT_Set_Char_Size( face, size.x*64, size.y*64, dpi.x, dpi.y );
	glyphs.resize( face->num_glyphs );	

	unsigned int max_glyph_width = ( face->bbox.xMax - face->bbox.xMin ) / 64;
	linegap = ( face->bbox.yMax - face->bbox.yMin ) / 64;
	unsigned int row_length = std::ceil( std::sqrt( face->num_glyphs ) );
	unsigned int max_atlas_width = row_length * max_glyph_width;

	unsigned int row_max_height = 0;
	unsigned int max_row_width = 0;

	glm::uvec2 cursor = glm::uvec2( 0, 0 );

	// precalculate glyph quads
	FT_Int32 load_mode = FT_LOAD_RENDER; 
	FT_Render_Mode_ render_mode = FT_RENDER_MODE_NORMAL; 

	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		FT_Load_Char( face, gi, load_mode );
		FT_Render_Glyph( face->glyph, render_mode );
		FT_GlyphSlot glyph = face->glyph;

		Glyphs::Metric& met = glyphs.metrics[ gi ];
		met = Glyphs::Metric( 
				glyph->metrics.horiAdvance/64, 
				glyph->metrics.horiBearingX/64,
				glyph->metrics.horiBearingY/64 );

		glm::uvec4& quad = glyphs.quads[ gi ];

		unsigned int width = glyph->bitmap.width; 
		unsigned int height = glyph->bitmap.rows; 

		quad = glm::uvec4( 
				cursor.x + padding, 
				cursor.y + padding, 
				width,
				height);

		cursor.x += width + padding * 2;
		row_max_height = std::max( height + padding * 2, row_max_height );

		// at the end of a row, do a line break
		if ( ( gi + 1 ) % row_length == 0 ) {
			max_row_width = std::max( cursor.x, max_row_width );
			cursor.x = 0;
			cursor.y += row_max_height;
			row_max_height = 0;
		}
	}

	// if glyphs.count is not divisible by row_length
	// a new row had already been started but not finished
	if( glyphs.count % row_length ) {
		cursor.y += row_max_height;
	}
	atlas.width = max_row_width;
	atlas.height = cursor.y;
	cursor = glm::uvec2( 0, 0 );
	atlas.pixels = (unsigned char*)malloc( atlas.width * atlas.height );

	// now write glyph bitmaps to glyph quads in atlas
	for ( unsigned int gi = 0; gi < glyphs.count; ++gi ) {
		FT_Load_Char( face, gi, load_mode );
		FT_Render_Glyph( face->glyph, render_mode );
		FT_Bitmap& bitmap = face->glyph->bitmap;

		glm::uvec4& quad = glyphs.quads[ gi ];

		if ( load_mode & FT_LOAD_TARGET_MONO ) { 
			for ( unsigned int row = 0; row < bitmap.rows; ++row ) {
				for ( unsigned int byte = 0; byte < bitmap.pitch; ++byte ) {
					const unsigned char& bitmap_byte = bitmap.buffer[ row * bitmap.pitch + byte ]; 
					for ( unsigned int bit = 0; bit < 8; ++bit ) {
						atlas.pixels[ ( quad.y + row ) * atlas.width + quad.x + byte * 8 + bit ] = 
							(unsigned char)( ( bitmap_byte << bit ) & 10000000 ); 
					}
				}
			}
		} else {
			for ( unsigned int row = 0; row < bitmap.rows; ++row ) {
				std::memcpy( &atlas.pixels[ ( quad.y + row ) * atlas.width + quad.x ], 
						&bitmap.buffer[ row * bitmap.pitch ], bitmap.pitch );
			}
		}
	}
	printf( "Loaded Atlas: %s\nWidth %u\nHeight: %u\nSize: %u\nDpi: %u %u\n", 
			name.c_str(), atlas.width, atlas.height, size.x, dpi.x, dpi.y ); 
	printf( "Linegap val: %u\n", linegap );

	FT_Done_Face( face );
}

void text::FontFile::readFontfile( std::string pFilepath )
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

unsigned int text::FontFile::readGlyphs( FILE* file )
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

std::string text::FontFile::write()
{
	std::string outname = std::string( name + ".font" );
	FILE* file = fopen( outname.c_str(), "wb" );

	unsigned int glyphs_size = writeGlyphs( file );
	fseek( file, glyphs_size, SEEK_SET );
	atlas.write( file );

	fclose( file );
	return outname;
}

unsigned int text::FontFile::writeGlyphs( FILE* file )
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

