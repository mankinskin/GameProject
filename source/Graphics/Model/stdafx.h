#pragma once
#include <vector>
#include <string>

namespace model {
	struct Model;
	extern std::vector<Model> allModels;
	namespace Loader {
		extern std::string MODEL_DIR;
	}
}
