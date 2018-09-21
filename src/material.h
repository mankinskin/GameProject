#pragma once
#include <glm.hpp>
#include <vector>
#include "storage.h"
#include "utils/id.h"

namespace mesh
{
  struct Material
  {
	using Container = utils::Container<Material>;
	using ID = typename Container::ID;
	static Container all;
	glm::vec4 amb_color;
	glm::vec4 diff_color;
	glm::vec4 spec_color;
  };
  struct MaterialTextures
  {
	using Container = utils::Container<MaterialTextures>;
	using ID = typename Container::ID;
	static Container all;
	unsigned int amb_tex;
	unsigned int diff_tex;
	unsigned int spec_tex;
  };
  extern gl::Storage<Material> materialUBO;
  void storeMaterials();
}

