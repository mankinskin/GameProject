#pragma once
#include "utils/id.h"
#include "storage.h"
#include <vector>
#include <string>
#include <glm.hpp>
#include <array>

namespace gl
{
  using Color = glm::vec4;
  using ColorID = utils::ID<Color>;
  const size_t MAX_COLOR_COUNT = 100;

  using ConstColor = glm::vec4;
  using ConstColorID = utils::ID<ConstColor>;
  const size_t CONST_COLOR_COUNT = 10;

  extern StreamStorage<Color> colorBuffer;

  void initColors();
  void createDefaultColors();

  void initColorBuffer();
  void updateColorBuffer();

  const utils::ID<Color> createColor(const glm::vec4 pColor, const std::string pColorName);
  const utils::ID<Color> getColor(const std::string pColorName);
}
