#include <string>

#include "font.h"
#include "loader.h"
#include "image.h"


int main()
{
	initFreeType();
	printf( "Type name of font to load:\n> " );
	std::string fontfile = "LiberationMono-Regular.ttf";
	puts( fontfile.c_str() );
	printf( "Loading %s ...\n\n", fontfile.c_str() );

	Loader fontloader( fontfile ); 
	fontloader.setSize( 4, 0 );
	fontloader.setPadding( 2 );
	Font font = fontloader.loadFont();
	font.save();	

	return 0;
}
