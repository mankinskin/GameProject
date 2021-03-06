#include "light.h"
#include "vao.h"
#include "shader.h"
#include "gl.h"
#include "framebuffer.h"
#include "gldebug.h"
#include "voxelization.h"
#include <cstring>
#include "primitives.h"
#include "viewport.h"

std::vector<glm::vec4> lights::allLightData;
std::vector<lights::LightIndexRange> lights::allLightIndexRanges;
gl::VAO lights::lightVAO;
gl::StreamStorage<lights::LightIndexRange> lights::lightIndexVBO;
gl::StreamStorage<glm::vec4> lights::lightDataUBO;
static shader::Program lightShaderProgram;

void lights::initLights()
{
  createLightVBO();
  createLightVAO();
  createLightDataBuffer();
}

void lights::createLightVBO()
{
  lightIndexVBO = gl::StreamStorage<LightIndexRange>("LightIndexBuffer", MAX_LIGHT_COUNT, GL_MAP_WRITE_BIT);
}

void lights::createLightVAO()
{
  lightVAO = gl::VAO("lightVAO");
  glVertexArrayVertexBuffer(lightVAO, 0, gl::quadVBO.ID, 0, sizeof(float) * 2);
  glVertexArrayElementBuffer(lightVAO, gl::quadEBO.ID);

  //gl::setVertexArrayVertexStorage(lightVAO, 1, lightIndexVBO.ID, sizeof(unsigned int) * 2);
  glVertexArrayVertexBuffer(lightVAO, 1, lightIndexVBO.ID, 0, sizeof(unsigned int) * 2);

  glVertexArrayBindingDivisor(lightVAO, 1, 1);

  //gl::setVertexAttrib(lightVAO, 0, 0, 2, GL_FLOAT, 0);
  //gl::setVertexAttrib(lightVAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
}

void lights::createLightDataBuffer()
{
  lightDataUBO = gl::StreamStorage<glm::vec4>("LightDataBuffer", MAX_LIGHT_COUNT * 3, GL_MAP_WRITE_BIT);
  lightDataUBO.setTarget(GL_UNIFORM_BUFFER);
}

void lights::updateLightDataBuffer()
{
  if (allLightData.size()) {
	//gl::uploadStorage(lightDataUBO, allLightData.size() * sizeof(glm::vec4), &allLightData[0]);
  }
}

void lights::updateLightIndexRangeBuffer()
{
  if (allLightIndexRanges.size()) {
	//gl::uploadStorage(lightIndexVBO, allLightIndexRanges.size() * sizeof(LightIndexRange), &allLightIndexRanges[0]);
  }
}
void lights::initLightShader()
{
  lightShaderProgram = shader::Program("lightShaderProgram", shader::Stage("lightShaderProgram.vert"), shader::Stage("lightShaderProgram.frag"));
  lightShaderProgram.addVertexAttribute("corner_pos", 0);
  lightShaderProgram.addVertexAttribute("index_range", 1);
}

unsigned int lights::createLight(glm::vec4 pPos, glm::vec4 pColorData)
{
  allLightIndexRanges.emplace_back(allLightData.size(), 2);
  allLightData.push_back(pPos);
  allLightData.push_back(pColorData);
  return allLightIndexRanges.size() - 1;
}

unsigned int lights::createLight(glm::vec4 pPos, glm::vec4 pColorData, glm::vec4 pFrustum)
{
  allLightIndexRanges.emplace_back(allLightData.size(), 3);
  allLightData.push_back(pPos);
  allLightData.push_back(pColorData);
  allLightData.push_back(pFrustum);
  return allLightIndexRanges.size() - 1;
}

void lights::reserveLightSpace(unsigned int pCount)
{
  allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
  allLightData.reserve(allLightData.capacity() + pCount * 3);
}

void lights::reservePointLightSpace(unsigned int pCount)
{
  allLightIndexRanges.reserve(allLightIndexRanges.capacity() + pCount);
  allLightData.reserve(allLightData.capacity() + pCount * 2);
}

void lights::renderLights()
{
  glViewport(0, 0, gl::Viewport::current->width, gl::Viewport::current->height);
  glDepthMask(0);
  //glDisable(GL_CULL_FACE);
  glBindVertexArray(lightVAO);
  lightShaderProgram.use();
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
  shader::Program::unuse();
  glBindVertexArray(0);
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LESS);
  glDepthMask(1);
  glViewport(0, 0, gl::Viewport::current->width, gl::Viewport::current->height);
}

void lights::setupLightShader()
{
  //lightShaderProgram.bindUniformBuffer(lightDataUBO, "LightDataBuffer");
  lightShaderProgram.bindUniformBuffer(gl::generalUniformBuffer, "GeneralUniformBuffer");
}

void lights::setLightPos(unsigned int pLightIndex, glm::vec3& pPos)
{
  std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset], &pPos, sizeof(float) * 3);
}

void lights::setLightPos(unsigned int pLightIndex, glm::vec4& pPos)
{
  std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset], &pPos, sizeof(float) * 4);
}

void lights::setLightColor(unsigned int pLightIndex, glm::vec3& pColorData)
{
  std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset + 1], &pColorData, sizeof(float) * 3);
}

void lights::setLightColor(unsigned int pLightIndex, glm::vec4& pColorData)
{
  std::memcpy(&allLightData[allLightIndexRanges[pLightIndex].offset + 1], &pColorData, sizeof(float) * 4);
}

glm::vec4& lights::getLightColor(unsigned int pLightIndex)
{
  return allLightData[allLightIndexRanges[pLightIndex].offset + 1];
}

