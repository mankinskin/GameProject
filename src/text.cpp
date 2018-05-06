#include "text.h"


unsigned int atlasTexture = 0;
using namespace text;

void text::loadFonts()
{
	initFreeType();
	Font font( "LiberationMono-Regular.ttf" );
	font.read();
	
}
