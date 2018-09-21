#include "material.h"
#include "vao.h"
#include "shader.h"
#include "gldebug.h"
#include "mesh.h"

utils::Container<mesh::Material> mesh::Material::all;
utils::Container<mesh::MaterialTextures> mesh::MaterialTextures::all;
gl::Storage<mesh::Material> mesh::materialUBO;

void mesh::storeMaterials()
{
  materialUBO = gl::Storage<Material>("MaterialBuffer", Material::all.size(), 0, &Material::all[0]);
  materialUBO.setTarget(GL_UNIFORM_BUFFER);
}

