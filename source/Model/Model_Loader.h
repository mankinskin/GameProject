#pragma once
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <vector>

namespace model {
	namespace Loader {

		void loadModels();
		
		void loadModelFile(std::string pFilename);
		
		void includeModel(std::string pFilename);
		void loadMeshes(const aiScene * pScene);
		void loadMesh(const aiScene* pScene, size_t pMeshIndex, size_t& pVertexOffset, size_t& pIndexOffset);
		void loadMaterials(const aiScene * pScene);
		void loadMaterial(size_t pTargetIndex, aiMaterial* pMat);
		void loadMaterialTextures(size_t pTargetIndex, aiMaterial* pMat);
		void setModelDirectory(std::string&& pDirectory);
		void resetModelDirectory();

	}
}
