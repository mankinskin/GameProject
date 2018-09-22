#include "model.h"
#include "modelfile.h"
#include "mesh.h"
#include "entity.h"
#include "collision.h"
#include <cstdlib>
#include <vector>
#include <chrono>
#include <time.h>
utils::Container<model::Model> model::Model::all;
utils::Container<std::string> model::allModelNames;

void model::initModels()
{
  //create meshes
  //model::Loader::includeModel("sphere.3DS", "sphere");
  //model::Loader::includeModel("cube.obj");
  //model::Loader::includeModel("astroBoy_walk.dae");
  //model::Loader::includeModel("deer.3ds");
  //model::Loader::includeModel("test/models-nonbsd/3DS/mp5_sil.3ds");
  model::Loader::loadModels();

  //normalize sphere mesh
  //unsigned int sphere_mesh = model::getModel("sphere").meshOffset;
  //unsigned int vertexOffset = model::mesh::allMeshes[sphere_mesh].vertexOffset;
  //unsigned int vertexCount = model::mesh::allMeshes[sphere_mesh].vertexCount;
  //for (unsigned int v = 0; v < vertexCount; ++v) {
  //	glm::vec3& vert = model::mesh::allStaticVertices[vertexOffset + v].pos;
  //	vert = glm::normalize(vert);
  //}
}

void model::setupModels()
{
  using namespace mesh;
  srand(time(NULL));
  unsigned int nodes_to_generate = 100;
  std::vector<unsigned int> generated_nodes(nodes_to_generate);
  entities::reserveEntities(nodes_to_generate);

  unsigned int grid_width = 100;

  for (unsigned int ni = 0; ni < nodes_to_generate; ++ni) {
	entities::createEntity(&generated_nodes[ni]);
	entities::setPos(generated_nodes[ni], glm::vec3((float)(rand()%grid_width)- (float)(grid_width/2), (float)(rand() % 10) - (float)(10 / 2), (float)(rand() % grid_width) - (float)(grid_width / 2)));
	entities::setScale(generated_nodes[ni], glm::vec3(0.5f, 0.5f, 0.5f));
	entities::setRotation(generated_nodes[ni], glm::vec4(0.0f, 1.0f, 0.0f, 1.5f));
  }

  addInstancesToMesh(getModel(0).meshOffset, generated_nodes);
  //addInstancesToMesh(getModel(1).meshOffset, { sphere_B_node });
}

model::Model& model::getModel(unsigned int pID)
{
  return Model::all[pID];
}

model::Model& model::getModel(std::string pName)
{
  unsigned int i = std::find(allModelNames.begin(), allModelNames.end(), pName) - allModelNames.begin();
  if (i < 0 || i >= allModelNames.size()) {
	printf("Could not find Model '%s'!", pName.c_str());
	return getModel(0);
  }
  return getModel(i);
}

unsigned int model::createModel(unsigned int pMeshOffset, unsigned int pMeshCount)
{
  Model::all.emplace_back(pMeshOffset, pMeshCount);
  return Model::all.size() - 1;
}

void model::revalidateModelMeshOffsets()
{
  unsigned int offs = 0;
  for (unsigned int mod = 0; mod < Model::all.size(); ++mod) {
	Model::all[mod].meshOffset = offs;
	offs += Model::all[mod].meshCount;
  }
}
