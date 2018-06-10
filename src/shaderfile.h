#pragma once
#include <glew.h>
#include <string>
#include <vector>
#include <unordered_map>
namespace shader 
{
	namespace Loader 
	{

		void buildShaderPrograms();

		void compileShader( unsigned int pShaderIndex );
		void linkProgram( unsigned int pProgramID );
		void compileAndLink();
	}
	class Shaderfile
	{
		public:
		Shaderfile()
		{}
		Shaderfile( std::string pFilename )
			:filename( pFilename )
		{}
		std::string filename;

		static std::string SHADER_DIR;
		static void setShaderDirectory( std::string& pDirectory );
		static void resetShaderDirectory();
	};
}
