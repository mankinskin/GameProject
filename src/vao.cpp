#include "vao.h"
#include "gldebug.h"
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
 
void gl::VAO::vertexBuffer( unsigned int pBuffer, unsigned int pStride )
{
    glVertexArrayVertexBuffer( ID, vertexBufferCount++, pBuffer, 0, pStride );
} 

void gl::VAO::elementBuffer( unsigned int pBuffer )
{
    glVertexArrayElementBuffer( ID, pBuffer );
}
