#pragma once
#include <glew.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace shader 
{
	class Shaderfile
	{
		static std::string SHADER_DIR;
		public:
		Shaderfile()
		{}
		Shaderfile( std::string pFilename )
			:filename( pFilename )
		{}
		std::string filename;

		static void setShaderDirectory( std::string& pDirectory );
		static void resetShaderDirectory();
	};
}
