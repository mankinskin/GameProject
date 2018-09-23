#include "physics.h"
#include "collision.h"
#include "nodes.h"
float physics::gravity_force = 9.81f;//m/s²

void physics::step()
{
  for (unsigned int i = 0; i < allBoundingSpherenodes.size(); ++i) {
	std::pair<unsigned int, unsigned int>& A = allBoundingSpherenodes[i];
	for (unsigned int j = i + 1; j < allBoundingSpherenodes.size(); ++j) {
	  std::pair<unsigned int, unsigned int>& B = allBoundingSpherenodes[j];
	  glm::vec3& A_pos = nodes::allPositions[A.first];
	  glm::vec3& B_pos = nodes::allPositions[B.first];
	  float dist = checkSphereIntersect(i, j);
	  glm::vec3 AtoB = glm::normalize(B_pos - A_pos) * dist;

	  if (dist < 0.0f) {
		printf("Spheres are intersecting by %7.5f/n", dist);
		A_pos += AtoB * (0.5f);
		B_pos += AtoB * -0.5f;
	  }
	}
  }
}
