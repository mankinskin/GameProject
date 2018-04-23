#include "VAO.h"
#include "glDebug.h"
#include <algorithm>
#include "debug.h"
#include <glew.h>

void gl::VAO::vertexAttrib( unsigned int pBindingIndex, unsigned int pAttributeIndex, unsigned int pCount, unsigned int pType, unsigned int pOffset, bool pNormalize ) 
{
     if ( pType == GL_FLOAT ) {
             glVertexArrayAttribFormat( ID, pAttributeIndex, pCount, pType, pNormalize, pOffset );
     }
     else if ( pType == GL_INT || pType == GL_UNSIGNED_INT ) {
             glVertexArrayAttribIFormat( ID, pAttributeIndex, pCount, pType, pOffset );
     }
     else if ( pType == GL_DOUBLE ) {
             glVertexArrayAttribLFormat( ID, pAttributeIndex, pCount, pType, pOffset );
     }
     glVertexArrayAttribBinding( ID, pAttributeIndex, pBindingIndex );
     glEnableVertexArrayAttrib( ID, pAttributeIndex );
}

void gl::VAO::attribDivisor( unsigned int pAttrib, unsigned int pDivisor )
{
    glVertexArrayBindingDivisor( ID, pAttrib, pDivisor );
}
