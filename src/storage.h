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

	template<typename T>
	struct Storage 
	{
		using size_type = size_t;
        Storage()
        {}
        Storage( std::string pName, unsigned int pSize, 
                int pFlags, void* pData = nullptr )
			:name( pName ), capacity( pSize * sizeof(T) ), flags( pFlags )
        {
	        glCreateBuffers( 1, &ID );
	        glNamedBufferStorage( ID, capacity, pData, flags );
        }
        
		std::string name;
		unsigned int ID;
		unsigned int capacity;
		int flags;
		unsigned int target;
		unsigned int binding;
	};

	template<typename T>
	struct StreamStorage 
		: public Storage<T>
    {
		StreamStorage()
        {} 
		StreamStorage( std::string pName, unsigned int pSize, 
                int pFlags, void* pData = nullptr )
			: Storage<T>( pName, pSize, pFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT, pData )
		{
            mappedPtr = glMapNamedBufferRange( Storage<T>::ID, 0, Storage<T>::capacity, Storage<T>::flags );
            if ( !mappedPtr ) {
                debug::pushError( "Failed to map Storage " + Storage<T>::name + " !\n" );
            }
        }
        constexpr T& operator[]( const typename Storage<T>::size_type N )
        {
            return *((T*)mappedPtr + N);
        }

        void* mappedPtr;
	};

	unsigned int getNewTargetBinding( const unsigned int pTarget );
	template<typename T>
		void setStorageTarget( Storage<T>& pStorage, const unsigned int pTarget )
		{
			pStorage.target = pTarget;
			pStorage.binding = getNewTargetBinding( pStorage.target );
			glBindBufferBase( pStorage.target, pStorage.binding, pStorage.ID );	
		}
}
