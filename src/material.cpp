#include "material.h"
#include "vao.h"
#include "shader.h"
#include "gldebug.h"
#include "mesh.h"

std::vector<mesh::Material> mesh::allMaterials;
std::vector<mesh::MaterialTextures> mesh::allMaterialTextures;
unsigned int mesh::materialUBO = 0;

void mesh::storeMaterials()
{
	materialUBO = vao::createStorage( "MaterialBuffer", sizeof( mesh::Material )*mesh::allMaterials.size(), &mesh::allMaterials[0], 0 );
	vao::bindStorage( GL_UNIFORM_BUFFER, materialUBO );
}





