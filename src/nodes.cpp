#include "nodes.h"
#include "vao.h"
#include "model.h"

#include <gtc/matrix_transform.hpp>

std::vector<glm::mat4> nodes::allMatrices;
std::vector<glm::vec3> nodes::allPositions;

std::vector<glm::vec3> nodes::allNormals;
std::vector<glm::vec3> nodes::allScales;
std::vector<glm::vec4> nodes::allRotations;

unsigned int nodes::num_nodes;
gl::StreamStorage<glm::mat4> nodes::nodeMatrixBuffer;


glm::vec4 getTranslation(const glm::mat4& m)
{
  return m[3];
}
void nodes::initEntityBuffers()
{
  nodeMatrixBuffer = gl::StreamStorage<glm::mat4>("EntityMatrixBuffer", MAX_ENTITIES, GL_MAP_WRITE_BIT);
  nodeMatrixBuffer.setTarget(GL_UNIFORM_BUFFER);
}

void nodes::updateEntityBuffers()
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
	gl::uploadStorage(nodeMatrixBuffer, sizeof(glm::mat4)*allMatrices.size(), &allMatrices[0]);
  }
}

void nodes::updateEntityMatrices()
{
  for (unsigned int e = 0; e < num_nodes; ++e) {
	allMatrices[e] = glm::translate(glm::mat4(1.0f), allPositions[e]);
	allMatrices[e] = glm::scale(allMatrices[e], allScales[e]);
	//glm::vec4& rot = allRotations[e];
	//allMatrices[e] = glm::rotate(allMatrices[e], rot.w*glm::pi<float>(), allNormals[e] + glm::vec3(rot));
  }
}

void nodes::resizeEntities(const size_t n)
{
  allRotations.resize(num_nodes);
  allNormals.resize(num_nodes, glm::vec3(0.0f, 1.0f, 0.0f));
  allMatrices.resize(num_nodes, glm::mat4(1.0f));
  allPositions.resize(num_nodes);
  allScales.resize(num_nodes, glm::vec3(1.0f));
}
void nodes::createEntities(unsigned int pCount, unsigned int * pEntityIDs)
{
  for (unsigned int p = 0; p < pCount; ++p) {
	*(pEntityIDs + p) = num_nodes + p;
  }
  num_nodes += pCount;
  resizeEntities(num_nodes);
}

void nodes::createEntity(unsigned int* pNode)
{
  newEntityID(*pNode);
  resizeEntities(num_nodes);
}

void nodes::newEntityID(unsigned int& pNode)
{
  pNode = num_nodes++;;
}

void nodes::reserveEntities(unsigned int pCount)
{
  allNormals.reserve(num_nodes + pCount);
  allRotations.resize(num_nodes + pCount);
  allMatrices.resize(num_nodes + pCount);
  allPositions.resize(num_nodes + pCount);
  allScales.resize(num_nodes + pCount);
}

void nodes::setScale(unsigned int pNodeID, glm::vec3 pScale)
{
  allScales[pNodeID] = pScale;
}

void nodes::setPos(unsigned int pNodeID, glm::vec3 pPos)
{
  allPositions[pNodeID] = pPos;
}

void nodes::setRotation(unsigned int pNodeID, glm::vec4 pRotation)
{
  allRotations[pNodeID] += pRotation;
}

void nodes::translate(unsigned int pNodeID, glm::vec3 pPos)
{
  allPositions[pNodeID] += pPos;
}

