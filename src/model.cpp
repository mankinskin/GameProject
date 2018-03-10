#include "model.h"
#include "model_loader.h"
#include "model.h"
#include "shader.h"
#include "mesh.h"
#include "entities.h"
#include "collision.h"
#include <cstdlib>
#include <vector>
#include <chrono>
#include <time.h>
std::vector<model::Model> model::allModels;
std::vector<std::string> model::allModelNames;

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
	//unsigned int vertexOffset = mesh::allMeshes[sphere_mesh].vertexOffset;
	//unsigned int vertexCount = mesh::allMeshes[sphere_mesh].vertexCount;
	//for (unsigned int v = 0; v < vertexCount; ++v) {
	//	glm::vec3& vert = mesh::allStaticVertices[vertexOffset + v].pos;
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
	return allModels[pID];
}

model::Model& model::getModel(std::string pName)
{
	try {
		unsigned int i = std::find(allModelNames.begin(), allModelNames.end(), pName) - allModelNames.begin();
		if (i < 0 || i >= allModelNames.size()) {
			throw "Could not find Model '"+pName+"'!";
		}
		return getModel(i);
	}
	catch (std::string pErrorMsg) {

		puts(pErrorMsg.c_str());
	}
}

unsigned int model::createModel(unsigned int pMeshOffset, unsigned int pMeshCount)
{
	allModels.emplace_back(pMeshOffset, pMeshCount);
	return allModels.size() - 1;
}

void model::revalidateModelMeshOffsets()
{
	unsigned int offs = 0;
	for (unsigned int mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].meshOffset = offs;
		offs += allModels[mod].meshCount;
	}
}
