#include "gldebug.h"
#include "debug.h"
#include "shader.h"
#include "camera.h"
#include "vao.h"
#include "framebuffer.h"
#include "line.h"
#include "colorings.h"


size_t coord_line_group;
size_t grid_1_line_group;
size_t grid_2_line_group;
void APIENTRY glerror_callback(GLenum sourcei, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	debug::pushError(message, debug::Error::Fatal);
}
void glDebug::init()
{
	grid_1_line_group = glDebug::generateDebugGrid("grid1.0", 1.0f, 100, 1.0f, 1.0f, 1.0f, 0.3f);
	grid_2_line_group = glDebug::generateDebugGrid("grid10.0", 10.0f, 10, 1.0f, 1.0f, 1.0f, 0.3f);

	coord_line_group = glDebug::initCoordinateSystem("coord");

	glDebugMessageCallback(glerror_callback, nullptr);
}

void glDebug::toggleGrid()
{
	gui::toggleLineGroup(grid_1_line_group);
	gui::toggleLineGroup(grid_2_line_group);
}

void glDebug::toggleCoord()
{
	gui::toggleLineGroup(coord_line_group);
}

size_t glDebug::generateDebugGrid(std::string pName, float pTileSize, size_t pTileCount, float pColorR, float pColorG, float pColorB, float pAlpha)
{
	return generateDebugGrid(pName, pTileSize, pTileSize, pTileCount, pTileCount, pColorR, pColorG, pColorB, pAlpha);
}

size_t glDebug::generateDebugGrid(std::string pName, float pTileSizeX, float pTileSizeY, size_t pTileCountX, size_t pTileCountY, float pColorR, float pColorG, float pColorB, float pAlpha)
{
	const int DEFAULT_FLAG = 0;
	//generate grid vertices and indices
	int vertexCount = (pTileCountX-1) * 2 + (pTileCountY+1) * 2;
	std::vector<size_t> verts;

	size_t lineOffset = gui::getLineCount();
	size_t lineCount = pTileCountX + pTileCountY +2;
	size_t grid_color = gui::createConstColor(glm::vec4(pColorR, pColorG, pColorB, pAlpha), pName + "_color").color_index;
	size_t vertexOffset = vertexCount;
	verts.resize(vertexCount);
	vertexCount = 0;
	float xMin = -0.5f*pTileSizeX*((float)pTileCountX);
	float yMin = -0.5f*pTileSizeY*((float)pTileCountY);

	//horizontals
	for (size_t y = 0; y < pTileCountY + 1; ++y) {
		for (size_t x = 0; x < 2; ++x) {
			verts[y * 2 + x] = gui::createLineVertex(gui::createLineVertexPosition(glm::vec4(xMin + pTileSizeX * (pTileCountX) * x, 0.0f, yMin + (pTileSizeY) * y, 1.0f)), grid_color);
		}
		gui::createLine(verts[y * 2], verts[y * 2 + 1]);
		vertexCount += 2;
	}
	//vertical borders 
	size_t v = 0;
	size_t ind = 0;
	//verticals
	for (size_t x = 0; x < pTileCountX - 1; ++x) {
		for (size_t y = 0; y < 2; ++y) {
			verts[vertexCount + x * 2 + y] = gui::createLineVertex(gui::createLineVertexPosition(glm::vec4(xMin + (x+1)*pTileSizeX, 0.0f, yMin + y * pTileSizeY * pTileCountY, 1.0f)), grid_color);
		}
		gui::createLine(verts[vertexCount + x * 2], verts[vertexCount + x * 2 + 1]);
		v += 2;
	}
	gui::createLine(verts[0], verts[vertexCount - 2]);
	gui::createLine(verts[1], verts[vertexCount - 1]);
	


	return gui::createLineGroup(lineOffset, lineCount);
}

size_t glDebug::initCoordinateSystem(std::string pName)
{
	size_t lineOffset = gui::getLineCount();
	size_t verts[9];
	size_t red_color_i = gui::getConstColor("red");
	size_t green_color_i = gui::getConstColor("green");
	size_t blue_color_i = gui::getConstColor("blue");

	verts[0] = gui::createLineVertex(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), red_color_i);
	verts[1] = gui::createLineVertex(glm::vec4(-50.0f, 0.0f, 0.0f, 1.0f), red_color_i);
	verts[2] = gui::createLineVertex(glm::vec4(50.0f, 0.0f, 0.0f, 1.0f), red_color_i);

	verts[3] = gui::createLineVertex(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), green_color_i);
	verts[4] = gui::createLineVertex(glm::vec4(0.0f, -50.0f, 0.0f, 1.0f), green_color_i);
	verts[5] = gui::createLineVertex(glm::vec4(0.0f, 50.0f, 0.0f, 1.0f), green_color_i);

	verts[6] = gui::createLineVertex(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), blue_color_i);
	verts[7] = gui::createLineVertex(glm::vec4(0.0f, 0.0f, -50.0f, 1.0f), blue_color_i);
	verts[8] = gui::createLineVertex(glm::vec4(0.0f, 0.0f, 50.0f, 1.0f), blue_color_i);

	gui::createLine(verts[0], verts[1]);
	gui::createLine(verts[0], verts[2]);
	gui::createLine(verts[3], verts[4]);
	gui::createLine(verts[3], verts[5]);
	gui::createLine(verts[6], verts[7]);
	gui::createLine(verts[6], verts[8]);

	return gui::createLineGroup(lineOffset, 6);
}
