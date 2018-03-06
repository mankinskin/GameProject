#include "colorings.h"
#include <algorithm>
#include "gui.h"
#include "vao.h"
#include "shader.h"
#include "gl.h"
#include "gldebug.h"
#include "texture.h"


size_t guiTextureAtlas;
std::vector<glm::vec4> allAtlasUVs;
size_t uvBuffer = 0;

unsigned int gui::constColorBuffer;
unsigned int gui::constColorVAO;
unsigned int gui::constColorShader;
unsigned int gui::constColoringBuffer;
std::vector<glm::vec4> gui::constColors;
//template<class ColorType>
//ColorType gui::colorings[gui::MAX_QUAD_COUNT];  //one index into constColors for each quad
std::vector<std::string> constColorNames;

gui::ConstColor::ConstColor(std::string pColorName) {
	color_index = std::find(constColorNames.begin(), constColorNames.end(), pColorName) - constColorNames.begin();
}

void gui::initConstColorVAO() {
	glCreateVertexArrays(1, &constColorVAO);
	glBindVertexArray(constColorVAO);

	glVertexArrayElementBuffer(constColorVAO, gl::quadEBO + 1);
	glVertexArrayVertexBuffer(constColorVAO, 0, gl::quadVBO + 1, 0, sizeof(glm::vec2));
	vao::setVertexAttrib(constColorVAO, 0, 0, 2, GL_FLOAT, 0);

	constColoringBuffer = vao::createStorage(sizeof(size_t) * MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	vao::createStream(constColoringBuffer, GL_MAP_WRITE_BIT);
	vao::bindStorage(GL_UNIFORM_BUFFER, constColoringBuffer);

	glBindVertexArray(0);
}

void gui::initConstColorShader() {
	constColorShader = shader::newProgram("constColorQuadShader", shader::createModule("constColorQuadShader.vert"), shader::createModule("constColorQuadShader.frag"));
	shader::addVertexAttribute(constColorShader, "corner_pos", 0);
}


void gui::setupConstColorShader() {
	shader::bindUniformBufferToShader(constColorShader, quadBuffer, "QuadBuffer");
	shader::bindUniformBufferToShader(constColorShader, constColoringBuffer, "ColoringBuffer");
	shader::bindUniformBufferToShader(constColorShader, constColorBuffer, "ColorBuffer");
}

void gui::updateConstColors()
{
	vao::uploadStorage(constColoringBuffer, sizeof(size_t)*MAX_QUAD_COUNT, &colorings<ConstColor>[0]);
}

void gui::uploadUVRanges()
{
	vao::initStorageData(uvBuffer, sizeof(glm::vec4)*MAX_QUAD_COUNT, &allAtlasUVs[0], 0);
}

size_t gui::createAtlasUVRange(glm::vec4 pUVRange)
{
	allAtlasUVs.push_back(pUVRange);
	return allAtlasUVs.size() - 1;
}

gui::ConstColor gui::createConstColor(glm::vec4 pColor, std::string pColorName)
{
	constColors.push_back(pColor);
	constColorNames.push_back(pColorName);
	return constColors.size() - 1;
}
size_t gui::getConstColor(std::string pColorName)
{
	size_t s = std::find(constColorNames.begin(), constColorNames.end(), pColorName) - constColorNames.begin();
	return s;
}

void gui::renderConstColors() {
	glBindVertexArray(constColorVAO);
	shader::use(constColorShader);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allQuads.size());

	shader::unuse();
	glBindVertexArray(0);
}

void gui::uploadConstColors()
{
	constColorBuffer = vao::createStorage(sizeof(glm::vec4) * constColors.size(), &constColors[0], 0);
	vao::bindStorage(GL_UNIFORM_BUFFER, constColorBuffer);
}

void gui::initColoringVAOs()
{
	initConstColorVAO();
}

void gui::initColoringShaders()
{
	initConstColorShader();
}

void gui::setupColoringShaders()
{
	setupConstColorShader();
}

void gui::updateColorings()
{
	updateConstColors();
}

void gui::renderColorings()
{
	glDepthFunc(GL_LEQUAL);
	renderConstColors();
	glDepthFunc(GL_LESS);
}
