#pragma once
#include "../../Graphics/Mesh/Mesh.h"
#include "physics.h"
#include <glm\glm.hpp>
namespace physics {
	struct BoundingSphere {
		BoundingSphere(glm::vec3 pLocalPos, float pRadius)
			:local_pos(pLocalPos), radius(pRadius) {}
		glm::vec3 local_pos;
		float radius;
	};
	extern std::vector<std::pair<size_t, size_t>> allBoundingSpherenode;//access this with a Bounding link index, first element of pair is an index to the node of a boundingsphere second is the bounding sphere index
	extern std::vector<BoundingSphere> allBoundingSpheres;
	size_t createBoundingSphere(BoundingSphere pBoundingSphere);

	size_t linkNodeToBoundingSphere(size_t pNodeIndex, BoundingSphere pBoundingSphere);
	size_t linkNodeToBoundingSphere(size_t pNodeIndex, size_t pBoundingSphereIndex);

	float checkTriangleIntersect(size_t pIndexOffset_A, size_t pEntity_A, size_t pIndexOffset_B, size_t pEntity_B);
	float checkSphereIntersect(size_t pSphereA, size_t pSphereB);


	template<class PrimA, class PrimB>
	float checkIntersect(size_t pNode_A, size_t pNode_B, PrimA pA, PrimB pB)
	{
		return 0.0f;
	}
}