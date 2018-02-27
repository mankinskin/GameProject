#include "entities.h"
#include <gtc/matrix_transform.hpp>
#include "vao.h"
#include "model.h"

std::vector<glm::mat4> entities::allMatrices;
std::vector<glm::vec3> entities::allPositions;
std::vector<glm::vec3> entities::allNormals;
std::vector<glm::vec3> entities::allScales;
std::vector<glm::vec4> entities::allRotations;

size_t entities::num_entities;
size_t entities::entityMatrixBuffer;


void entities::initEntityBuffers()
{
	entityMatrixBuffer = vao::createStorage(sizeof(glm::mat4)*MAX_ENTITIES, nullptr, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	
	vao::bindStorage(GL_UNIFORM_BUFFER, entityMatrixBuffer);
	vao::createStream(entityMatrixBuffer, GL_MAP_WRITE_BIT);
}

void entities::updateEntityBuffers()
{
	if(allMatrices.size()){
		vao::uploadStorage(entityMatrixBuffer, sizeof(glm::mat4)*allMatrices.size(), &allMatrices[0]);
	}
}


void entities::updateEntityMatrices()
{
	for (size_t e = 0; e < num_entities; ++e) {

		allMatrices[e] = glm::translate(glm::mat4(), allPositions[e]);
		allMatrices[e] = glm::scale(allMatrices[e], allScales[e]);
		glm::vec4& rot = allRotations[e];
		//allMatrices[e] = glm::rotate(allMatrices[e], rot.w*glm::pi<float>(), allNormals[e] + glm::vec3(rot));

	}
}

void entities::createEntities(size_t pCount, size_t * pEntityIDs)
{
	for (size_t p = 0; p < pCount; ++p) {
		*(pEntityIDs + p) = num_entities + p;
	}
	num_entities += pCount;
	allRotations.resize(num_entities);
	allNormals.resize(num_entities, glm::vec3(0.0f, 1.0f, 0.0f));
	allMatrices.resize(num_entities);
	allPositions.resize(num_entities);
	allScales.resize(num_entities);
}
void entities::createEntity(size_t* pNode) {
	newEntityID(*pNode);
	allNormals.resize(num_entities, glm::vec3(0.0f, 1.0f, 0.0f));
	allRotations.resize(num_entities);
	allMatrices.resize(num_entities);
	allPositions.resize(num_entities);
	allScales.resize(num_entities);
}

void entities::newEntityID(size_t& pNode)
{
	pNode = num_entities++;;
}

void entities::reserveEntities(size_t pCount)
{
	allNormals.reserve(num_entities + pCount);
	allRotations.resize(num_entities + pCount);
	allMatrices.resize(num_entities + pCount);
	allPositions.resize(num_entities + pCount);
	allScales.resize(num_entities + pCount);
}

void entities::setScale(size_t pNodeID, glm::vec3 pScale)
{
	allScales[pNodeID] = pScale;
}

void entities::setPos(size_t pNodeID, glm::vec3 pPos)
{
	allPositions[pNodeID] = pPos;
}
void entities::setRotation(size_t pNodeID, glm::vec4 pRotation)
{
	allRotations[pNodeID] += pRotation;
}
void entities::translate(size_t pNodeID, glm::vec3 pPos)
{
	allPositions[pNodeID] += pPos;
}
