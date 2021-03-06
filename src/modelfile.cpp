#include "modelfile.h"
#include "debug.h"
#include <assimp/postprocess.h>
#include "model.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#define DEFAULT_MODEL_DIR "assets/models/"

std::string model::Loader::MODEL_DIR = DEFAULT_MODEL_DIR;
Assimp::Importer imp = Assimp::Importer();
std::vector<model::Loader::ModelFile> loadBuffer;

void model::Loader::includeModel(std::string pFilename, std::string pName)
{
  loadBuffer.emplace_back(pFilename, pName);
}

void model::Loader::loadModels()
{
  if (!loadBuffer.size())
	//nothing to do
	return;
  Model::all.reserve(Model::all.size() + loadBuffer.size());
  for (const ModelFile& file : loadBuffer) {
	loadModelFile(file);
  }
  Model::all.shrink_to_fit();
  model::mesh::storeMaterials();
}

void model::Loader::loadModelFile(const ModelFile pFile)
{
  using namespace mesh;
  constexpr int ASSIMP_LOAD_OPTIONS = aiProcess_Triangulate;
  const aiScene* scene = imp.ReadFile(MODEL_DIR + pFile.filename, ASSIMP_LOAD_OPTIONS);
  const char* err = imp.GetErrorString();
  if (err[0] != '\0') {
	debug::warning(err);
	return;
  }
  if (scene == nullptr) {
	debug::error("scene was nullptr!");
	return;
  }
  Model model;
  model.meshOffset = allMeshes.size();
  model.meshCount = scene->mNumMeshes;
  loadMeshes(scene);
  loadMaterials(scene);
  if (pFile.modelname != "") {
	allModelNames.push_back(pFile.modelname);
  } else {
	allModelNames.push_back(pFile.filename);
  }
  Model::all.push_back(model);
}

void model::Loader::loadMeshes(const aiScene* pScene)
{
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
	model::mesh::allStaticVertices.push_back(model::mesh::Vertex(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uv.x, uv.y));
  }
  for (size_t f = 0; f < mesh->mNumFaces; ++f) {
	aiFace face = mesh->mFaces[f];
	for (size_t fi = 0; fi < 3; ++fi) {
	  model::mesh::allIndices.push_back(face.mIndices[fi] + pVertexOffset);
	}
  }

  model::mesh::allMeshes.emplace_back(pVertexOffset, mesh->mNumVertices, pIndexOffset, mesh->mNumFaces * 3, size_t(model::mesh::Material::all.size() + mesh->mMaterialIndex));
  pVertexOffset += mesh->mNumVertices;
  pIndexOffset += mesh->mNumFaces * 3;
}

void model::Loader::loadMaterials(const aiScene* pScene) {
  using namespace mesh;
  size_t off = Material::all.size();
  Material::all.resize(off + pScene->mNumMaterials);
  MaterialTextures::all.resize(off + pScene->mNumMaterials);
  for (size_t m = 0; m < pScene->mNumMaterials; ++m) {
	loadMaterial(off + m, pScene->mMaterials[m]);
  }
  for (size_t m = 0; m < pScene->mNumMaterials; ++m) {
	loadMaterialTextures(off + m, pScene->mMaterials[m]);
  }
}

void model::Loader::loadMaterial(size_t pTargetIndex, aiMaterial* pMat) {
  using model::mesh::Material;
  Material::all[pTargetIndex].amb_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  Material::all[pTargetIndex].diff_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  Material::all[pTargetIndex].spec_color = glm::vec4(1.0f, 1.0f, 1.0f, 100.0f);
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
	model::mesh::MaterialTextures::all[pTargetIndex].amb_tex = texture::Texture2D(path_buf.C_Str());
  }
  if (pMat->GetTextureCount(aiTextureType_DIFFUSE)) {
	pMat->GetTexture(aiTextureType_DIFFUSE, 0, &path_buf);
	model::mesh::MaterialTextures::all[pTargetIndex].diff_tex = texture::Texture2D(path_buf.C_Str());
  }
  if (pMat->GetTextureCount(aiTextureType_SPECULAR)) {
	pMat->GetTexture(aiTextureType_SPECULAR, 0, &path_buf);
	model::mesh::MaterialTextures::all[pTargetIndex].spec_tex = texture::Texture2D(path_buf.C_Str());
  }
}

void model::Loader::setModelDirectory(std::string&& pDirectory)
{
  MODEL_DIR = pDirectory;
}

void model::Loader::resetModelDirectory()
{
  MODEL_DIR = DEFAULT_MODEL_DIR;
}
