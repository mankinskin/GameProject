#include "image.h"
#include <glm.hpp>

void Image::write( png_bytep pPixels, unsigned int pWidth, unsigned int pHeight, 
		png_byte pType, png_byte pBitDepth )
{
	width = pWidth;
	height = pHeight;
	pixels = pPixels;
	color_type = pType;
	bit_depth = pBitDepth;

	FILE* file = fopen( filename.c_str(), "wb" );
	if ( !file ) {
		printf( "Failed to open/create file %s !", filename.c_str() );
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
			bit_depth, color_type,
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

	fclose( file );
}

void Image::read()
{
	FILE* file = fopen( filename.c_str(), "rb" );
	if ( !file ) {
		printf( "Failed to open/create file %s !", filename.c_str() );
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
	color_type = png_get_color_type( png, info );
	bit_depth = png_get_bit_depth( png, info );

	png_read_update_info( png, info );

	unsigned int row_size = png_get_rowbytes( png, info );

	png_bytep* rows = (png_bytep*)malloc( sizeof(png_bytep) * height );

	pixels = (png_bytep)malloc( sizeof(png_byte) * height * row_size );

	for ( unsigned int r = 0; r < height; ++r ) {
		rows[r] = pixels + row_size * r;
	}

	png_read_image( png, rows );
	pixels = rows[0];

	fclose( file );
}

