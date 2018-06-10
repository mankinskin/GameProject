#pragma once
#include "gl.h"
#include "storage.h"
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm.hpp>
#include <typeinfo>
#include <gtc/type_ptr.hpp>

namespace shader 
{
	enum class ShaderType 
	{
		Invalid,
		Vertex,
		Fragment,
		Geometry,
		Compute
	};

	enum class ProgramType 
	{
		Invalid,
		Basic,
		Geometry,
		Compute
	};

	struct Program 
	{
		GLuint ID;
		std::string name;
		ProgramType type;
		unsigned int stages[4];
		unsigned int shaderCount;
	};

	struct Shader 
	{
		Shader( std::string& pFileName ) 
			, content( "" )
			, ID( 0 )
			, type( ShaderType::Vertex ) 
		{}
		std::string content;
		std::string fileName;
		GLuint ID;
		ShaderType type;
	};
	
	extern std::vector<Program> allPrograms;
	extern std::unordered_map<std::string, unsigned int> programLookup;
	extern std::vector<Shader> allShaders;
	extern std::unordered_map<std::string, unsigned int> shaderLookup;

	void buildShaderPrograms();

	void compileShader( unsigned int pShaderIndex );
	void linkProgram( unsigned int pProgramID );
	void compileAndLink();
	unsigned int createShader( std::string pFileName );
	unsigned int createProgram( std::string pProgramName );
	unsigned int newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex );
	unsigned int newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex, unsigned int pGeometryShaderIndex );
	unsigned int newProgram( std::string pProgramName, unsigned int pComputeShaderIndex );
	void use( std::string pProgramName );
	void use( unsigned int pID );
	void unuse();
	void addVertexAttribute( unsigned int pProgram, std::string pAttributeName, unsigned int pAttributeIndex );
	void addVertexAttribute( std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex );

	template<typename T>
		void bindUniformBufferToShader( unsigned int pProgram, const gl::Storage<T>& pStorage, std::string pBlockName )
		{
			int blockIndex = glGetUniformBlockIndex( pProgram, pBlockName.c_str() );
			if ( blockIndex < 0 ) {
				debug::pushError( "invalid uniform block name " + pBlockName + "!" );
				return;
			}
			glUniformBlockBinding( pProgram, blockIndex, pStorage.binding );
		}
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

