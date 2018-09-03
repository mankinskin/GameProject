#include "physics.h"
#include "collision.h"
#include "entity.h"
float physics::gravity_force = 9.81f;//m/s�

void physics::step()
{
  for (unsigned int i = 0; i < allBoundingSpherenode.size(); ++i) {
	std::pair<unsigned int, unsigned int>& A = allBoundingSpherenode[i];
	for (unsigned int j = i + 1; j < allBoundingSpherenode.size(); ++j) {
	  std::pair<unsigned int, unsigned int>& B = allBoundingSpherenode[j];
	  glm::vec3& A_pos = entities::allPositions[A.first];
	  glm::vec3& B_pos = entities::allPositions[B.first];
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
