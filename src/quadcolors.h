#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include <tuple>
#include "gui.h"
#include "utils/id.h"
#include "color.h"
#include "storage.h"
#include "vao.h"
#include "quad.h"

namespace gui
{
  extern gl::VAO colorQuadVAO;
  extern gl::StreamStorage<unsigned int> colorQuadBuffer;

  void initColorQuadShader();
  void setupColorQuadShader();

  void renderColorQuads();

  void initColorQuadVAO();
  void updateColorQuads();
  void renderColorQuads();
  void colorQuad(const utils::ID<Quad>& pID, const utils::ID<gl::ColorData>& pColorData);
}

