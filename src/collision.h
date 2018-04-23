#pragma once
#include "Mesh.h"
#include "physics.h"
#include <glm\glm.hpp>

namespace physics {
	struct BoundingSphere {
		BoundingSphere( glm::vec3 pLocalPos, float pRadius )
			:local_pos( pLocalPos ), radius( pRadius ) {}
		glm::vec3 local_pos;
		float radius;
	};
	extern std::vector<std::pair<unsigned int, unsigned int>> allBoundingSpherenode;//access this with a Bounding link index, first element of pair is an index to the node of a boundingsphere second is the bounding sphere index
	extern std::vector<BoundingSphere> allBoundingSpheres;
	unsigned int createBoundingSphere( BoundingSphere pBoundingSphere );

	unsigned int linkNodeToBoundingSphere( unsigned int pNodeIndex, BoundingSphere pBoundingSphere );
	unsigned int linkNodeToBoundingSphere( unsigned int pNodeIndex, unsigned int pBoundingSphereIndex );

	float checkTriangleIntersect( unsigned int pIndexOffset_A, unsigned int pEntity_A, unsigned int pIndexOffset_B, unsigned int pEntity_B );
	float checkSphereIntersect( unsigned int pSphereA, unsigned int pSphereB );


	template<class PrimA, class PrimB>
	float checkIntersect( unsigned int pNode_A, unsigned int pNode_B, PrimA pA, PrimB pB )
	{
		return 0.0f;
	}
}