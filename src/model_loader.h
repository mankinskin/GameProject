#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

namespace model {
	struct Model;
	extern std::vector<Model> allModels;
	namespace Loader {
		extern std::string MODEL_DIR;
	
		struct ModelLoadFile {
			ModelLoadFile( std::string pFilename, std::string pModelname )
				:filename( pFilename ), modelname( pModelname ){}
			std::string filename;
			std::string modelname;
		};
		void loadModels();
		
		void loadModelFile( ModelLoadFile pFile );
		
		void includeModel( std::string pFilename, std::string pName = "" );
		void loadMeshes( const aiScene * pScene );
		void loadMesh( const aiScene* pScene, size_t pMeshIndex, size_t& pVertexOffset, size_t& pIndexOffset );
		void loadMaterials( const aiScene * pScene );
		void loadMaterial( size_t pTargetIndex, aiMaterial* pMat );
		void loadMaterialTextures( size_t pTargetIndex, aiMaterial* pMat );
		void setModelDirectory( std::string&& pDirectory );
		void resetModelDirectory();

	}
}
