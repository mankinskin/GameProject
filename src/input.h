#pragma once
#include "keys.h"
#include "gates.h"
#include "event.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <utility>
#include <initializer_list>

namespace input 
{
    void init();
    void setupControls();
    void fetchGLFWEvents();
    void end();
}
