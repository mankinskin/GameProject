#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "Node.h"
#include <glm\gtc\matrix_transform.hpp>
#include "..\BaseGL\VAO.h"
#include "..\Model\Model.h"

std::vector<glm::mat4> node::allMatrices;
std::vector<glm::vec3> node::allPositions;
std::vector<glm::vec3> node::allNormals;
std::vector<glm::vec3> node::allScales;
std::vector<glm::vec4> node::allRotations;

size_t node::num_nodes;
size_t node::nodeMatrixBuffer;


void node::initNodeBuffers()
{
	nodeMatrixBuffer = vao::createStorage(sizeof(glm::mat4)*MAX_NODES, nullptr, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	
	vao::bindStorage(GL_UNIFORM_BUFFER, nodeMatrixBuffer);
	vao::createStream(nodeMatrixBuffer, GL_MAP_WRITE_BIT);
}

void node::updateNodeBuffers()
{
	if(allMatrices.size()){
		vao::uploadStorage(nodeMatrixBuffer, sizeof(glm::mat4)*allMatrices.size(), &allMatrices[0]);
	}
}


void node::updateNodeMatrices()
{
	for (size_t e = 0; e < num_nodes; ++e) {

		allMatrices[e] = glm::translate(glm::mat4(), allPositions[e]);
		allMatrices[e] = glm::scale(allMatrices[e], allScales[e]);
		glm::vec4& rot = allRotations[e];
		//allMatrices[e] = glm::rotate(allMatrices[e], rot.w*glm::pi<float>(), allNormals[e] + glm::vec3(rot));

	}
}

void node::createNodes(size_t pCount, size_t * pnode)
{
	size_t beg = num_nodes;
	for (size_t* pEnt = pnode; num_nodes < beg + pCount; ++pEnt) {
		newNodeID(*pEnt);
	}

	allRotations.resize(num_nodes);
	allNormals.resize(num_nodes, glm::vec3(0.0f, 1.0f, 0.0f));
	allMatrices.resize(num_nodes);
	allPositions.resize(num_nodes);
	allScales.resize(num_nodes);
}
void node::createNode(size_t* pNode) {
	newNodeID(*pNode);
	allNormals.resize(num_nodes, glm::vec3(0.0f, 1.0f, 0.0f));
	allRotations.resize(num_nodes);
	allMatrices.resize(num_nodes);
	allPositions.resize(num_nodes);
	allScales.resize(num_nodes);
}

void node::newNodeID(size_t& pNode)
{
	pNode = num_nodes++;;
}

void node::setScale(size_t pNodeID, glm::vec3 pScale)
{
	allScales[pNodeID] = pScale;
}

void node::setPos(size_t pNodeID, glm::vec3 pPos)
{
	allPositions[pNodeID] = pPos;
}
void node::setRotation(size_t pNodeID, glm::vec4 pRotation)
{
	allRotations[pNodeID] += pRotation;
}
void node::translate(size_t pNodeID, glm::vec3 pPos)
{
	allPositions[pNodeID] += pPos;
}
