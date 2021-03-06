#include "model.h"
#include "modelfile.h"
#include "mesh.h"
#include "nodes.h"
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
  model::Loader::includeModel("cube.3ds");
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
  unsigned int nodes_to_generate = 1024;
  std::vector<nodes::NodeID> generated_nodes(nodes_to_generate);
  nodes::Node::reserve(nodes_to_generate);

  unsigned int grid_width = ceil(pow(nodes_to_generate, 1/3))*100;

  for (size_t n = 0; n < generated_nodes.size(); ++n) {
    nodes::NodeID& node = generated_nodes[n];
    node = nodes::Node::create();
    node->setPos(glm::vec3((float)((rand() + n)%grid_width)- (float)(grid_width/2), (float)((rand()+n) % 10) - (float)(10 / 2), (float)((rand() + n) % grid_width) - (float)(grid_width / 2)));
    node->setScale(glm::vec3(0.001f, 0.001f, 0.001f));
  }

  addInstancesToMesh(getModel(0).meshOffset, generated_nodes);
  //addInstancesToMesh(getModel(1).meshOffset, { sphere_B_nodes });
  revalidateMeshNodeOffsets();
  revalidateModelMeshOffsets();
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
