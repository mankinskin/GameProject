#include "image.h"
#include <glm.hpp>
#include <png.h>

unsigned int getChannels( png_byte type )
{
	if ( type == PNG_COLOR_TYPE_GRAY || type == PNG_COLOR_TYPE_PALETTE ) {
		return 1;
	}
	else if ( type == PNG_COLOR_TYPE_GRAY_ALPHA ) {
		return 2;
	}
	else if ( type == PNG_COLOR_TYPE_RGB ) {
		return 3;
	}
	else if ( type == PNG_COLOR_TYPE_RGBA ) {
		return 4;
	}
	return -1;
}
png_byte getColorType( unsigned int channels )
{
	if ( channels == 1 ) {
		return PNG_COLOR_TYPE_GRAY;
	}
	else if ( channels == 2 ) {
		return PNG_COLOR_TYPE_GRAY_ALPHA;
	}
	else if ( channels == 3 ) {
		return PNG_COLOR_TYPE_RGB;
	}
	else if ( channels == 4 ) {
		return PNG_COLOR_TYPE_RGBA;
	}
	return -1;
}
void Image::read( FILE* file )
{
	if ( !file ) {
		printf( "Can't read from file %s !", filename.c_str() );
		return;
	}

	png_structp png = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if ( !png ) {
		puts( "Failed to read PNG!" );
		return;
	}
	png_infop info = png_create_info_struct( png );
	setjmp( png_jmpbuf( png ) ); 
	png_init_io( png, file );

	png_read_info( png, info );
	width = png_get_image_width( png, info );
	height = png_get_image_height( png, info );
	channels = getChannels( png_get_color_type( png, info ) );
	png_byte bit_depth = png_get_bit_depth( png, info );

	png_read_update_info( png, info );

	unsigned int row_size = png_get_rowbytes( png, info );

	png_bytep* rows = (png_bytep*)malloc( sizeof(png_bytep) * height );

	pixels = (unsigned char*)malloc( sizeof(png_byte) * height * row_size );

	for ( unsigned int r = 0; r < height; ++r ) {
		rows[r] = pixels + row_size * r;
	}

	png_read_image( png, rows );
	pixels = rows[0];
}

void Image::write( FILE* file, unsigned char* pPixels, unsigned int pWidth, unsigned int pHeight, 
		unsigned int pChannels, unsigned char pBitDepth )
{
	width = pWidth;
	height = pHeight;
	pixels = pPixels;
	channels = pChannels;
	bit_depth = pBitDepth;

	write( file );
}

void Image::write( FILE* file )
{
	if ( !file ) {
		printf( "Can't write to file %s !", filename.c_str() );
		return;
	}

	png_structp png = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if ( !png ) {
		printf( "Failed to write PNG!" );
		return;
	}

	png_infop info = png_create_info_struct( png );
	setjmp( png_jmpbuf( png ) );
	png_init_io( png, file );

	png_set_IHDR(
			png, info, width, height, 
			(png_byte)bit_depth, getColorType( channels ),
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT );

	png_write_info( png, info );

	unsigned int row_size = png_get_rowbytes( png, info );

	png_bytep* rows = (png_bytep*)malloc( sizeof(png_bytep) * height );

	for ( unsigned int r = 0; r < height; ++r ) {
		rows[ r ] = (png_bytep)&pixels[ row_size * r ];
	}

	png_write_image( png, rows );
	png_write_end( png, NULL );
}

Image::~Image()
{
	free( pixels );
}
