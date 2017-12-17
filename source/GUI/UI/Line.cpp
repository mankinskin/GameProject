#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "Line.h"
#include "../../BaseGL/VAO.h"
#include "../../BaseGL/shader/shader.h"
#include "Colorings.h"
std::vector<glm::vec4> allLines;
std::vector<size_t> allLineColorIndices;
size_t guiLineVAO = 0;
size_t guiLineVBO = 0;
size_t guiLineColorIndexVBO = 0;
size_t guiLineShader = 0;
size_t MAX_GUI_LINES = 1000;

//size_t gui::createLine(glm::vec2 pV1, glm::vec2 pV2, size_t pColorIndex) {
//	allLines.push_back(glm::vec4(pV1, pV2));
//	allLineColorIndices.push_back(pColorIndex);
//	return allLines.size() - 1;
//}

void gui::setLineColor(size_t pLineIndex, size_t pColorIndex)
{
	allLineColorIndices[pLineIndex] = pColorIndex;
}

void gui::initLineVAO()
{
	glCreateVertexArrays(1, &guiLineVAO);

	guiLineVBO = vao::createStorage(sizeof(glm::vec4) * MAX_GUI_LINES, nullptr, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
	vao::createStream(guiLineVBO, GL_MAP_WRITE_BIT);

	guiLineColorIndexVBO = vao::createStorage(sizeof(size_t) * MAX_GUI_LINES, nullptr, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
	vao::createStream(guiLineColorIndexVBO, GL_MAP_WRITE_BIT);

	vao::setVertexArrayVertexStorage(guiLineVAO, 0, guiLineVBO, sizeof(glm::vec4));
	vao::setVertexArrayVertexStorage(guiLineVAO, 1, guiLineColorIndexVBO, sizeof(size_t));
	vao::setVertexAttrib(guiLineVAO, 0, 0, 4, GL_FLOAT, 0);
	vao::setVertexAttrib(guiLineVAO, 1, 1, 1, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(guiLineVAO, 0, 1);
	glVertexArrayBindingDivisor(guiLineVAO, 1, 1);
}

void gui::updateLines() {
	if (allLines.size()) {
		vao::uploadStorage(guiLineVBO, sizeof(glm::vec4) * allLines.size(), &allLines[0]);
		vao::uploadStorage(guiLineColorIndexVBO, sizeof(size_t) * allLineColorIndices.size(), &allLineColorIndices[0]);
	}
}

void gui::setupLineShader()
{
	//shader::bindUniformBufferToShader(guiLineShader, gui::Coloring<gui::ConstColor>::colorBuffer, "ColorBuffer");
}

void gui::renderLines()
{
	glBlendFunc(GL_ONE, GL_ONE);
	glBindVertexArray(guiLineVAO);
	shader::use(guiLineShader);
	glDrawArraysInstanced(GL_LINES, 0, 2, allLines.size());
	shader::unuse();
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void gui::initLineShader() {
	guiLineShader = shader::newProgram("guiLineShader", shader::createModule("guiLineShader.vert"), shader::createModule("guiLineShader.frag"));
	shader::addVertexAttribute(guiLineShader, "vertex_pair", 0);
	shader::addVertexAttribute(guiLineShader, "color_index", 1);
}

