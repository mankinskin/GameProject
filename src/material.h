#pragma once
#include <glm.hpp>
#include <vector>

namespace mesh {
	struct Material {
		glm::vec4 amb_color;
		glm::vec4 diff_color;
		glm::vec4 spec_color;
	};
	struct MaterialTextures {
		size_t amb_tex;
		size_t diff_tex;
		size_t spec_tex;
	};
	extern std::vector<Material> allMaterials;
	extern std::vector<MaterialTextures> allMaterialTextures;
	extern size_t materialUBO;
	void storeMaterials();

}

