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
#include "quadcolors.h"
#include "line.h"
#include "font.h"
#include "shader_loader.h"
#include "quad.h"
#include "voxelization.h"

unsigned int Shader::currentShaderProgram;

std::vector<Shader::Program> Shader::allPrograms;
std::unordered_map<std::string, unsigned int> Shader::ShaderProgramLookup;
std::vector<Shader::Module> Shader::allModules;
std::unordered_map<std::string, unsigned int> Shader::moduleLookup;


unsigned int Shader::createModule( std::string pFileName )
{
    unsigned int index = allModules.size();
    moduleLookup.insert( std::pair<std::string, unsigned int>( pFileName, index ) );
    allModules.push_back( Module( pFileName ) );
    return index;
}

unsigned int Shader::newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Basic;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].ShaderCount = 2;
    return allPrograms[index].ID;
}

unsigned int Shader::newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex, unsigned int pGeometryShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Geometry;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].stages[2] = pGeometryShaderIndex;
    allPrograms[index].ShaderCount = 3;
    return allPrograms[index].ID;
}

unsigned int Shader::newProgram( std::string pProgramName, unsigned int pComputeShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Compute;
    allPrograms[index].stages[0] = pComputeShaderIndex;
    allPrograms[index].ShaderCount = 1;
    return allPrograms[index].ID;
}

unsigned int Shader::createProgram( std::string pProgramName )
{
    Program program;
    program.ID = glCreateProgram();
    program.name = pProgramName;
    ShaderProgramLookup.insert( std::pair<std::string, unsigned int>( pProgramName, program.ID ) );
    allPrograms.push_back( program );
    return allPrograms.size() - 1;
}

void Shader::use( unsigned int pID )
{
    currentShaderProgram = pID;
    glUseProgram( pID );
}

void Shader::use( std::string pProgramName )
{
    use( ShaderProgramLookup.find( pProgramName )->second );
}

void Shader::unuse()
{
    currentShaderProgram = {};
    glUseProgram( 0 );
}

void Shader::addVertexAttribute( unsigned int pProgramID, std::string pAttributeName, unsigned int pAttributeIndex )
{
    //binds a in variable symbol out of a Shader to an attribute index
    glBindAttribLocation( pProgramID, pAttributeIndex, pAttributeName.c_str() );
}

void Shader::addVertexAttribute( std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex )
{
    auto it = ShaderProgramLookup.find( pProgramName.c_str() );
    if ( it == ShaderProgramLookup.end() ) {
        debug::pushError( "addVertexAttribute():/nCould not find Shader Program " + pProgramName + "!/n" );
        return;
    }
    addVertexAttribute( it->second, pAttributeName, pAttributeIndex );
}

void Shader::setUniform( unsigned int pProgram, std::string pUniformName, int pValue ) {
    glUniform1i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, unsigned int pValue ) {
    glUniform1ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, float pValue ) {
    glUniform1f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::vec3 pValue ) {
    glUniform3f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::vec4 pValue ) {
    glUniform4f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec4 pValue ) {
    glUniform4ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec3 pValue ) {
    glUniform3ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec4 pValue ) {
    glUniform4i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec3 pValue ) {
    glUniform3i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::mat4 pValue, bool pTranspose ) {
    glUniformMatrix4fv( glGetUniformLocation( currentShaderProgram, pUniformName.c_str() ), 1, pTranspose, glm::value_ptr( pValue ) );
}
void Shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::mat3 pValue, bool pTranspose ) {
    glUniformMatrix3fv( glGetUniformLocation( pProgram, pUniformName.c_str() ), 1, pTranspose, glm::value_ptr( pValue ) );
}
