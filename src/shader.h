#pragma once
#include "gl.h"
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm.hpp>
#include "storage.h"
#include <typeinfo>
#include <gtc/type_ptr.hpp>

namespace shader {

    unsigned int createModule( std::string pFileName );
    unsigned int createProgram( std::string pProgramName );
    unsigned int newProgram( std::string pProgramName, unsigned int pVertexModuleIndex, unsigned int pFragmentModuleIndex );
    unsigned int newProgram( std::string pProgramName, unsigned int pVertexModuleIndex, unsigned int pFragmentModuleIndex, unsigned int pGeometryModuleIndex );
    unsigned int newProgram( std::string pProgramName, unsigned int pComputeModuleIndex );
    void use( std::string pProgramName );
    void use( unsigned int pID );
    void unuse();
    void addVertexAttribute( unsigned int pProgram, std::string pAttributeName, unsigned int pAttributeIndex );
    void addVertexAttribute( std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex );

    void bindUniformBufferToShader( unsigned int pProgram, const gl::Storage& pStorage, std::string pBlockName );
    extern unsigned int currentShaderProgram;

    void setUniform( unsigned int pProgram, std::string pUniformName, int pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, unsigned int pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, float pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::vec3 pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::vec4 pValue ); 
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec4 pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec3 pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec4 pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec3 pValue );
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::mat4 pValue, bool pTranspose );
    void setUniform( unsigned int pProgram, std::string pUniformName, glm::mat3 pValue, bool pTranspose );
}

