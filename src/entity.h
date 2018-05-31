#pragma once
#include "storage.h"
#include <vector>
#include <glm.hpp>

//node
//node hold the information about the transformation of an object. They store a position, an orientation and a scale.
namespace entities 
{
	void initEntityBuffers();
	void updateEntityBuffers();

	void updateEntityMatrices();
	void createEntities( unsigned int pCount, unsigned int* pEntityIDs );
	void createEntity( unsigned int* pEntityID );
	void newEntityID( unsigned int & pNode );
	void reserveEntities( unsigned int pCount );

	void setPos( unsigned int pNodeID, glm::vec3 pPos );
	void setRotation( unsigned int pNodeID, glm::vec4 pRotation );
	void setScale( unsigned int pNodeID, glm::vec3 pScale );

	void translate( unsigned int pNodeID, glm::vec3 pPos );
	

	extern gl::StreamStorage<glm::mat4> entityMatrixBuffer;

	const unsigned int MAX_ENTITIES = 1000;
	extern unsigned int num_entities;

	extern std::vector<glm::vec3> allPositions;
	extern std::vector<glm::vec3> allNormals;
	extern std::vector<glm::vec4> allRotations;

	extern std::vector<glm::vec3> allScales;
	extern std::vector<glm::mat4> allMatrices;


}
