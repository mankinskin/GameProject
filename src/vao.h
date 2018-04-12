#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glew.h>
#include "storage.h"

namespace gl {

    struct VAO
    {
        VAO() 
        {}
        VAO( std::string pName )
            :name( pName ), vertexBufferCount( 0 )
        {
            printf( "Creating VAO: %s...\n", name.c_str() );
            glCreateVertexArrays( 1, &ID );
        }
        void vertexAttrib( unsigned int pBindingIndex, unsigned int pAttributeIndex, 
                unsigned int pCount, unsigned int pType, 
                unsigned int pOffset, bool pNormalize = false );
        void vertexBuffer( unsigned int pBuffer, unsigned int pStride );
        void elementBuffer( unsigned int pBuffer );

        operator unsigned int()
        {
            return ID;
        }
        std::string name;
        unsigned int ID;
        unsigned int vertexBufferCount;
    };
}
