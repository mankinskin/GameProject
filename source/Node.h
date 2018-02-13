#pragma once
#include <vector>
#include <glm\glm.hpp>

//node
//node hold the information about the transformation of an object. They store a position, an orientation and a scale.
namespace node {

	void initNodeBuffers();
	void updateNodeBuffers();

	void updateNodeMatrices();
	void createNodes(size_t pCount, size_t* pnode);
	void createNode(size_t* pNode);
	void newNodeID(size_t & pNode);
	void reserveNodes(size_t pCount);

	void setPos(size_t pNodeID, glm::vec3 pPos);
	void setRotation(size_t pNodeID, glm::vec4 pRotation);
	void setScale(size_t pNodeID, glm::vec3 pScale);

	void translate(size_t pNodeID, glm::vec3 pPos);


	extern size_t nodeMatrixBuffer;

	const size_t MAX_NODES = 10000;
	extern size_t num_nodes;

	extern std::vector<glm::vec3> allPositions;
	extern std::vector<glm::vec3> allNormals;
	extern std::vector<glm::vec4> allRotations;

	extern std::vector<glm::vec3> allScales;
	extern std::vector<glm::mat4> allMatrices;


}
