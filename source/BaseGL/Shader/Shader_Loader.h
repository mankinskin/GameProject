#pragma once
#include <string>

namespace shader {
	namespace Loader {


		void buildShaderPrograms();
		void setShaderDirectory(std::string& pDirectory);
		void resetShaderDirectory();

		void compileModule(size_t pModuleIndex);
		void linkProgram(size_t pProgramID);
		void compileAndLink();
	}
}
