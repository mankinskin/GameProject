#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include <array>
#include "utils.h"
#include "storage.h"

namespace gl 
{
	const unsigned int MAX_COLOR_COUNT = 100;
    typedef glm::vec4 Color;
    extern std::array<glm::vec4, gl::MAX_COLOR_COUNT> allColors;
    typedef utils::Itr<Color, std::array<Color, MAX_COLOR_COUNT>, allColors> ColorIt;

	extern StreamStorage<Color> colorBuffer;

    void initColors();
	void createDefaultColors();

    void initColorBuffer();
    void updateColorBuffer();

    ColorIt createColor( glm::vec4 pColor, std::string pColorName );
    ColorIt getColor( std::string pColorName );
    
    Color getColorData( ColorIt colorIndex );
    Color getColorData( std::string colorName );
}
