#pragma once
#include <vector>
#include <glm\glm.hpp>

//node
//node hold the information about the transformation of an object. They store a position, an orientation and a scale.
namespace entities {

	void initEntityBuffers();
	void updateEntityBuffers();

	void updateEntityMatrices();
	void createEntities(size_t pCount, size_t* pEntityIDs);
	void createEntity(size_t* pEntityID);
	void newEntityID(size_t & pNode);
	void reserveEntities(size_t pCount);

	void setPos(size_t pNodeID, glm::vec3 pPos);
	void setRotation(size_t pNodeID, glm::vec4 pRotation);
	void setScale(size_t pNodeID, glm::vec3 pScale);

	void translate(size_t pNodeID, glm::vec3 pPos);
	

	extern size_t entityMatrixBuffer;

	const size_t MAX_ENTITIES = 1000;
	extern size_t num_entities;

	extern std::vector<glm::vec3> allPositions;
	extern std::vector<glm::vec3> allNormals;
	extern std::vector<glm::vec4> allRotations;

	extern std::vector<glm::vec3> allScales;
	extern std::vector<glm::mat4> allMatrices;


}
