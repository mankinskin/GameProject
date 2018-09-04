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
  using Color = utils::ID<ColorData>;
  const size_t MAX_COLOR_COUNT = 100;

  using ConstColorData = glm::vec4;
  using ConstColor = utils::ID<ConstColorData>;
  const size_t CONST_COLOR_COUNT = 10;

  extern StreamStorage<ColorData> colorBuffer;

  void initColors();
  void createDefaultColors();

  void initColorBuffer();
  void updateColorBuffer();

  const utils::ID<ColorData> createColor(const glm::vec4 pColorData, const std::string pColorName);
  const utils::ID<ColorData> getColor(const std::string pColorName);
}
