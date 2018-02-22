#include "Collision.h"
#include "Entities.h"
#include "Mesh.h"

std::vector<physics::BoundingSphere> physics::allBoundingSpheres;
std::vector<std::pair<size_t, size_t>> physics::allBoundingSpherenode;
size_t physics::createBoundingSphere(BoundingSphere pBoundingSphere)
{
	allBoundingSpheres.push_back(pBoundingSphere);
	return allBoundingSpheres.size() - 1;
}


size_t physics::linkNodeToBoundingSphere(size_t pNodeIndex, BoundingSphere pBoundingSphere)
{
	size_t bs_index = allBoundingSpheres.size();
	allBoundingSpheres.push_back(pBoundingSphere);
	return linkNodeToBoundingSphere(pNodeIndex, bs_index);
}

size_t physics::linkNodeToBoundingSphere(size_t pNodeIndex, size_t pBoundingSphereIndex)
{
	allBoundingSpherenode.emplace_back(pNodeIndex, pBoundingSphereIndex);
	return allBoundingSpherenode.size() - 1;
}
float physics::checkTriangleIntersect(size_t pIndexOffset_A, size_t pEntity_A, size_t pIndexOffset_B, size_t pEntity_B)
{
	using namespace mesh;
	glm::uvec3 face_A;
	glm::uvec3 face_B;
	glm::vec3 A_verts[3];
	glm::vec3 A_edge_vectors[3];
	
	glm::vec3 B_verts[3];
	glm::vec3 B_edge_vectors[3];

	glm::mat4 A_transform = entities::allMatrices[pEntity_A];
	glm::mat4 B_transform = entities::allMatrices[pEntity_B];

	for (size_t i = 0; i < 3; ++i) {
		face_A[i] = allIndices[pIndexOffset_A + i];
		face_B[i] = allIndices[pIndexOffset_B + i];
		
		A_verts[i] = allStaticVertices[face_A[i]].pos;
		B_verts[i] = allStaticVertices[face_B[i]].pos;

		A_edge_vectors[i] += A_verts[i];
		A_edge_vectors[(2 + i) % 3] -= A_verts[i];
		
		B_edge_vectors[i] += B_verts[i];
		B_edge_vectors[(2 + i) % 3] -= B_verts[i];
	}

	glm::vec3 n_A = glm::cross(A_edge_vectors[0], -A_edge_vectors[2]);
	glm::vec3 n_B = glm::cross(B_edge_vectors[0], -B_edge_vectors[2]);

	//calc A edge distance to B
	glm::vec3 distances[3] = { glm::vec3(), glm::vec3(), glm::vec3() };
	
	
	for (size_t i = 0; i < 3; ++i) {
		distances[i] = n_B * A_verts[i] + (-n_B + B_verts[0]);
		
	}
	return false;
}

float physics::checkSphereIntersect(size_t pSphereA, size_t pSphereB)
{
	std::pair<size_t, size_t> node_link_A = allBoundingSpherenode[pSphereA];
	std::pair<size_t, size_t> node_link_B = allBoundingSpherenode[pSphereB];
	BoundingSphere& A = allBoundingSpheres[node_link_A.second];
	BoundingSphere& B = allBoundingSpheres[node_link_B.second];

	glm::vec3 pos_A = A.local_pos + entities::allPositions[node_link_A.first];
	glm::vec3 pos_B = B.local_pos + entities::allPositions[node_link_B.first];
	return glm::length(pos_A - pos_B) - (A.radius + B.radius);//returns the distance between both bounding spheres (if negative, the spheres intersect by that distance)
}
