#pragma once
#include <array>
#include <vector>
#include <glm.hpp>
#include "gl.h"
#include "color.h"
#include "utils/id.h"
#include "storage.h"

namespace gui
{
  const unsigned int MAX_LINE_VERTEX_COUNT = 400;
  const unsigned int MAX_LINE_VERTEX_POSITION_COUNT = 200;
  const int DEFAULT_LINE_GROUP_FLAGS = 1;

  struct LineGroup
  {
	LineGroup(unsigned int pLineOffset, unsigned int pLineCount,
		int pFlags = DEFAULT_LINE_GROUP_FLAGS);
	unsigned int lineOffset;
	unsigned int lineCount;
	int flags;
  };

  void toggleLineGroup(unsigned int pLineGroup);
  unsigned int createLineGroup(unsigned int pLineOffset,
	  unsigned int pLineCount, int pFlags = DEFAULT_LINE_GROUP_FLAGS);
  //struct LineVertex{
  //    LinePointIt vertex;
  //    gl::Color color;
  //};


  glm::uvec2 createLine(unsigned int pPosA, unsigned int pPosB, unsigned int pColorA, unsigned int pColorB);
  glm::uvec2 createLine(unsigned int pPosA, unsigned int pPosB, unsigned int pColorData);
  unsigned int createLineVertexPos(glm::vec4 pPos);
  unsigned int createLineVertex(unsigned int pVertex, unsigned int pColorData = 0);
  void setLineVertexColor(unsigned int pVertex, unsigned int pColorData);


  void initLineVAO();
  void updateLinePositions();
  void updateLineColors();
  void renderLines();
  void initLineShader();
  void setupLineShader();
  unsigned int getLineCount();
}

