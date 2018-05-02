#include "font.h"
#include "image.h"

void Font::save()
{
	Image atlas_image( name + "_atlas.png" );
	atlas_image.write( &atlas.pixels[0], atlas.width, atlas.height );
	


}
