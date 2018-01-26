#include "../../../Global/stdafx.h"
#include "stdafx.h"
#include "Line.h"
#include "../../BaseGL/VAO.h"
#include "../../BaseGL/shader/shader.h"
#include "Colorings.h"

std::vector<gui::LineGroup> allLineGroups;
std::vector<glm::vec4> allLineVertexPositions;
std::vector<glm::uvec2> allLineVertices;//stores a vertex as the indices to its data (pos & color)
std::vector<glm::uvec2> allLines;//stores each line as 2 indices to its vertices
size_t lineVAO = 0;
size_t lineVBO = 0;
size_t lineEBO = 0;
size_t linePosBuffer = 0;
size_t lineShader = 0;
size_t MAX_LINE_VERTEX_COUNT = 1000;
size_t MAX_LINE_COUNT = 1000;
std::vector<int> lineGroupFlags;

size_t gui::getLineCount() {
	return allLines.size();
}
size_t gui::createLineGroup(size_t pLineOffset, size_t pLineCount, int pFlag)
{
	allLineGroups.push_back(LineGroup(pLineOffset, pLineCount));
	lineGroupFlags.push_back(pFlag);
	return allLineGroups.size() - 1;
}
void gui::toggleLineGroup(size_t pLineGroup) {
	lineGroupFlags[pLineGroup] = !lineGroupFlags[pLineGroup];
}
size_t gui::createLine(glm::vec4 pVertexAPos, size_t pColorIndexA, glm::vec4 pVertexBPos, size_t pColorIndexB) {
	return createLine(createLineVertex(pVertexAPos, pColorIndexA), createLineVertex(pVertexBPos, pColorIndexB));
}
size_t gui::createLine(glm::vec4 pVertexAPos, glm::vec4 pVertexBPos, size_t pColorIndex) {
	return createLine(createLineVertex(pVertexAPos, pColorIndex), createLineVertex(pVertexBPos, pColorIndex));
}
size_t gui::createLine(size_t pVertexA, size_t pVertexB) {
	allLines.push_back(glm::uvec2(pVertexA, pVertexB));
	return allLines.size() - 1;
}

size_t gui::createLineVertex(glm::vec4 pPos, size_t pColorIndex)
{
	return createLineVertex(createLineVertexPosition(pPos), pColorIndex);
}
size_t gui::createLineVertex(size_t pPosIndex, size_t pColorIndex)
{
	allLineVertices.push_back(glm::uvec2(pPosIndex, pColorIndex));
	return allLineVertices.size() - 1;
}
size_t gui::createLineVertexPosition(glm::vec4 pPos) {
	allLineVertexPositions.push_back(pPos);
	return allLineVertexPositions.size() - 1;
}
void gui::setLineColor(size_t pLineIndex, size_t pColorIndex)
{
	setLineVertexColor(pLineIndex, 0, pColorIndex);
	setLineVertexColor(pLineIndex, 1, pColorIndex);
}
size_t getLineVertexIndex(size_t pLineIndex, size_t pVertex) {
	return *(size_t*)(&allLines[pLineIndex] + sizeof(size_t)*pVertex);
}
void gui::setLineVertexColor(size_t pLineIndex, size_t pVertex, size_t pColorIndex)
{
	std::memcpy(&allLineVertices[getLineVertexIndex(pLineIndex, pVertex)].y, &pColorIndex, sizeof(size_t));
}
void gui::initLineVAO()
{
	glCreateVertexArrays(1, &lineVAO);

	linePosBuffer = vao::createStorage(sizeof(glm::vec4) * MAX_LINE_VERTEX_COUNT, &allLineVertexPositions[0], 0);
	//vao::createStream(linePosBuffer, GL_MAP_WRITE_BIT);
	vao::bindStorage(GL_UNIFORM_BUFFER, linePosBuffer);

	lineVBO = vao::createStorage(sizeof(glm::uvec2) * MAX_LINE_VERTEX_COUNT, &allLineVertices[0], 0);
	//vao::createStream(lineVBO, GL_MAP_WRITE_BIT);

	lineEBO = vao::createStorage(sizeof(glm::uvec2) * MAX_LINE_VERTEX_COUNT, &allLines[0], 0);
	//vao::createStream(lineEBO, GL_MAP_WRITE_BIT);

	//vao::setVertexArrayVertexStorage(lineVAO, 0, lineVBO, sizeof(glm::uvec2));
	glVertexArrayVertexBuffer(lineVAO, 0, vao::getStorageID(lineVBO), 0, sizeof(glm::uvec2));
	vao::setVertexAttrib(lineVAO, 0, 0, 2, GL_UNSIGNED_INT, 0);

	glVertexArrayElementBuffer(lineVAO, vao::getStorageID(lineEBO));
}

void gui::updateLinePositions() 
{
	vao::uploadStorage(linePosBuffer, sizeof(glm::vec4) * allLineVertexPositions.size(), &allLineVertexPositions[0]);
}
void gui::updateLineBuffers()
{
	if (allLines.size())
	{
		//updateLinePositions();
		vao::uploadStorage(lineVBO, sizeof(glm::uvec2) * allLineVertices.size(), &allLineVertices[0]);
		vao::uploadStorage(lineEBO, sizeof(glm::uvec2) * allLines.size(), &allLines[0]);
	}
}

void gui::renderLines()
{
	glBindVertexArray(lineVAO);
	glDepthFunc(GL_ALWAYS);
	shader::use(lineShader);
	for (unsigned int m = 0; m < allLineGroups.size(); ++m)
	{
		if (lineGroupFlags[m])
		{
			glDrawElements(GL_LINES, allLineGroups[m].lineCount*2, GL_UNSIGNED_INT, (void*)(allLineGroups[m].lineOffset*2*sizeof(size_t)));
		}
	}
	shader::unuse();
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void gui::initLineShader() {
	lineShader = shader::newProgram("lineShader", shader::createModule("lineShader.vert"), shader::createModule("lineShader.frag"));
	shader::addVertexAttribute(lineShader, "vertex", 0);
}
void gui::setupLineShader()
{
	shader::bindUniformBufferToShader(lineShader, gl::generalUniformBuffer, "GeneralUniformBuffer");
	shader::bindUniformBufferToShader(lineShader, gui::constColorBuffer, "ColorBuffer");
	shader::bindUniformBufferToShader(lineShader, linePosBuffer, "PosBuffer");
}
