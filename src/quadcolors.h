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
#include <array>

namespace gui
{
  extern gl::VAO colorQuadVAO;
  extern gl::StreamStorage<unsigned int> colorQuadBuffer;
  extern std::array<unsigned int, gui::MAX_QUAD_COUNT> quadColors;

  void initColorQuadShader();
  void setupColorQuadShader();

  void renderColorQuads();

  void initColorQuadVAO();
  void updateColorQuads();
  void renderColorQuads();

  template<template<class>class M1, template<class>class M2>
	void colorQuad(const utils::ID<Quad, M1>& pID, const utils::ID<gl::ColorData, M2>& pColor)
	{
	  printf("Coloring Quad %lu with color %lu\n", pID.index, pColor.index);
	  quadColors[pID.index] = pColor.index;
	}
}

