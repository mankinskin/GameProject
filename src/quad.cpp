#include "Quad.h"
#include "shader.h"
#include "VAO.h"
#include <algorithm>
#include "Input.h"
#include "Mouse.h"
#include "glDebug.h"
#include "Framebuffer.h"

std::vector<glm::vec4> gui::allQuads;
size_t gui::quadBuffer;

size_t gui::quadIndexShader;
size_t gui::quadIndexVAO;
std::vector<size_t> quadIndexMap;
std::vector<float> quadDepthMap;


void gui::updateQuadBuffer()
{
	if (allQuads.size()) {
		vao::uploadStorage(quadBuffer, sizeof(glm::vec4)*allQuads.size(), &allQuads[0]);
	}
}

void gui::rasterQuadIndices()
{
	if (allQuads.size()) {
		glDepthMask(0);
		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(quadIndexVAO);
		shader::use(quadIndexShader);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allQuads.size());

		shader::unuse();
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glDepthMask(1);
	}
}


size_t gui::createQuad(float pPosX, float pPosY, float pWidth, float pHeight)
{
	allQuads.emplace_back(pPosX, pPosY, pWidth, pHeight);
	return allQuads.size();
}
size_t gui::createQuad(glm::vec4 pQuad)
{
	allQuads.push_back(pQuad);
	return allQuads.size();
}
void gui::reserveQuads(size_t pCount)
{
	allQuads.reserve(allQuads.size() + pCount);
}
void gui::readQuadIndexBuffer()
{
	//use pixel pack buffer
	glBindBuffer(GL_PIXEL_PACK_BUFFER, texture::quadIndexBuffer + 1);
	//get quad depth pixels
	glReadPixels(0, 0, gl::screenWidth, gl::screenHeight, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
void gui::initQuadBuffer()
{
	quadBuffer = vao::createStorage(MAX_QUAD_COUNT * sizeof(glm::vec4), 0, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	vao::createStream(quadBuffer, GL_MAP_WRITE_BIT);
	vao::bindStorage(GL_UNIFORM_BUFFER, quadBuffer);

	glCreateVertexArrays(1, &quadIndexVAO);
	glVertexArrayElementBuffer(quadIndexVAO, gl::quadEBO + 1);
	glVertexArrayVertexBuffer(quadIndexVAO, 0, gl::quadVBO + 1, 0, sizeof(glm::vec2));

	vao::setVertexAttrib(quadIndexVAO, 0, 0, 2, GL_FLOAT, 0);

	quadIndexMap.resize(gl::screenWidth * gl::screenHeight);
	quadDepthMap.resize(gl::screenWidth * gl::screenHeight);
}
void gui::initQuadIndexShader()
{
	quadIndexShader = shader::newProgram("quadIndexShader", shader::createModule("quadIndexShader.vert"), shader::createModule("quadIndexShader.frag"));
	shader::addVertexAttribute(quadIndexShader, "corner_pos", 0);
}

void gui::setupQuadIndexShader()
{
	shader::bindUniformBufferToShader(quadIndexShader, quadBuffer, "QuadBuffer");
}
void gui::clearQuads()
{
	allQuads.clear();
}
size_t gui::readQuadIndexMap(size_t pPos) {
	return *((size_t*)vao::getMappedPtr(texture::quadIndexBuffer) + pPos);
}
size_t gui::readQuadIndexMap(size_t pXPos, size_t pYPos) {
	return readQuadIndexMap((gl::screenWidth * pYPos) + pXPos);
}

float gui::readQuadDepthMap(size_t pPos)
{
	return quadDepthMap[pPos];
}

float gui::readQuadDepthMap(size_t pXPos, size_t pYPos)
{
	return readQuadDepthMap((gl::screenWidth * pYPos) + pXPos);
}
void gui::moveQuad(size_t pQuad, glm::vec2 pOffset) {
	allQuads[pQuad - 1] += glm::vec4(pOffset.x, pOffset.y, 0.0f, 0.0f);
}
void gui::resizeQuad(size_t pQuad, glm::vec2 pOffset) {
	allQuads[pQuad - 1] += glm::vec4(0.0f, 0.0f, pOffset.x, pOffset.y);
}

void gui::setQuadPos(size_t pQuad, glm::vec2 pPos)
{
	std::memcpy(&allQuads[pQuad - 1], &pPos, sizeof(glm::vec2));
}
