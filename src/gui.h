#pragma once
#include "gl.h"
#include <string>
#include <unordered_map>

namespace gui
{
  void init();
  void initWidgets();
  glm::vec2 pixel_round(const glm::vec2 pIn);
  float pixel_round_x(const float pIn);
  float pixel_round_y(const float pIn);

  size_t toPixelsX(const float screenX);
  size_t toPixelsY(const float screenY);
  glm::uvec2 toPixels(const glm::vec2 screen);
  float toScreenX(const size_t pixelsX);
  float toScreenY(const size_t pixelsY);
  glm::vec2 toScreen(const glm::uvec2 pixels);
}

