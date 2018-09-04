#include "shader.h"
#include "gl.h"
#include "gldebug.h"
#include "debug.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <gtc/type_ptr.hpp>

static const std::string DEFAULT_SHADER_DIRECTORY = "shaders//";
std::string shader::SHADER_DIR = DEFAULT_SHADER_DIRECTORY;

static unsigned int currentShaderProgram;

void shader::setShaderDirectory(std::string& pDirectory)
{
  SHADER_DIR = pDirectory;
}

void shader::resetShaderDirectory()
{
  SHADER_DIR = DEFAULT_SHADER_DIRECTORY;
}

std::string extractStageString(std::string filename)
{
  unsigned int begin = filename.find_first_of('.') + 1;
  unsigned int end = filename.find_last_of('.');
  return filename.substr(begin, end);
}

GLenum toStageType(std::string stagestring)
{
  if (stagestring ==  "vert") {
	return GL_VERTEX_SHADER;
  }
  else if (stagestring ==  "frag") {
	return GL_FRAGMENT_SHADER;
  }
  else if (stagestring ==  "geo") {
	return GL_GEOMETRY_SHADER;
  }
  else if (stagestring ==  "comp") {
	return GL_COMPUTE_SHADER;
  }
  else {
	return 0;
  }
}

shader::Stage::Stage(std::string pFilename)
  : filename(pFilename)
	, type(toStageType(extractStageString(pFilename)))
{
  if (!type) {
	debug::fatal(
		"\nShader::loadShader(): invalid shader file name " + filename +
		"!\nHas to include '.vert', '.frag', '.geo' or '.comp'!");
  }
}

void shader::Stage::compile()
{
  printf("Shader: Compiling %s\n", filename.c_str());
  std::ifstream file;
  file.open(SHADER_DIR + filename + ".txt");
  if (file.fail()) {
	debug::fatal("Failed to compile shader: Could not open " + SHADER_DIR + filename + ".txt" + "!\n");
	return;
  }
  ID = glCreateShader(type);
  content = static_cast<std::stringstream const&>(std::stringstream() << file.rdbuf()).str();
  const char* content_ptr = content.c_str();
  int length = content.length();
  glShaderSource(ID, 1, &content_ptr, &length);
  glCompileShader(ID);
  int success = 0;
  glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
	int maxLength = 0;
	glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &maxLength);
	std::vector<char> errorLog(maxLength);
	glGetShaderInfoLog(ID, maxLength, &maxLength, &errorLog[0]);
	debug::fatal(
		"\n!!! Failed to compile " + filename +
		"\nOpenGL Error Log: " + std::string(&(errorLog[0])) + "\n");
  }
}

void shader::Program::addStage(const shader::Stage& stage)
{
  if (stageCount >= MAX_POSSIBLE_STAGES) {
	printf("Shader: Unable to add Stage %s to Program %s: Too many stages!",
		stage.filename.c_str(), name.c_str());
  }
  else {
	stages[stageCount++] = stage;
  }
}

void shader::Program::build()
{
  for(unsigned int s = 0; s < stageCount; ++s) {
	stages[s].compile();
  }
  link();
}

void shader::Program::link()
{
  printf("Shader: Linking %s\n", name.c_str());
  if (!ID) {
	puts("Shader: Unable to link program: Program not created yet.");
  }
  if (!stageCount) {
	puts("Shader: Unable to link program: Program contains no stages.");
  }
  for (unsigned int i = 0; i < stageCount; ++i) {
	glAttachShader(ID, stages[i].ID);
  }
  glLinkProgram(ID);
  GLint success = 0;
  glGetProgramiv(ID, GL_LINK_STATUS, &success);

  if (success == GL_FALSE) {
	GLint maxLength = 0;
	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &maxLength);
	std::vector<char> errorLog(maxLength);
	glGetProgramInfoLog(ID, maxLength, &maxLength, &errorLog[0]);
	glDeleteProgram(ID);
	debug::fatal("!!!/nError when linking program: " + name + " /nopenGL Error Log: " + &(errorLog[0]));
	return;
  }
  for (unsigned int i = 0; i < stageCount; ++i) {
	glDetachShader(ID, stages[i].ID);
  }
}

void shader::Program::use() const
{
  currentShaderProgram = ID;
  glUseProgram(ID);
}

void shader::Program::unuse()
{
  currentShaderProgram = 0;
  glUseProgram(0);
}

void shader::Program::addVertexAttribute(std::string pAttributeName, unsigned int pAttributeIndex)
{
  glBindAttribLocation(ID, pAttributeIndex, pAttributeName.c_str());
}

void shader::Program::setUniform(std::string pUniformName, int pValue) {
  glUniform1i(glGetUniformLocation(ID, pUniformName.c_str()), pValue);
}
void shader::Program::setUniform(std::string pUniformName, unsigned int pValue) {
  glUniform1ui(glGetUniformLocation(ID, pUniformName.c_str()), pValue);
}
void shader::Program::setUniform(std::string pUniformName, float pValue) {
  glUniform1f(glGetUniformLocation(ID, pUniformName.c_str()), pValue);
}
void shader::Program::setUniform(std::string pUniformName, glm::vec3 pValue) {
  glUniform3f(glGetUniformLocation(ID, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}
void shader::Program::setUniform(std::string pUniformName, glm::vec4 pValue) {
  glUniform4f(glGetUniformLocation(ID, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}
void shader::Program::setUniform(std::string pUniformName, glm::uvec4 pValue) {
  glUniform4ui(glGetUniformLocation(ID, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}
void shader::Program::setUniform(std::string pUniformName, glm::uvec3 pValue) {
  glUniform3ui(glGetUniformLocation(ID, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}
void shader::Program::setUniform(std::string pUniformName, glm::ivec4 pValue) {
  glUniform4i(glGetUniformLocation(ID, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}
void shader::Program::setUniform(std::string pUniformName, glm::ivec3 pValue) {
  glUniform3i(glGetUniformLocation(ID, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}
void shader::Program::setUniform(std::string pUniformName, glm::mat4 pValue, bool pTranspose) {
  glUniformMatrix4fv(glGetUniformLocation(ID, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}
void shader::Program::setUniform(std::string pUniformName, glm::mat3 pValue, bool pTranspose) {
  glUniformMatrix3fv(glGetUniformLocation(ID, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}
