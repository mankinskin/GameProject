#include "font.h"
#include <unistd.h>
#include <fcntl.h>

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

void Font::read( std::string pFilename)
{
	FILE* file = fopen( pFilename.c_str(), "rb" );

	if ( !file ) {
		printf( "Failed to open file %s\n", pFilename.c_str() );
		return;
	}

	unsigned int glyphs_size = readGlyphs( file );
	if ( glyphs_size == 0 ) {
		printf( "Font file %s invalid!", pFilename.c_str() );
		return;
	}
	Image atlas_image( file, glyphs_size );
	atlas_image.read();
	atlas = atlas_image;

	fclose( file );
}
