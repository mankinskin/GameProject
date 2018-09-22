#include "gldebug.h"
#include "debug.h"
#include "shader.h"
#include "camera.h"
#include "vao.h"
#include "framebuffer.h"
#include "line.h"
#include "color.h"
#include "quadcolors.h"


unsigned int coord_line_group;
unsigned int grid_1_line_group;
unsigned int grid_2_line_group;

void APIENTRY glerror_callback(GLenum sourcei, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  if (severity == GL_DEBUG_SEVERITY_HIGH)
	debug::fatal(message);
  else
	debug::warning(message);
}

void gl::debug::init()
{
  glDebugMessageCallback(glerror_callback, nullptr);
}

void gl::debug::createDebugGeometry()
{
  coord_line_group = initCoordinateSystem("coord");
  grid_1_line_group = generateDebugGrid("grid1.0", 1.0f, 100, 1.0f, 1.0f, 1.0f, 0.2f);
  //grid_2_line_group = generateDebugGrid("grid10.0", 1.0f, 100, 1.0f, 1.0f, 1.0f, 0.2f);
}

void gl::debug::toggleGrid()
{
  gui::toggleLineGroup(grid_1_line_group);
  gui::toggleLineGroup(grid_2_line_group);
}

void gl::debug::toggleCoord()
{
  gui::toggleLineGroup(coord_line_group);
}

unsigned int gl::debug::generateDebugGrid(std::string pName,
	float pTileSize, unsigned int pTileCount,
	float pColorR, float pColorG, float pColorB, float pAlpha)
{
  return generateDebugGrid(pName, pTileSize, pTileSize, pTileCount, pTileCount, pColorR, pColorG, pColorB, pAlpha);
}

unsigned int gl::debug::generateDebugGrid(std::string pName,
	float pTileSizeX, float pTileSizeY, unsigned int pTileCountX, unsigned int pTileCountY,
	float pColorR, float pColorG, float pColorB, float pAlpha)
{
  const size_t vertexCount = (pTileCountX+1) * 2 + (pTileCountY-1) * 2;
  //generate grid vertices and indices
  static std::vector<size_t> verts;
  verts.reserve(vertexCount);

  const unsigned int lineOffset = gui::getLineCount();
  unsigned int lineCount = pTileCountX + pTileCountY + 2;
  size_t grid_color = gl::createColor(glm::vec4(pColorR, pColorG, pColorB, pAlpha), pName + "_color").index;
  float xMin = -0.5f*pTileSizeX*((float)pTileCountX);
  float yMin = -0.5f*pTileSizeY*((float)pTileCountY);

  //horizontals
  for (size_t y = 0; y < pTileCountY - 1; ++y) {
    size_t vis[2] = {y*2, y*2+1} ;
    for (size_t v = 0; v < 2; ++v) {
      verts[vis[v]] = gui::createLineVertexPos(glm::vec4(xMin + !!v * pTileCountY * pTileSizeX, 0.0f, yMin + pTileSizeY * y + 1, 1.0f));
    }
    gui::createLine(verts[vis[0]], verts[vis[1]], grid_color);
  }
  const size_t offset = 2 * (pTileCountY-1);
  //verticals
  for (size_t x = 0; x < pTileCountX + 1; ++x) {
	size_t vis[2] = {offset + x*2, offset + x*2 + 1};
	for (size_t v = 0; v < 2; ++v) {
	  verts[vis[v]] = gui::createLineVertexPos(glm::vec4(xMin + x * pTileSizeX, 0.0f, yMin + !!v * pTileSizeY * pTileCountY, 1.0f));
	}
	gui::createLine(verts[vis[0]], verts[vis[1]] , grid_color);
  }
  gui::createLine(verts[offset], verts[vertexCount - 2] , grid_color);
  gui::createLine(verts[offset+1], verts[vertexCount - 1] , grid_color);

  return gui::createLineGroup(lineOffset, lineCount);
}

unsigned int gl::debug::initCoordinateSystem(std::string pName)
{
  unsigned int lineOffset = gui::getLineCount();
  unsigned int verts[7];

  verts[0] = gui::createLineVertexPos(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  verts[1] = gui::createLineVertexPos(glm::vec4(-50.0f, 0.0f, 0.0f, 1.0f));
  verts[2] = gui::createLineVertexPos(glm::vec4(50.0f, 0.0f, 0.0f, 1.0f));
  verts[3] = gui::createLineVertexPos(glm::vec4(0.0f, -50.0f, 0.0f, 1.0f));
  verts[4] = gui::createLineVertexPos(glm::vec4(0.0f, 50.0f, 0.0f, 1.0f));
  verts[5] = gui::createLineVertexPos(glm::vec4(0.0f, 0.0f, -50.0f, 1.0f));
  verts[6] = gui::createLineVertexPos(glm::vec4(0.0f, 0.0f, 50.0f, 1.0f));

  unsigned int red_color_i = gl::getColor("red").index;
  unsigned int green_color_i = gl::getColor("green").index;
  unsigned int blue_color_i = gl::getColor("blue").index;


  gui::createLine(verts[0], verts[1], red_color_i);
  gui::createLine(verts[0], verts[2], red_color_i);
  gui::createLine(verts[0], verts[3], green_color_i);
  gui::createLine(verts[0], verts[4], green_color_i);
  gui::createLine(verts[0], verts[5], blue_color_i);
  gui::createLine(verts[0], verts[6], blue_color_i);

  return gui::createLineGroup(lineOffset, gui::getLineCount() - lineOffset);
}
