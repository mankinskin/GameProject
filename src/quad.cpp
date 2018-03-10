#include "quad.h"
#include "shader.h"
#include "vao.h"
#include <algorithm>
#include "input.h"
#include "mouse.h"
#include "gldebug.h"
#include "framebuffer.h"
#include "viewport.h"
#include "primitives.h"

std::vector<glm::vec4> gui::allQuads;
unsigned int gui::quadBuffer;
unsigned int gui::quadIndexShader;
unsigned int gui::quadIndexVAO;
std::vector<unsigned int> quadIndexMap;
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


unsigned int gui::createQuad(float pPosX, float pPosY, float pWidth, float pHeight)
{
	allQuads.emplace_back(pPosX, pPosY, pWidth, pHeight);
	return allQuads.size();
}
unsigned int gui::createQuad(glm::vec4 pQuad)
{
	allQuads.push_back(pQuad);
	return allQuads.size();
}
void gui::reserveQuads(unsigned int pCount)
{
	allQuads.reserve(allQuads.size() + pCount);
}
void gui::readQuadIndexBuffer()
{
	//use pixel pack buffer
	glBindBuffer(GL_PIXEL_PACK_BUFFER, texture::quadIndexBuffer + 1);
	//get quad depth pixels
	glReadPixels(0, 0, gl::Viewport::current->width, gl::Viewport::current->height, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);

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

	quadIndexMap.resize(gl::Viewport::current->width * gl::Viewport::current->height);
	quadDepthMap.resize(gl::Viewport::current->width * gl::Viewport::current->height);
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
unsigned int gui::readQuadIndexMap(unsigned int pPos) {
	return *((unsigned int*)vao::getMappedPtr(texture::quadIndexBuffer) + pPos);
}
unsigned int gui::readQuadIndexMap(unsigned int pXPos, unsigned int pYPos) {
	return readQuadIndexMap((gl::Viewport::current->width * pYPos) + pXPos);
}

float gui::readQuadDepthMap(unsigned int pPos)
{
	return quadDepthMap[pPos];
}

float gui::readQuadDepthMap(unsigned int pXPos, unsigned int pYPos)
{
	return readQuadDepthMap((gl::Viewport::current->width * pYPos) + pXPos);
}
void gui::moveQuad(unsigned int pQuad, glm::vec2 pOffset) {
	allQuads[pQuad - 1] += glm::vec4(pOffset.x, pOffset.y, 0.0f, 0.0f);
}
void gui::resizeQuad(unsigned int pQuad, glm::vec2 pOffset) {
	allQuads[pQuad - 1] += glm::vec4(0.0f, 0.0f, pOffset.x, pOffset.y);
}

void gui::setQuadPos(unsigned int pQuad, glm::vec2 pPos)
{
	std::memcpy(&allQuads[pQuad - 1], &pPos, sizeof(glm::vec2));
}
