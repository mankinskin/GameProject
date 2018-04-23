#include "shader.h"
#include "gl.h"
#include "glDebug.h"
#include "Debug.h"
#include "camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <gtc/type_ptr.hpp>
#include "mesh.h"
#include "lights.h"
#include "quadcolors.h"
#include "line.h"
#include "font.h"
#include "shader_loader.h"
#include "quad.h"
#include "voxelization.h"

unsigned int shader::currentShaderProgram;

std::vector<shader::Program> shader::allPrograms;
std::unordered_map<std::string, unsigned int> shader::shaderProgramLookup;
std::vector<shader::Module> shader::allModules;
std::unordered_map<std::string, unsigned int> shader::moduleLookup;


unsigned int shader::createModule( std::string pFileName )
{
    unsigned int index = allModules.size();
    moduleLookup.insert( std::pair<std::string, unsigned int>( pFileName, index ) );
    allModules.push_back( Module( pFileName ) );
    return index;
}

unsigned int shader::newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Basic;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].shaderCount = 2;
    return allPrograms[index].ID;
}

unsigned int shader::newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex, unsigned int pGeometryShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Geometry;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].stages[2] = pGeometryShaderIndex;
    allPrograms[index].shaderCount = 3;
    return allPrograms[index].ID;
}

unsigned int shader::newProgram( std::string pProgramName, unsigned int pComputeShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Compute;
    allPrograms[index].stages[0] = pComputeShaderIndex;
    allPrograms[index].shaderCount = 1;
    return allPrograms[index].ID;
}

unsigned int shader::createProgram( std::string pProgramName )
{
    Program program;
    program.ID = glCreateProgram();
    program.name = pProgramName;
    shaderProgramLookup.insert( std::pair<std::string, unsigned int>( pProgramName, program.ID ) );
    allPrograms.push_back( program );
    return allPrograms.size() - 1;
}

void shader::use( unsigned int pID )
{
    currentShaderProgram = pID;
    glUseProgram( pID );
}

void shader::use( std::string pProgramName )
{
    use( shaderProgramLookup.find( pProgramName )->second );
}

void shader::unuse()
{
    currentShaderProgram = {};
    glUseProgram( 0 );
}

void shader::addVertexAttribute( unsigned int pProgramID, std::string pAttributeName, unsigned int pAttributeIndex )
{
    //binds a in variable symbol out of a shader to an attribute index
    glBindAttribLocation( pProgramID, pAttributeIndex, pAttributeName.c_str() );
}

void shader::addVertexAttribute( std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex )
{
    auto it = shaderProgramLookup.find( pProgramName.c_str() );
    if ( it == shaderProgramLookup.end() ) {
        debug::pushError( "addVertexAttribute():/nCould not find shader Program " + pProgramName + "!/n" );
        return;
    }
    addVertexAttribute( it->second, pAttributeName, pAttributeIndex );
}

void shader::setUniform( unsigned int pProgram, std::string pUniformName, int pValue ) {
    glUniform1i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, unsigned int pValue ) {
    glUniform1ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, float pValue ) {
    glUniform1f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::vec3 pValue ) {
    glUniform3f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::vec4 pValue ) {
    glUniform4f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec4 pValue ) {
    glUniform4ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec3 pValue ) {
    glUniform3ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec4 pValue ) {
    glUniform4i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec3 pValue ) {
    glUniform3i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::mat4 pValue, bool pTranspose ) {
    glUniformMatrix4fv( glGetUniformLocation( currentShaderProgram, pUniformName.c_str() ), 1, pTranspose, glm::value_ptr( pValue ) );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::mat3 pValue, bool pTranspose ) {
    glUniformMatrix3fv( glGetUniformLocation( pProgram, pUniformName.c_str() ), 1, pTranspose, glm::value_ptr( pValue ) );
}
