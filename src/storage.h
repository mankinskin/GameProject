#pragma once
#include <string>
#include <glew.h>

namespace gl
{
	extern int SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
	extern int UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	extern int MAX_UNIFORM_BUFFER_BINDINGS;
	extern int MIN_MAP_BUFFER_ALIGNMENT;
	const int MAP_PERSISTENT_FLAGS = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	struct Storage 
	{
		Storage()
		{}
		Storage( std::string pName, unsigned int pID, unsigned int pCapacity, unsigned int pFlags)
			:name( pName ), ID( pID ), capacity( pCapacity ), flags( pFlags )
		{}
		std::string name;
		unsigned int ID;
		unsigned int capacity;
		unsigned int flags;
		unsigned int target;
		unsigned int binding;
	};

	Storage createStorage( std::string pName, unsigned int pCapacity, unsigned int pFlags, void* pData = nullptr );
	void setStorageTarget( Storage& pStorage, const unsigned int pTarget );
}
