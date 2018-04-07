#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include "utils.h"
#include "storage.h"

namespace gl {

    typedef glm::vec4 Color;

    typedef utils::Itr<Color> ColorIt;

	const unsigned int MAX_COLOR_COUNT = 100;
	extern Storage colorBuffer;

	extern std::vector<Color> allColors;

    void initColors();
	void createDefaultColors();

    void initColorBuffer();
    void updateColorBuffer();

    ColorIt createColor( glm::vec4 pColor, std::string pColorName );
    ColorIt getColor( std::string pColorName );
    
    Color getColorData( ColorIt colorIndex );
    Color getColorData( std::string colorName );
};
