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

    const utils::ID<Color> createColor(const glm::vec4 pColor, const std::string pColorName);
    const utils::ID<Color> getColor(const std::string pColorName);
}
