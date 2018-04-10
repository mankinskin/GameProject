#pragma once
#include <string>
#include <glew.h>
#include "debug.h"

namespace gl
{
	extern int SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
	extern int UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	extern int MAX_UNIFORM_BUFFER_BINDINGS;
	extern int MIN_MAP_BUFFER_ALIGNMENT;

	struct Storage 
	{
        Storage()
        {}
        Storage( std::string pName, unsigned int pCapacity, 
                int pFlags, void* pData = nullptr )
			:name( pName ), capacity( pCapacity ), flags( pFlags )
        {
	        glCreateBuffers( 1, &ID );
	        glNamedBufferStorage( ID, pCapacity, pData, pFlags );
        }
        
		std::string name;
		unsigned int ID;
		unsigned int capacity;
		int flags;
		unsigned int target;
		unsigned int binding;
	};

	struct StreamStorage 
        :public Storage
	{
		StreamStorage()
        {} 
		StreamStorage( std::string pName, unsigned int pCapacity, 
                int pFlags, void* pData = nullptr )
			:Storage( pName, pCapacity, pFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT, pData )
		{
            mappedPtr = glMapNamedBufferRange( ID, 0, capacity, flags );
            if ( !mappedPtr ) {
                debug::pushError( "Failed to map Storage " + name + " !\n" );
            }
        }
        template<typename T>
            T& access( size_t N )
            {
                return *((T*)mappedPtr + N);
            }
        void* mappedPtr;
	};

	void setStorageTarget( Storage& pStorage, const unsigned int pTarget );
}
