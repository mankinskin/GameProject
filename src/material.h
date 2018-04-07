#pragma once
#include <glm.hpp>
#include <vector>
#include "storage.h"

namespace mesh {
	struct Material {
		glm::vec4 amb_color;
		glm::vec4 diff_color;
		glm::vec4 spec_color;
	};
	struct MaterialTextures {
		unsigned int amb_tex;
		unsigned int diff_tex;
		unsigned int spec_tex;
	};
	extern std::vector<Material> allMaterials;
	extern std::vector<MaterialTextures> allMaterialTextures;
	extern gl::Storage materialUBO;
	void storeMaterials();

}

