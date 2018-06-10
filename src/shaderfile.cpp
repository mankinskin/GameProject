#include "shaderfile.h"
#include "shader.h"
#include "debug.h"
#include <fstream>
#include <sstream>
#include "gldebug.h"

static const std::string DEFAULT_SHADER_DIRECTORY = "shaders//";
std::string shader::Shaderfile::SHADER_DIR = DEFAULT_SHADER_DIRECTORY;

void shader::Loader::setShaderDirectory( std::string& pDirectory )
{
	SHADER_DIR = pDirectory;
}

void shader::Loader::resetShaderDirectory()
{
	SHADER_DIR = DEFAULT_SHADER_DIRECTORY;
}

