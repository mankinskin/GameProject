#pragma once
#include "utils/id.h"
#include "storage.h"
#include <vector>
#include <string>
#include <glm.hpp>
#include <array>

namespace gl
{
  using ColorData = glm::vec4;
  const size_t MAX_COLOR_COUNT = 100;
  extern utils::Container<ColorData> colors;
  struct Color : public utils::ID<ColorData>
  {
	Color()
	  : utils::ID<ColorData>(&colors)
	{}
	Color(utils::ID<ColorData> id)
	  : utils::ID<ColorData>(id)
	{}
	Color(size_t i)
	  : utils::ID<ColorData>(i, &colors)
	{}
  };

  using ConstColorData = glm::vec4;
  using ConstColor = utils::ID<ConstColorData>;
  const size_t CONST_COLOR_COUNT = 10;

  extern StreamStorage<ColorData> colorBuffer;

  void initColors();
  void createDefaultColors();

  void initColorBuffer();
  void updateColorBuffer();

  const Color createColor(const glm::vec4 pColorData, const std::string pColorName);
  const Color getColor(const std::string pColorName);
}
