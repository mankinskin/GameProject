#include <string>

#include "loader.h"


int main()
{
	loader::initFreeType();
	printf( "Type name of font to load:\n> " );
	std::string fontfile = "LiberationMono-Regular.ttf";
	puts( fontfile.c_str() );
	printf( "Loading %s ...\n\n", fontfile.c_str() );

	loader::FontMemory font; 
	if ( font.loadFile( fontfile ) ) {
		return 1;
	}
	font.print();
	return 0;
}

