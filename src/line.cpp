#include "line.h"
#include "shader.h"
#include "vao.h"

std::vector<gui::LineGroup> allLineGroups;

std::array<glm::uvec2, gui::MAX_LINE_VERTEX_COUNT> allLineVertices;
unsigned int lineVertexCount = 0;
std::array<glm::vec4, gui::MAX_LINE_VERTEX_POSITION_COUNT> allLineVertexPositions;
unsigned int lineVertexPositionCount = 0;

gl::StreamStorage<glm::uvec2> lineVertexBuffer;
gl::StreamStorage<glm::vec4> lineVertexPositionBuffer;

gl::VAO lineVAO;
static shader::Program lineShader;

unsigned int gui::getLineCount()
{
  return lineVertexCount/2;
}
gui::LineGroup::LineGroup(unsigned int pLineOffset,
	unsigned int pLineCount, int pFlags)
  : lineOffset(pLineOffset)
  , lineCount(pLineCount)
	, flags(1)
{}

unsigned int gui::createLineGroup(unsigned int pLineOffset,
	unsigned int pLineCount, int pFlags)
{
  allLineGroups.push_back(LineGroup(pLineOffset, pLineCount, pFlags));
  return allLineGroups.size() - 1;
}

void gui::toggleLineGroup(unsigned int pLineGroup)
{
  allLineGroups[pLineGroup].flags = !allLineGroups[pLineGroup].flags;
}

unsigned int gui::createLineVertexPos(glm::vec4 pPos)
{
  allLineVertexPositions[ lineVertexPositionCount ] = pPos;
  return lineVertexPositionCount++;
}

unsigned int gui::createLineVertex(unsigned int pPos, unsigned int pColorData)
{
  allLineVertices[ lineVertexCount ] = glm::uvec2(pPos, pColorData);
  return lineVertexCount++;
}

glm::uvec2 gui::createLine(unsigned int pPosA, unsigned int pPosB, unsigned int pColorA, unsigned int pColorB)
{
  return glm::uvec2(createLineVertex(pPosA, pColorA), createLineVertex(pPosB, pColorB));
}
glm::uvec2 gui::createLine(unsigned int pPosA, unsigned int pPosB, unsigned int pColorData)
{
  return createLine(pPosA, pPosB, pColorData, pColorData);
}

void gui::initLineVAO()
{
  lineVertexPositionBuffer = gl::StreamStorage<glm::vec4>("LineVertexPositonBuffer",
	  MAX_LINE_VERTEX_POSITION_COUNT, GL_MAP_WRITE_BIT);
  lineVertexPositionBuffer.setTarget(GL_UNIFORM_BUFFER);

  lineVertexBuffer = gl::StreamStorage<glm::uvec2>("LineVertexBuffer",
	  MAX_LINE_VERTEX_COUNT, GL_MAP_WRITE_BIT);

  lineVAO = gl::VAO("lineVAO");
  lineVAO.vertexBuffer(0, lineVertexBuffer, 0, sizeof(unsigned int) * 2);
  lineVAO.vertexAttrib(0, 0, 2, GL_UNSIGNED_INT, 0);
}

void gui::updateLinePositions()
{
  gl::uploadStorage(lineVertexPositionBuffer, sizeof(glm::vec4) * lineVertexPositionCount, &allLineVertexPositions[0]);
}

void gui::updateLineColors()
{
  gl::uploadStorage(lineVertexBuffer, sizeof(glm::uvec2) * lineVertexCount, &allLineVertices[0]);
}

void gui::renderLines()
{
  lineVAO.bind();
  glDepthFunc(GL_ALWAYS);
  glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
  lineShader.use();
  for (unsigned int g = 0; g < allLineGroups.size(); ++g) {
	const LineGroup& lineGroup = allLineGroups[g];
	if (lineGroup.flags) {
	  glDrawArrays(GL_LINES,
		  lineGroup.lineOffset * 2,
		  lineGroup.lineCount * 2);
	}
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LESS);
  shader::Program::unuse();
  lineVAO.unbind();
}

void gui::initLineShader()
{
  lineShader = shader::Program("lineShader",
	  shader::Stage("lineShader.vert"),
	  shader::Stage("lineShader.frag"));
  lineShader.addVertexAttribute("vertexColor", 0);
}

void gui::setupLineShader()
{
  lineShader.build();
  lineShader.bindUniformBuffer(gl::generalUniformBuffer, "GeneralUniformBuffer");
  lineShader.bindUniformBuffer(gl::colorBuffer, "ColorBuffer");
  lineShader.bindUniformBuffer(lineVertexPositionBuffer, "PosBuffer");
}

