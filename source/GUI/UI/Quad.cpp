#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "Quad.h"
#include "../../BaseGL/shader/shader.h"
#include "..\..\BaseGL\VAO.h"
#include <algorithm>
#include "../../Input/Input.h"
#include "../../Input/Mouse.h"
#include "../../GlobalGL/glDebug.h"
#include "../../BaseGL/Framebuffer.h"

std::vector<glm::vec4> gui::allQuads;
size_t gui::quadBuffer;

size_t gui::quadIndexShader = 0;
size_t gui::quadIndexVAO = 0;
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
		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(quadIndexVAO);
		shader::use(quadIndexShader);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allQuads.size());

		shader::unuse();
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glDebug::getGLError("rasterQuads()");
	}
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
	glBindBuffer(GL_PIXEL_PACK_BUFFER, texture::quadIndexBuffer + 1);

	glReadPixels(0, 0, gl::screenWidth, gl::screenHeight, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
	//glGetTextureImage(texture::quadIndexTexture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, gl::screenWidth * gl::screenHeight, &quadIndexMap[0]);
	//get quad depth pixels
	//glReadPixels(0, 0, gl::screenWidth, gl::screenHeight, GL_DEPTH_COMPONENT, GL_FLOAT, &quadDepthMap[0]);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glDebug::getGLError("fetchButtonMap():");
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


