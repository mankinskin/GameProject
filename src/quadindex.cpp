#include "quadindex.h"
#include "primitives.h"
#include "shader.h"
#include "quad.h"
#include "viewport.h"

unsigned int gui::quadIndexShader;
gl::VAO gui::quadIndexVAO;
std::vector<unsigned int> quadIndexMap;
std::vector<float> quadDepthMap;

gl::StreamStorage<unsigned int> gui::quadIndexBuffer;

void gui::initQuadIndexBuffer()
{
    quadIndexVAO = gl::VAO( "quadIndexVAO" );

    quadIndexVAO.elementBuffer( gl::quadEBO );
    quadIndexVAO.vertexBuffer( 0, gl::quadVBO );
    quadIndexVAO.vertexAttrib( 0, 0, 2, GL_FLOAT, 0 );

    quadIndexBuffer = gl::StreamStorage<unsigned int>( "QuadIndexBuffer", 
            gl::getWidth() * gl::getHeight(), GL_MAP_READ_BIT );

    quadIndexMap.resize( gl::getWidth() * gl::getHeight() );
    quadDepthMap.resize( gl::getWidth() * gl::getHeight() );
}

void gui::initQuadIndexShader()
{
    quadIndexShader = Shader::newProgram( "quadIndexShader", 
            Shader::createModule( "quadIndexShader.vert" ), 
            Shader::createModule( "quadIndexShader.frag" ) );
    Shader::addVertexAttribute( quadIndexShader, "corner_pos", 0 );
}

void gui::setupQuadIndexShader()
{
    Shader::bindUniformBufferToShader( quadIndexShader, 
            quadBuffer, "QuadBuffer" );
}

void gui::rasterQuadIndices()
{
    if ( quadCount ) {
        glDepthMask( 0 );
        glDepthFunc( GL_LEQUAL );
        quadIndexVAO.bind();
        Shader::use( quadIndexShader );

        glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 
                0, quadCount );

        Shader::unuse();
        quadIndexVAO.unbind();
        glDepthFunc( GL_LESS );
        glDepthMask( 1 );
    }
}

void gui::readQuadIndexBuffer()
{
    glBindBuffer( GL_PIXEL_PACK_BUFFER, quadIndexBuffer.ID );
    glReadPixels( 0, 0, gl::getWidth(), gl::getHeight(), 
            GL_RED_INTEGER, GL_UNSIGNED_INT, 0 );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
}

unsigned int gui::readQuadIndexMap( const unsigned int pPos )
{
    return *((unsigned int*)quadIndexBuffer.mappedPtr + pPos);
}

unsigned int gui::readQuadIndexMap( 
        const unsigned int pXPos, const unsigned int pYPos )
{
    return readQuadIndexMap( 
            ( gl::Viewport::current->width * pYPos ) + pXPos );
}

float gui::readQuadDepthMap( const unsigned int pPos )
{
    return quadDepthMap[pPos];
}

float gui::readQuadDepthMap( const unsigned int pXPos, 
        const unsigned int pYPos )
{
    return readQuadDepthMap( 
            ( gl::Viewport::current->width * pYPos ) + pXPos );
}

