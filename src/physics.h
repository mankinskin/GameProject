#pragma once
#include <vector>
#include <glm.hpp>

namespace physics
{
  extern float gravity_force;

  void step();

  extern std::vector<glm::vec4> allVelocities;//vec3 normalized vector for direction + scalar for kenetic energy
}
