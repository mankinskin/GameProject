#include "model.h"
#include "Model_Loader.h"
#include "Model.h"
#include "shader.h"
#include "Mesh.h"
#include "Entities.h"
#include "Collision.h"
#include <cstdlib>
#include <vector>
#include <chrono>
#include <time.h>
std::vector<model::Model> model::allModels;
std::vector<std::string> model::allModelNames;

void model::initModels()
{
	//create meshes
	model::Loader::includeModel("sphere.3DS", "sphere");
	//model::Loader::includeModel("cube.obj");
	//model::Loader::includeModel("astroBoy_walk.dae");
	//model::Loader::includeModel("deer.3ds");
	//model::Loader::includeModel("test/models-nonbsd/3DS/mp5_sil.3ds");
	model::Loader::loadModels();
	
	//normalize sphere mesh
	size_t sphere_mesh = model::getModel("sphere").meshOffset;
	size_t vertexOffset = mesh::allMeshes[sphere_mesh].vertexOffset;
	size_t vertexCount = mesh::allMeshes[sphere_mesh].vertexCount;
	for (size_t v = 0; v < vertexCount; ++v) {
		glm::vec3& vert = mesh::allStaticVertices[vertexOffset + v].pos;
		vert = glm::normalize(vert);
	}
}

void model::setupModels()
{
	using namespace mesh;
	srand(time(NULL));
	size_t nodes_to_generate = 100;
	std::vector<size_t> generated_nodes(nodes_to_generate);
	entities::reserveEntities(nodes_to_generate);

	size_t grid_width = 100;

	for (unsigned int ni = 0; ni < nodes_to_generate; ++ni) {

		entities::createEntity(&generated_nodes[ni]);
		entities::setPos(generated_nodes[ni], glm::vec3((float)(rand()%grid_width)- (float)(grid_width/2), (float)(rand() % 10) - (float)(10 / 2), (float)(rand() % grid_width) - (float)(grid_width / 2)));
		entities::setScale(generated_nodes[ni], glm::vec3(0.5f, 0.5f, 0.5f));
		entities::setRotation(generated_nodes[ni], glm::vec4(0.0f, 1.0f, 0.0f, 1.5f));
	}

	addInstancesToMesh(getModel(0).meshOffset, generated_nodes);
	//addInstancesToMesh(getModel(1).meshOffset, { sphere_B_node });
}

model::Model& model::getModel(size_t pID)
{
	return allModels[pID];
}

model::Model& model::getModel(std::string pName)
{
	try {
		size_t i = std::find(allModelNames.begin(), allModelNames.end(), pName) - allModelNames.begin();
		if (i < 0 || i >= allModelNames.size()) {
			throw "Could not find Model '"+pName+"'!";
		}
		return getModel(i);
	}
	catch (std::string pErrorMsg) {

		puts(pErrorMsg.c_str());
	}
}

size_t model::createModel(size_t pMeshOffset, size_t pMeshCount)
{
	allModels.emplace_back(pMeshOffset, pMeshCount);
	return allModels.size() - 1;
}

void model::revalidateModelMeshOffsets()
{
	size_t offs = 0;
	for (size_t mod = 0; mod < allModels.size(); ++mod) {
		allModels[mod].meshOffset = offs;
		offs += allModels[mod].meshCount;
	}
}