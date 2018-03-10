#pragma once
#include <glew.h>
#include <string>
#include <vector>
#include <unordered_map>
namespace shader {
	enum class ModuleType {
		Vertex,
		Fragment,
		Geometry,
		Compute
	};

	enum class ProgramType {
		Basic,
		Geometry,
		Compute
	};

	struct Program {
		GLuint ID;
		std::string name;
		ProgramType type;
		unsigned int stages[4];
		unsigned int shaderCount;
	};

	struct Module {
		Module(std::string& pFileName) :fileName(pFileName), content(""), ID(0), type(ModuleType::Vertex) {}
		std::string content;
		std::string fileName;
		GLuint ID;
		ModuleType type;
	};
	
	extern std::vector<Program> allPrograms;
	extern std::unordered_map<std::string, unsigned int> shaderProgramLookup;
	extern std::vector<Module> allModules;
	extern std::unordered_map<std::string, unsigned int> moduleLookup;

	namespace Loader {
		extern std::string SHADER_DIR;

		void buildShaderPrograms();
		void setShaderDirectory(std::string& pDirectory);
		void resetShaderDirectory();

		void compileModule(unsigned int pModuleIndex);
		void linkProgram(unsigned int pProgramID);
		void compileAndLink();
	}
}
