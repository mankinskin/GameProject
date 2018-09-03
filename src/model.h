#pragma once
#include "gl.h"
#include <vector>
#include <string>

namespace model
{
  struct Model
  {
	Model()
	  : meshOffset(0)
	  , meshCount(0)
	{}
	Model(unsigned int pMeshOffset, unsigned int pMeshCount)
	  : meshOffset(pMeshOffset)
	  , meshCount(pMeshCount)
	{}
	unsigned int meshOffset = 0;
	unsigned int meshCount = 0;
  };

  void initModels();
  void setupModels();
  void revalidateModelMeshOffsets();
  Model& getModel(unsigned int pID);
  Model& getModel(std::string pName);
  const unsigned int MAX_MODELS = 100;
  const unsigned int MAX_MESHES_PER_MODEL = 10;

  unsigned int createModel(unsigned int pMeshOffset, unsigned int pMeshCount);
  extern std::vector<std::string> allModelNames;
}
