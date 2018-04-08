#include "material.h"
#include "vao.h"
#include "shader.h"
#include "gldebug.h"
#include "mesh.h"

std::vector<mesh::Material> mesh::allMaterials;
std::vector<mesh::MaterialTextures> mesh::allMaterialTextures;
gl::Storage mesh::materialUBO;

void mesh::storeMaterials()
{
	materialUBO = gl::Storage( "MaterialBuffer", sizeof( mesh::Material )*mesh::allMaterials.size(), 0, &mesh::allMaterials[0] );
    gl::setStorageTarget( materialUBO, GL_UNIFORM_BUFFER );
}





