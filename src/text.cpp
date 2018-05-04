#include "text.h"


using namespace text;



void text::loadFonts()
{
	initFreeType();
	Font font( "LiberationMono-Regular.ttf" );
	font.read();
	
}
