#include "lights.h"
#include "vao.h"
#include "shader.h"
#include "gl.h"
#include "framebuffer.h"
#include "gldebug.h"
#include "voxelization.h"

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

void lighting::initLightVAO() 
{
	lightIndexVBO = vao::createStorage(MAX_LIGHT_COUNT * sizeof(LightIndexRange), 0, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
	vao::createStream(lightIndexVBO, GL_MAP_WRITE_BIT);
	
	glCreateVertexArrays(1, &lightVAO);
	glVertexArrayVertexBuffer(lightVAO, 0, gl::quadVBO + 1, 0, sizeof(float) * 2);
	glVertexArrayElementBuffer(lightVAO, gl::quadEBO + 1);
	vao::setVertexArrayVertexStorage(lightVAO, 1, lightIndexVBO, sizeof(size_t) * 2);
	glVertexArrayVertexBuffer(lightVAO, 1, lightIndexVBO+1, 0, sizeof(size_t) * 2);
	glVertexArrayBindingDivisor(lightVAO, 1, 1);

	vao::setVertexAttrib(lightVAO, 0, 0, 2, GL_FLOAT, 0);
	vao::setVertexAttrib(lightVAO, 1, 1, 2, GL_UNSIGNED_INT, 0);

}

void lighting::initLightDataBuffer() 
{
	lightDataUBO = vao::createStorage(MAX_LIGHT_COUNT * sizeof(glm::vec4) * 3, 0, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
	vao::createStream(lightDataUBO, GL_MAP_WRITE_BIT);
	vao::bindStorage(GL_UNIFORM_BUFFER, lightDataUBO);
}

void lighting::updateLightDataBuffer() 
{
	if (allLightData.size())
	{
		vao::uploadStorage(lightDataUBO, allLightData.size() * sizeof(glm::vec4), &allLightData[0]);
	}
}

void lighting::updateLightIndexRangeBuffer() 
{
	if (allLightIndexRanges.size()) 
	{
		vao::uploadStorage(lightIndexVBO, allLightIndexRanges.size() * sizeof(LightIndexRange), &allLightIndexRanges[0]);
	}
}
void lighting::initLightShader()
{
	lightShaderProgram = shader::newProgram("lightShaderProgram", shader::createModule("lightShaderProgram.vert"), shader::createModule("lightShaderProgram.frag"));
	shader::addVertexAttribute(lightShaderProgram, "corner_pos", 0);
	shader::addVertexAttribute(lightShaderProgram, "index_range", 1);
}

size_t lighting::createLight(glm::vec4 pPos, glm::vec4 pColor)
{
	allLightIndexRanges.emplace_back(allLightData.size(), 2);
	allLightData.push_back(pPos);
	allLightData.push_back(pColor);
	return allLightIndexRanges.size() - 1;
}

size_t lighting::createLight(glm::vec4 pPos, glm::vec4 pColor, glm::vec4 pFrustum)
{
	allLightIndexRanges.emplace_back(allLightData.size(), 3);
	allLightData.push_back(pPos);
	allLightData.push_back(pColor);
	allLightData.push_back(pFrustum);
	return allLightIndexRanges.size() - 1;
}
void lighting::reserveLightSpace(size_t pCount) 
{
	allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
	allLightData.reserve(allLightData.capacity() + pCount * 3);
}
void lighting::reservePointLightSpace(size_t pCount) 
{
	allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
	allLightData.reserve(allLightData.capacity() + pCount * 2);
}
void lighting::renderLights()
{
	glViewport(0, 0, gl::screenWidth, gl::screenHeight);
	glDepthMask(0);
	//glDisable(GL_CULL_FACE);
	glBindVertexArray(lightVAO);
	shader::use(lightShaderProgram);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//mix colors
	glDepthFunc(GL_ALWAYS);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture::gAmbientTexture);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture::gDiffuseTexture);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, texture::gSpecularTexture);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, texture::gPosTexture);
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, texture::gNormalTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxelization::volumeImage);
	glBindImageTexture(0, voxelization::volumeImage, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 100);

	glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindTexture(GL_TEXTURE_3D, 0);
	shader::unuse();
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	glDepthMask(1); 
	glViewport(0, 0, gl::screenWidth, gl::screenHeight);
}
void lighting::setupLightShader()
{
	//shader::bindUniformBufferToShader(lightShaderProgram, lightDataUBO, "LightDataBuffer");
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
