#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <utility>
#include <initializer_list>
namespace app {
	namespace Input {
		void init();
		void setupControls();
		void fetchGLFWEvents();
		void end();
	}
}
