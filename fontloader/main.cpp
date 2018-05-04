#include <cstdio>
#include <string>
#include "font.h"
#include "ttf.h"
#include "image.h"


int main()
{
	initFreeType();
	printf( "Type name of font to load:\n> " );
	std::string fontfile = "LiberationMono-Regular.ttf";
	puts( fontfile.c_str() );
	printf( "Loading %s ...\n\n", fontfile.c_str() );

	TTF::setPadding( 2 );
	TTF ttf( fontfile );
	ttf.setSize( 4, 0 );
	
	Font font;
	ttf.load( font );

	std::string outfile = font.write();	

	Font readfont;
	readfont.read( "atlas.png" );

	Image outatlas( "atlas.png" );
	outatlas.write( &readfont.atlas.pixels[0], readfont.atlas.width, readfont.atlas.height ); 

	return 0;
}
