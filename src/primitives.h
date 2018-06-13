#pragma once
#include <cstddef>
#include <glm.hpp>
#include "storage.h"

namespace gl
{
    void initPrimitives();

    extern Storage<glm::vec2> quadVBO;
    extern Storage<unsigned int> quadEBO;

    extern Storage<glm::vec3> cubeVBO;
    extern Storage<unsigned int> cubeEBO;
}
