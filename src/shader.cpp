#include "shader.h"
#include "gl.h"
#include "gldebug.h"
#include "debug.h"
#include "camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <gtc/type_ptr.hpp>
#include "mesh.h"
#include "lights.h"
#include "colorings.h"
#include "line.h"
#include "font.h"
#include "shader_loader.h"
#include "quad.h"
#include "voxelization.h"

unsigned int shader::currentShaderProgram;

std::vector<shader::Program> shader::allPrograms;
std::unordered_map<std::string, size_t> shader::shaderProgramLookup;
std::vector<shader::Module> shader::allModules;
std::unordered_map<std::string, size_t> shader::moduleLookup;


size_t shader::createModule(std::string pFileName)
{
    size_t index = allModules.size();
    moduleLookup.insert(std::pair<std::string, size_t>(pFileName, index));
    allModules.push_back(Module(pFileName));
    return index;
}

size_t shader::newProgram(std::string pProgramName, size_t pVertexShaderIndex, size_t pFragmentShaderIndex)
{
    size_t index = createProgram(pProgramName);
    allPrograms[index].type = ProgramType::Basic;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].shaderCount = 2;
    return allPrograms[index].ID;
}

size_t shader::newProgram(std::string pProgramName, size_t pVertexShaderIndex, size_t pFragmentShaderIndex, size_t pGeometryShaderIndex)
{
    size_t index = createProgram(pProgramName);
    allPrograms[index].type = ProgramType::Geometry;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].stages[2] = pGeometryShaderIndex;
    allPrograms[index].shaderCount = 3;
    return allPrograms[index].ID;
}

size_t shader::newProgram(std::string pProgramName, size_t pComputeShaderIndex)
{
    size_t index = createProgram(pProgramName);
    allPrograms[index].type = ProgramType::Compute;
    allPrograms[index].stages[0] = pComputeShaderIndex;
    allPrograms[index].shaderCount = 1;
    return allPrograms[index].ID;
}

size_t shader::createProgram(std::string pProgramName)
{
    Program program;
    program.ID = glCreateProgram();
    program.name = pProgramName;
    shaderProgramLookup.insert(std::pair<std::string, size_t>(pProgramName, program.ID));
    allPrograms.push_back(program);
    return allPrograms.size() - 1;
}

void shader::use(size_t pID)
{
    currentShaderProgram = pID;
    glUseProgram(pID);
}

void shader::use(std::string pProgramName)
{
    use(shaderProgramLookup.find(pProgramName)->second);
}

void shader::unuse()
{
    currentShaderProgram = {};
    glUseProgram(0);
}

void shader::bindUniformBufferToShader(size_t pProgram, size_t pStorageIndex, std::string pBlockName)
{
    bindUniformBufferToShader(pProgram, vao::allStorages[pStorageIndex], pBlockName);
}

void shader::bindUniformBufferToShader(size_t pProgram, vao::Storage& pStorage, std::string pBlockName)
{
    int blockIndex = glGetUniformBlockIndex(pProgram, pBlockName.c_str());
    if (blockIndex < 0) {
        debug::pushError("invalid uniform block name " + pBlockName + "!");
        return;
    }
    glUniformBlockBinding(pProgram, blockIndex, pStorage.binding);
}

void shader::bindUniformBufferToShader(std::string pProgramName, size_t pTargetStorageIndex, std::string pBlockName)
{
    auto it = shaderProgramLookup.find(pProgramName);
    if (it == shaderProgramLookup.end()) {
        debug::pushError("bindUniformBlockBuffer(): Tried to access invalid shader program!");
        return;
    }
    bindUniformBufferToShader(it->second, pTargetStorageIndex, pBlockName);
}

void shader::addVertexAttribute(size_t pProgramID, std::string pAttributeName, size_t pAttributeIndex)
{
    //binds a in variable symbol out of a shader to an attribute index
    glBindAttribLocation(pProgramID, pAttributeIndex, pAttributeName.c_str());
}

void shader::addVertexAttribute(std::string pProgramName, std::string pAttributeName, size_t pAttributeIndex)
{
    auto it = shaderProgramLookup.find(pProgramName.c_str());
    if (it == shaderProgramLookup.end()) {
        debug::pushError("addVertexAttribute():/nCould not find shader Program " + pProgramName + "!/n");
        return;
    }
    addVertexAttribute(it->second, pAttributeName, pAttributeIndex);
}

void shader::setUniform(size_t pProgram, std::string pUniformName, int pValue) {
    glUniform1i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, size_t pValue) {
    glUniform1ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, float pValue) {
    glUniform1f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::vec3 pValue) {
    glUniform3f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::vec4 pValue) {
    glUniform4f(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::uvec4 pValue) {
    glUniform4ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::uvec3 pValue) {
    glUniform3ui(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::ivec4 pValue) {
    glUniform4i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z, pValue.w);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::ivec3 pValue) {
    glUniform3i(glGetUniformLocation(pProgram, pUniformName.c_str()), pValue.x, pValue.y, pValue.z);
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::mat4 pValue, bool pTranspose) {
    glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}
void shader::setUniform(size_t pProgram, std::string pUniformName, glm::mat3 pValue, bool pTranspose) {
    glUniformMatrix3fv(glGetUniformLocation(pProgram, pUniformName.c_str()), 1, pTranspose, glm::value_ptr(pValue));
}
