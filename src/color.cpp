#include "color.h"
#include <algorithm>
#include "vao.h"
#include "gl.h"

std::vector<gl::Color> gl::allColors;
std::vector<std::string> colorNames;

gl::Storage<gl::Color> gl::colorBuffer;

void gl::initColors()
{
	createDefaultColors();
	initColorBuffer();
}

void gl::createDefaultColors()
{
    puts( "Creating constant colors..." );
	createColor( glm::vec4( 0.0f, 0.0f, 0.0f, 0.0f ), "nocolor" );
	createColor( glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f ), "black" );
	createColor( glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ), "white" );
	createColor( glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f ), "red" );
	createColor( glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), "green" );
	createColor( glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ), "blue" );
	createColor( glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f ), "yellow" );
	createColor( glm::vec4( 0.0f, 1.0f, 1.0f, 1.0f ), "cyan" );
	createColor( glm::vec4( 1.0f, 0.0f, 1.0f, 1.0f ), "magenta" );
	createColor( glm::vec4( 0.5f, 0.0f, 0.5f, 1.0f ), "purple" );
	createColor( glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f ), "grey" );
	createColor( glm::vec4( 0.2f, 0.2f, 0.2f, 1.0f ), "darkgrey" );
	createColor( glm::vec4( 0.7f, 0.7f, 0.7f, 1.0f ), "lightgrey" );
}

gl::ColorIt gl::createColor( Color pColor, std::string pColorName ) 
{
    printf( "Creating Color: %s ( %f, %f, %f, %f )\n", 
            pColorName.c_str(), pColor.x, pColor.y, pColor.z, pColor.w );
    allColors.push_back( pColor );
    colorNames.push_back( pColorName );
    return ColorIt( allColors, allColors.size() - 1 );
}

gl::ColorIt gl::getColor( std::string pColorName )
{
    std::vector<std::string>::iterator nameIt = 
        std::find( colorNames.begin(), colorNames.end(), pColorName ); 
    if ( nameIt == colorNames.end() ) {
        printf( "Color %s not found!\n", pColorName.c_str() );
        return ColorIt( allColors, 0 );
    }
    unsigned int i = nameIt - colorNames.begin();
    printf( "Color %s found at %d!\n", pColorName.c_str(), i );
    return ColorIt( allColors, i );
}

void gl::initColorBuffer()
{
    colorBuffer = StreamStorage<Color>( "ColorBuffer", MAX_COLOR_COUNT, GL_MAP_WRITE_BIT );
    gl::setStorageTarget( colorBuffer, GL_UNIFORM_BUFFER );
}

void gl::updateColorBuffer()
{
    if( allColors.size() ){
        //gl::uploadStorage( colorBuffer, allColors.size()*sizeof( Color ), &allColors[0] );
    }
}

gl::Color gl::getColorData( ColorIt colorIndex )
{
    return *colorIndex;
}

gl::Color gl::getColorData( std::string colorName )
{
    return *getColor( colorName );
}
