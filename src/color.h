#pragma once
#include "utils/id.h"
#include "storage.h"
#include <vector>
#include <string>
#include <glm.hpp>
#include <array>

namespace gl
{
    typedef glm::vec4 Color;
    const size_t MAX_COLOR_COUNT = 100;
    typedef utils::ID<Color> ColorID;

    extern StreamStorage<Color> colorBuffer;

    void initColors();
    void createDefaultColors();

    void initColorBuffer();
    void updateColorBuffer();

    ColorID createColor( glm::vec4 pColor, std::string pColorName );
    ColorID getColor( std::string pColorName );

    Color getColorData( ColorID colorIndex );
    Color getColorData( std::string colorName );
}
