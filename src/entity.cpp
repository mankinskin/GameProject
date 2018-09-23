#include "entity.h"
#include "vao.h"
#include "model.h"

#include <gtc/matrix_transform.hpp>

std::vector<glm::mat4> entities::allMatrices;
std::vector<glm::vec3> entities::allPositions;
std::vector<glm::vec3> entities::allNormals;
std::vector<glm::vec3> entities::allScales;
std::vector<glm::vec4> entities::allRotations;

unsigned int entities::num_entities;
gl::StreamStorage<glm::mat4> entities::entityMatrixBuffer;


void entities::initEntityBuffers()
{
  entityMatrixBuffer = gl::StreamStorage<glm::mat4>("EntityMatrixBuffer", MAX_ENTITIES, GL_MAP_WRITE_BIT);
  entityMatrixBuffer.setTarget(GL_UNIFORM_BUFFER);
}

void entities::updateEntityBuffers()
{
  //printf("Entities: %lu\n", allMatrices.size());
  //for (size_t m = 0; m < allMatrices.size(); ++m) {
  //  const glm::mat4& mat = allMatrices[m];
  //  printf("Mat %lu\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\n", m,
  //  	mat[0][0], mat[0][1], mat[0][2], mat[0][3],
  //  	mat[1][0], mat[1][1], mat[1][2], mat[1][3],
  //  	mat[2][0], mat[2][1], mat[2][2], mat[2][3],
  //  	mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
  //}
  if (allMatrices.size()) {
	gl::uploadStorage(entityMatrixBuffer, sizeof(glm::mat4)*allMatrices.size(), &allMatrices[0]);
  }
}

void entities::updateEntityMatrices()
{
  for (unsigned int e = 0; e < num_entities; ++e) {
	allMatrices[e] = glm::translate(glm::mat4(1.0f), allPositions[e]);
	allMatrices[e] = glm::scale(allMatrices[e], allScales[e]);
	//glm::vec4& rot = allRotations[e];
	//allMatrices[e] = glm::rotate(allMatrices[e], rot.w*glm::pi<float>(), allNormals[e] + glm::vec3(rot));
  }
}

void entities::resizeEntities(const size_t n)
{
  allRotations.resize(num_entities);
  allNormals.resize(num_entities, glm::vec3(0.0f, 1.0f, 0.0f));
  allMatrices.resize(num_entities, glm::mat4(1.0f));
  allPositions.resize(num_entities);
  allScales.resize(num_entities, glm::vec3(1.0f));
}
void entities::createEntities(unsigned int pCount, unsigned int * pEntityIDs)
{
  for (unsigned int p = 0; p < pCount; ++p) {
	*(pEntityIDs + p) = num_entities + p;
  }
  num_entities += pCount;
  resizeEntities(num_entities);
}

void entities::createEntity(unsigned int* pNode)
{
  newEntityID(*pNode);
  resizeEntities(num_entities);
}

void entities::newEntityID(unsigned int& pNode)
{
  pNode = num_entities++;;
}

void entities::reserveEntities(unsigned int pCount)
{
  allNormals.reserve(num_entities + pCount);
  allRotations.resize(num_entities + pCount);
  allMatrices.resize(num_entities + pCount);
  allPositions.resize(num_entities + pCount);
  allScales.resize(num_entities + pCount);
}

void entities::setScale(unsigned int pNodeID, glm::vec3 pScale)
{
  allScales[pNodeID] = pScale;
}

void entities::setPos(unsigned int pNodeID, glm::vec3 pPos)
{
  allPositions[pNodeID] = pPos;
}

void entities::setRotation(unsigned int pNodeID, glm::vec4 pRotation)
{
  allRotations[pNodeID] += pRotation;
}

void entities::translate(unsigned int pNodeID, glm::vec3 pPos)
{
  allPositions[pNodeID] += pPos;
}

