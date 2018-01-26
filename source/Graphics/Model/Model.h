#pragma once
#include "..\GlobalGL\gl.h"
#include <vector>
#include <string>

namespace model {

	struct Model {
		Model()
			:meshOffset(0), meshCount(0) {}
		Model(size_t pMeshOffset, size_t pMeshCount)
			:meshOffset(pMeshOffset), meshCount(pMeshCount) {}


		size_t meshOffset = 0;
		size_t meshCount = 0;
	};
	void initModels();
	void setupModels();
	void revalidateModelMeshOffsets();
	Model& getModel(size_t pID);
	const size_t MAX_MODELS = 100;

	size_t createModel(size_t pMeshOffset, size_t pMeshCount);

}
