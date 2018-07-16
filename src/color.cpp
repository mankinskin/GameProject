#include "color.h"
#include "gl.h"
#include "vao.h"
#include <algorithm>

std::array<std::string, gl::MAX_COLOR_COUNT> colorNames;
unsigned int colorCount = 0;

gl::StreamStorage<gl::Color> gl::colorBuffer;

void gl::initColors()
{
    createDefaultColors();
    initColorBuffer();
}

void gl::createDefaultColors()
{
    puts("Creating constant colors...");
    createColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), "nocolor");
    createColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), "black");
    createColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "white");
    createColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), "red");
    createColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), "green");
    createColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), "blue");
    createColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), "yellow");
    createColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), "cyan");
    createColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), "magenta");
    createColor(glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), "purple");
    createColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), "grey");
    createColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), "darkgrey");
    createColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), "lightgrey");
}

const utils::ID<gl::Color> gl::createColor(const Color pColor, const std::string pColorName)
{
    colorNames[ colorCount++ ] = pColorName;
    return ColorID(pColor);
}

const utils::ID<gl::Color> gl::getColor(const std::string pColorName)
{
    auto nameIt = std::find(colorNames.begin(),
            colorNames.begin() + colorCount, pColorName);
    if (nameIt == colorNames.end()) {
        printf("Color %s not found!\n", pColorName.c_str());
        return ColorID(utils::INVALID_ID);
    }
    unsigned int i = nameIt - colorNames.begin();
    return ColorID(i);
}

void gl::initColorBuffer()
{
    colorBuffer = StreamStorage<Color>("ColorBuffer", MAX_COLOR_COUNT,
            GL_MAP_WRITE_BIT);
    colorBuffer.setTarget(GL_UNIFORM_BUFFER);
}

void gl::updateColorBuffer()
{
    if (colorCount) {
        gl::uploadStorage(colorBuffer,
                sizeof(glm::vec4)*colorCount, &ColorID::container[0]);
    }
}
