#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "model.h"
#include "Model_Loader.h"
#include "../Model/Model.h"
#include "../BaseGL/shader/shader.h"
#include "../Mesh/Mesh.h"
#include "Node.h"
#include "../Physics/Collision.h"

std::vector<model::Model> model::allModels;

void model::initModels()
{
	//create meshes
	//model::Loader::includeModel("sphere.3DS");
	//model::Loader::includeModel("cube.obj");
	//model::Loader::includeModel("astroBoy_walk.dae");
	model::Loader::includeModel("deer.3ds");
	model::Loader::includeModel("test/models-nonbsd/3DS/mp5_sil.3ds");
	model::Loader::loadModels();

}

void model::setupModels()
{
	using namespace mesh;
	size_t triangle_A = createMesh({ Vertex(-1.0f, -2.0f, 1.0f), Vertex(2.0f, 1.0f, -1.0f), Vertex(0.0f, 2.0f, 1.0f) }, { 0, 1, 2 }, 0);
	size_t triangle_B = createMesh({ Vertex(-2.0f, 0.0f, 2.0f), Vertex(1.0f, 0.5f, 1.0f), Vertex(0.0f, -1.0f, -2.0f) }, { 0, 1, 2 }, 0);

	size_t sphere_A_node;
	size_t sphere_B_node;


	node::createNode(&sphere_A_node);
	node::createNode(&sphere_B_node);


	node::setPos(sphere_A_node, glm::vec3(14.0f, 5.0f, 1.0f));
	node::setScale(sphere_A_node, glm::vec3(0.8f, 0.8f, 0.8f));
	node::setRotation(sphere_A_node, glm::vec4(1.0f, 0.0f, 0.0f, 2.0f));

	node::setPos(sphere_B_node, glm::vec3(4.0f, 5.0f, 3.0f));
	node::setScale(sphere_B_node, glm::vec3(0.1f, 0.1f, 0.1f));
	node::setRotation(sphere_B_node, glm::vec4(1.0f, 0.0f, 0.0f, 2.0f));
	addInstancesToMesh(getModel(0).meshOffset, { sphere_A_node });
	addInstancesToMesh(getModel(1).meshOffset, { sphere_B_node });
}

model::Model & model::getModel(size_t pID)
{
	return allModels[pID];
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