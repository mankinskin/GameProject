#include "material.h"
#include "vao.h"
#include "shader.h"
#include "gldebug.h"
#include "mesh.h"

std::vector<mesh::Material> mesh::allMaterials;
std::vector<mesh::MaterialTextures> mesh::allMaterialTextures;
gl::Storage<mesh::Material> mesh::materialUBO;

void mesh::storeMaterials()
{
  materialUBO = gl::Storage<Material>("MaterialBuffer", mesh::allMaterials.size(), 0, &mesh::allMaterials[0]);
  materialUBO.setTarget(GL_UNIFORM_BUFFER);
}

