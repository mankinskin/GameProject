#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "Material.h"
#include "..\BaseGL\VAO.h"
#include "../BaseGL/shader/shader.h"
#include "../GlobalGL/glDebug.h"
#include "Mesh.h"

std::vector<mesh::Material> mesh::allMaterials;
std::vector<mesh::MaterialTextures> mesh::allMaterialTextures;
size_t mesh::materialUBO = 0;

void mesh::storeMaterials()
{
	materialUBO = vao::createStorage(sizeof(mesh::Material)*mesh::allMaterials.size(), &mesh::allMaterials[0], 0);
	vao::bindStorage(GL_UNIFORM_BUFFER, materialUBO);
}





