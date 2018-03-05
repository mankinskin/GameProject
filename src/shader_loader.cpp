#include "shader_loader.h"
#include "shader.h"
#include "debug.h"
#include <fstream>
#include <sstream>
#include "gldebug.h"

#define DEFAULT_SHADER_DIRECTORY "shaders//"

std::string shader::Loader::SHADER_DIR = DEFAULT_SHADER_DIRECTORY;



void shader::Loader::buildShaderPrograms()
{

	compileAndLink();
}

void shader::Loader::setShaderDirectory(std::string& pDirectory)
{
	SHADER_DIR = pDirectory;
}

void shader::Loader::resetShaderDirectory()
{
	SHADER_DIR = DEFAULT_SHADER_DIRECTORY;
}

void shader::Loader::compileModule(size_t pModuleIndex)
{
	Module& mod = allModules[pModuleIndex];
	if (mod.fileName.find(".vert") != std::string::npos) {
		mod.ID = glCreateShader(GL_VERTEX_SHADER);
		mod.type = ModuleType::Vertex;
	}
	else if (mod.fileName.find(".geo") != std::string::npos) {
		mod.ID = glCreateShader(GL_GEOMETRY_SHADER);
		mod.type = ModuleType::Geometry;
	}
	else if (mod.fileName.find(".frag") != std::string::npos) {
		mod.ID = glCreateShader(GL_FRAGMENT_SHADER);
		mod.type = ModuleType::Fragment;
	}
	else if (mod.fileName.find(".comp") != std::string::npos) {
		mod.ID = glCreateShader(GL_COMPUTE_SHADER);
		mod.type = ModuleType::Compute;
	}
	else {
		debug::pushError("/nShader::loadShader(): invalid shader file name " + mod.fileName + "!/nHas to include '.vert', '.frag', '.geo' or '.comp'!", debug::Error::Fatal);
		return;
	}
	std::ifstream moduleFile;
	moduleFile.open(SHADER_DIR + mod.fileName + ".txt");
	if (moduleFile.fail()) {
		debug::pushError("Failed to compile shader: Could not open " + SHADER_DIR + mod.fileName + ".txt" + "!/n");
		return;
	}

	mod.content = static_cast<std::stringstream const&>(std::stringstream() << moduleFile.rdbuf()).str();
	const char* content = mod.content.c_str();
	int length = mod.content.length();
	glShaderSource(mod.ID, 1, &content, &length);
	glCompileShader(mod.ID);
	int success = 0;
	glGetShaderiv(mod.ID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int maxLength = 0;
		glGetShaderiv(mod.ID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(mod.ID, maxLength, &maxLength, &errorLog[0]);
		debug::pushError("Failed to compile " + mod.fileName + "/nOpenGL Error Log: " + std::string(&(errorLog[0])) + "/n", debug::Error::Fatal);
		return;
	}
	allModules[pModuleIndex] = mod;
}

void shader::Loader::linkProgram(size_t pProgramIndex)
{
	Program& program = allPrograms[pProgramIndex];

	if (program.type == ProgramType::Compute) {
		program.stages[0] = allModules[program.stages[0]].ID;
		glAttachShader(program.ID, program.stages[0]);
	}
	else
	{
		for (size_t i = 0; i < program.shaderCount; ++i) {
			program.stages[i] = allModules[program.stages[i]].ID;
			glAttachShader((GLuint)program.ID, (GLuint)program.stages[i]);
		}
	}
	glLinkProgram(program.ID);
	GLint success = 0;
	glGetProgramiv(program.ID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program.ID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(program.ID, maxLength, &maxLength, &errorLog[0]);

		glDeleteProgram(program.ID);


		debug::pushError("!!!/nError when linking program: " + program.name + " /nopenGL Error Log: " + &(errorLog[0]), debug::Error::Fatal);

	}

	for (size_t i = 0; i < program.shaderCount; ++i) {
		glDetachShader(program.ID, program.stages[i]);
	}
}

void shader::Loader::compileAndLink()
{
	for (size_t s = 0; s < allModules.size(); ++s) {
		compileModule(s);
	}
	for (size_t p = 0; p < allPrograms.size(); ++p) {
		linkProgram(p);
	}
}
