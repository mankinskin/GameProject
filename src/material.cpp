#include "material.h"
#include "vao.h"
#include "shader.h"
#include "gldebug.h"
#include "mesh.h"

utils::Container<model::mesh::Material> model::mesh::Material::all;
utils::Container<model::mesh::MaterialTextures> model::mesh::MaterialTextures::all;
gl::Storage<model::mesh::Material> model::mesh::materialUBO;

void model::mesh::storeMaterials()
{
  materialUBO = gl::Storage<Material>("MaterialBuffer", Material::all.size(), 0, &Material::all[0]);
  materialUBO.setTarget(GL_UNIFORM_BUFFER);
}

