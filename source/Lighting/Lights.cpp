#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Lights.h"
#include "../BaseGL/VAO.h"
#include "../BaseGL/shader/shader.h"
#include "../GlobalGL/gl.h"
#include "../BaseGL/Framebuffer.h"
#include "../GlobalGL/glDebug.h"
#include "../BaseGL/texture.h"

std::vector<glm::vec4> lighting::allLightData;
std::vector<lighting::LightIndexRange> lighting::allLightIndexRanges;
size_t lighting::lightVAO = 0;
size_t lighting::lightIndexVBO = 0;
size_t lighting::lightDataUBO = 0;
size_t lighting::lightShaderProgram = 0;
size_t lighting::MAX_LIGHT_COUNT = 100;

void lighting::initLighting()
{
	lighting::initLightVAO();
	lighting::initLightDataBuffer();
}

void lighting::initLightVAO() {

	lightIndexVBO = vao::createStorage(MAX_LIGHT_COUNT * sizeof(LightIndexRange), 0, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
	vao::createStream(lightIndexVBO, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &lightVAO);
	glVertexArrayVertexBuffer(lightVAO, 0, gl::quadVBO + 1, 0, sizeof(float) * 2);
	glVertexArrayElementBuffer(lightVAO, gl::quadEBO + 1);
	vao::setVertexArrayVertexStorage(lightVAO, 1, lightIndexVBO, sizeof(size_t) * 2);
	glVertexArrayBindingDivisor(lightVAO, 1, 1);
	vao::setVertexAttrib(lightVAO, 0, 0, 2, GL_FLOAT, 0);
	vao::setVertexAttrib(lightVAO, 1, 1, 2, GL_UNSIGNED_INT, 0);

}

void lighting::initLightDataBuffer() {

	lightDataUBO = vao::createStorage(MAX_LIGHT_COUNT * sizeof(glm::vec4) * 3, 0, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
	vao::createStream(lightDataUBO, GL_MAP_WRITE_BIT);
	vao::bindStorage(GL_UNIFORM_BUFFER, lightDataUBO);
}

void lighting::updateLightDataBuffer() {
	if (allLightData.size()) {
		vao::uploadStorage(lightDataUBO, allLightData.size() * sizeof(glm::vec4), &allLightData[0]);
	}
}

void lighting::updateLightIndexRangeBuffer() {
	if (allLightIndexRanges.size()) {
		vao::uploadStorage(lightIndexVBO, allLightIndexRanges.size() * sizeof(LightIndexRange), &allLightIndexRanges[0]);
	}
}
void lighting::initLightShader() {
	lightShaderProgram = shader::newProgram("lightShaderProgram", shader::createModule("lightShaderProgram.vert"), shader::createModule("lightShaderProgram.frag"));
	shader::addVertexAttribute(lightShaderProgram, "corner_pos", 0);
	shader::addVertexAttribute(lightShaderProgram, "index_range", 1);
}

size_t lighting::createLight(glm::vec4& pPos, glm::vec4& pColor) {
	allLightIndexRanges.emplace_back(allLightData.size(), 2);
	allLightData.push_back(pPos);
	allLightData.push_back(pColor);
	return allLightIndexRanges.size() - 1;
}

size_t lighting::createLight(glm::vec4& pPos, glm::vec4& pColor, glm::vec4& pFrustum) {
	allLightIndexRanges.emplace_back(allLightData.size(), 3);
	allLightData.push_back(pPos);
	allLightData.push_back(pColor);
	allLightData.push_back(pFrustum);
	return allLightIndexRanges.size() - 1;
}
void lighting::reserveLightSpace(size_t pCount) {
	allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
	allLightData.reserve(allLightData.capacity() + pCount * 3);
}
void lighting::reservePointLightSpace(size_t pCount) {
	allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
	allLightData.reserve(allLightData.capacity() + pCount * 2);
}
void lighting::renderLights()
{
	
	//glDepthMask(0);
	//glBlendFunc(GL_ONE, GL_ZERO);
	//glDepthFunc(GL_LEQUAL);
	glBindVertexArray(lightVAO);
	shader::use(lightShaderProgram);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture::gAmbientTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture::gDiffuseTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture::gSpecularTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture::gPosTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texture::gNormalTexture);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, allLightIndexRanges.size());

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader::unuse();
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	glDepthMask(1);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, texture::gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, int(gl::screenWidth * gl::resolution), int(gl::screenHeight * gl::resolution), 0, 0, gl::screenWidth, gl::screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDebug::getGLError("renderLights()4");
}
void lighting::setupLightShader()
{
	shader::bindUniformBufferToShader(lightShaderProgram, lightDataUBO, "LightDataBuffer");
	shader::bindUniformBufferToShader(lightShaderProgram, gl::generalUniformBuffer, "GeneralUniformBuffer");
}

void lighting::setLightPos(size_t pLightIndex, glm::vec3& pPos) {
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset], &pPos, sizeof(float) * 3);
}
void lighting::setLightPos(size_t pLightIndex, glm::vec4& pPos) {
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset], &pPos, sizeof(float) * 4);
}
void lighting::setLightColor(size_t pLightIndex, glm::vec3& pColor) {
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset + 1], &pColor, sizeof(float) * 3);
}
void lighting::setLightColor(size_t pLightIndex, glm::vec4& pColor) {
	std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset + 1], &pColor, sizeof(float) * 4);
}

glm::vec4& lighting::getLightColor(size_t pLightIndex) {
	return allLightData[allLightIndexRanges[pLightIndex].offset + 1];
}
