#include "model_loader.h"
#include "debug.h"
#include <assimp/postprocess.h>
#include "model.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#define DEFAULT_MODEL_DIR "assets//models//"

std::string model::Loader::MODEL_DIR = DEFAULT_MODEL_DIR;
Assimp::Importer imp = Assimp::Importer();
std::vector<model::Loader::ModelLoadFile> modelLoadBuffer;


void model::Loader::includeModel(std::string pFilename, std::string pName)
{
	//define model file to load
	modelLoadBuffer.emplace_back(pFilename, pName);
}

void model::Loader::loadModels()
{
	allModels.reserve(allModels.size() + modelLoadBuffer.size());
	for (ModelLoadFile& file : modelLoadBuffer) {
		loadModelFile(file);
	}
	allModels.shrink_to_fit();
	mesh::storeMaterials();
}

void model::Loader::loadMeshes(const aiScene* pScene) {

	using namespace mesh;
	size_t vertexCount = 0;
	size_t faceCount = 0;
	size_t vertexOffset = allStaticVertices.size();
	size_t indexOffset = allIndices.size();
	//collect mesh information
	for (size_t m = 0; m < pScene->mNumMeshes; ++m) {
		aiMesh* mesh = pScene->mMeshes[m];
		vertexCount += mesh->mNumVertices;
		faceCount += mesh->mNumFaces;
	}

	allStaticVertices.reserve(vertexOffset + vertexCount);
	allIndices.reserve(indexOffset + faceCount * 3);
	allMeshes.reserve(allMeshes.size() + pScene->mNumMeshes);
	

	for (size_t m = 0; m < pScene->mNumMeshes; ++m) {
		loadMesh(pScene, m, vertexOffset, indexOffset);
	}
}

void model::Loader::loadMesh(const aiScene* pScene, size_t pMeshIndex, size_t& pVertexOffset, size_t& pIndexOffset)
{
	aiMesh* mesh = pScene->mMeshes[pMeshIndex];
	for (size_t v = 0; v < mesh->mNumVertices; ++v) {
		aiVector3D pos = aiVector3D(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
		aiVector3D normal = mesh->mNormals[v];
		aiVector3D uv = aiVector3D();
		if (mesh->HasTextureCoords(pMeshIndex)) {
			uv = mesh->mTextureCoords[0][v];
		}
		mesh::allStaticVertices.emplace_back(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uv.x, uv.y);
	}
	for (size_t f = 0; f < mesh->mNumFaces; ++f) {
		aiFace face = mesh->mFaces[f];
		for (size_t fi = 0; fi < 3; ++fi) {
			mesh::allIndices.push_back(face.mIndices[fi] + pVertexOffset);
		}
	}

	mesh::allMeshes.emplace_back(pVertexOffset, mesh->mNumVertices, pIndexOffset, mesh->mNumFaces * 3, size_t(mesh::allMaterials.size() + mesh->mMaterialIndex));
	pVertexOffset += mesh->mNumVertices;
	pIndexOffset += mesh->mNumFaces * 3;
}

void model::Loader::loadMaterials(const aiScene* pScene) {
	using namespace mesh;
	size_t off = allMaterials.size();
	allMaterials.resize(off + pScene->mNumMaterials);
	allMaterialTextures.resize(off + pScene->mNumMaterials);
	for (size_t m = 0; m < pScene->mNumMaterials; ++m) {
		loadMaterial(off + m, pScene->mMaterials[m]);
	}
	for (size_t m = 0; m < pScene->mNumMaterials; ++m) {
		loadMaterialTextures(off + m, pScene->mMaterials[m]);
	}
}

void model::Loader::loadMaterial(size_t pTargetIndex, aiMaterial* pMat) {
	using mesh::allMaterials;
	allMaterials[pTargetIndex].amb_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	allMaterials[pTargetIndex].diff_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	allMaterials[pTargetIndex].spec_color = glm::vec4(1.0f, 1.0f, 1.0f, 100.0f);
	//pMat->Get(AI_MATKEY_COLOR_AMBIENT, allMaterials[pTargetIndex].amb_color);
	//pMat->Get(AI_MATKEY_COLOR_DIFFUSE, allMaterials[pTargetIndex].diff_color);
	//pMat->Get(AI_MATKEY_COLOR_SPECULAR, allMaterials[pTargetIndex].spec_color);
	//pMat->Get(AI_MATKEY_SHININESS, allMaterials[pTargetIndex].spec_color.w);
}

void model::Loader::loadMaterialTextures(size_t pTargetIndex, aiMaterial* pMat)
{
	aiString path_buf;

	if (pMat->GetTextureCount(aiTextureType_AMBIENT)) {
		pMat->GetTexture(aiTextureType_AMBIENT, 0, &path_buf);
		mesh::allMaterialTextures[pTargetIndex].amb_tex = texture::all2DTextures[texture::createTexture2D(path_buf.C_Str())].ID;
	}
	if (pMat->GetTextureCount(aiTextureType_DIFFUSE)) {
		pMat->GetTexture(aiTextureType_DIFFUSE, 0, &path_buf);
		mesh::allMaterialTextures[pTargetIndex].diff_tex = texture::all2DTextures[texture::createTexture2D(path_buf.C_Str())].ID;
	}
	if (pMat->GetTextureCount(aiTextureType_SPECULAR)) {
		pMat->GetTexture(aiTextureType_SPECULAR, 0, &path_buf);
		mesh::allMaterialTextures[pTargetIndex].spec_tex = texture::all2DTextures[texture::createTexture2D(path_buf.C_Str())].ID;
	}
}

void model::Loader::loadModelFile(ModelLoadFile pFile)
{
	using namespace mesh;
	const aiScene* scene = imp.ReadFile(MODEL_DIR + pFile.filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded	);
	const char* err = imp.GetErrorString();
	if (err[0] != '/0') {
		debug::pushError(err);
	}
	if (scene == nullptr) {
		debug::pushError("scene was nullptr!");
	}
	Model model;
	model.meshOffset = allMeshes.size();
	model.meshCount = scene->mNumMeshes;
	loadMeshes(scene);
	loadMaterials(scene);
	std::string* name = &pFile.modelname;
	if (pFile.modelname == "") {
		name = &pFile.filename;
	}
	allModels.push_back(model);
	allModelNames.push_back(*name);
}

void model::Loader::setModelDirectory(std::string&& pDirectory)
{
	MODEL_DIR = pDirectory;
}

void model::Loader::resetModelDirectory()
{
	MODEL_DIR = DEFAULT_MODEL_DIR;
}


