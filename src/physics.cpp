#include "physics.h"
#include "collision.h"
#include "nodes.h"
float physics::gravity_force = 9.81f;//m/s²

void physics::step()
{
  for (unsigned int i = 0; i < allBoundingSphereNodes.size(); ++i) {
	std::pair<nodes::NodeID, size_t>& A = allBoundingSphereNodes[i];
	for (unsigned int j = i + 1; j < allBoundingSphereNodes.size(); ++j) {
	  std::pair<nodes::NodeID, size_t>& B = allBoundingSphereNodes[j];
	  const glm::vec3 A_pos = A.first->getPos();
	  const glm::vec3 B_pos = B.first->getPos();
	  float dist = checkSphereIntersect(i, j);

	  if (dist < 0.0f) {
		printf("Spheres are intersecting by %7.5f/n", dist);
		A.first->move(glm::normalize(B_pos - A_pos) * dist * 0.5f);
		A.first->move(glm::normalize(B_pos - A_pos) * dist * -0.5f);
	  }
	}
  }
}
