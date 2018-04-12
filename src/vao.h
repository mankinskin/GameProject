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
            :name( pName )
        {
            glCreateVertexArrays( 1, &ID );
        }


        operator unsigned int()
        {
            return ID;
        }
        std::string name;
        unsigned int ID;
    };
}
