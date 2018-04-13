#include "quad.h"
#include "shader.h"
#include "vao.h"
#include <algorithm>
#include "input.h"
#include "mouse.h"
#include "gldebug.h"
#include "framebuffer.h"
#include "viewport.h"
#include "primitives.h"

unsigned int gui::quadIndexShader;
gl::VAO gui::quadIndexVAO;
std::vector<unsigned int> quadIndexMap;
std::vector<float> quadDepthMap;

gl::StreamStorage<unsigned int> gui::quadIndexBuffer;

gl::StreamStorage<glm::vec4> gui::quadBuffer;
std::array<glm::vec4, gui::MAX_QUAD_COUNT> allQuads;
size_t gui::quadCount = 0;


gui::Quad::Quad( const unsigned int in )
    :index( in )
{}
gui::Quad::Quad( glm::vec4 pData ) 
    :index( createQuad( pData ) )
{}

void gui::updateQuadBuffer()
{
    if ( quadCount ) {
        gl::uploadStorage( quadBuffer, 
                sizeof( glm::vec4 )*quadCount, &allQuads[0] );
    }
}

void gui::rasterQuadIndices()
{
    if ( quadCount ) {
        glDepthMask( 0 );
        glDepthFunc( GL_LEQUAL );
        glBindVertexArray( quadIndexVAO );
        shader::use( quadIndexShader );

        glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 
                0, quadCount );

        shader::unuse();
        glBindVertexArray( 0 );
        glDepthFunc( GL_LESS );
        glDepthMask( 1 );
    }
}

void gui::initQuadBuffer()
{
    quadIndexVAO = gl::VAO( "quadIndexVAO" );
    quadBuffer = gl::StreamStorage<glm::vec4>( "QuadBuffer", 
            MAX_QUAD_COUNT, GL_MAP_WRITE_BIT, &allQuads[0] );
    quadBuffer.setTarget( GL_UNIFORM_BUFFER );

    quadIndexVAO.elementBuffer( gl::quadEBO );
    quadIndexVAO.vertexBuffer( gl::quadVBO );

    quadIndexVAO.vertexAttrib( 0, 0, 2, GL_FLOAT, 0 );

    quadIndexMap.resize( gl::getWidth() * gl::getHeight() );
    quadDepthMap.resize( gl::getWidth() * gl::getHeight() );
}

void gui::initQuadIndexBuffer()
{
    quadIndexBuffer = gl::StreamStorage<unsigned int>( "QuadIndexBuffer", 
            gl::getWidth() * gl::getHeight(), GL_MAP_READ_BIT );
    quadIndexBuffer.setTarget( GL_PIXEL_PACK_BUFFER );
}

void gui::readQuadIndexBuffer()
{
    glBindBuffer( GL_PIXEL_PACK_BUFFER, quadIndexBuffer.ID );
    glReadPixels( 0, 0, gl::getWidth(), gl::getHeight(), 
            GL_RED_INTEGER, GL_UNSIGNED_INT, 0 );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
}

void gui::initQuadIndexShader()
{
    quadIndexShader = shader::newProgram( "quadIndexShader", 
            shader::createModule( "quadIndexShader.vert" ), 
            shader::createModule( "quadIndexShader.frag" ) );
    shader::addVertexAttribute( quadIndexShader, "corner_pos", 0 );
}

void gui::setupQuadIndexShader()
{
    shader::bindUniformBufferToShader( quadIndexShader, 
            quadBuffer, "QuadBuffer" );
}

unsigned int gui::readQuadIndexMap( const unsigned int pPos )
{
    return quadIndexMap[ pPos ];
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

void gui::moveQuad( const Quad pQuad, const glm::vec2 pOffset )
{
    printf("Moving Quad %d\n", pQuad.index );
    getQuadData( pQuad ) += glm::vec4( pOffset.x, pOffset.y, 0.0f, 0.0f );
}

void gui::moveQuadScaled( const Quad pQuad, const glm::vec2 pOffset, const glm::vec2 scale)
{
    moveQuad( pQuad, pOffset * scale );
}

void gui::resizeQuad( const Quad pQuad, const glm::vec2 pOffset )
{
    getQuadData( pQuad ) += glm::vec4( 0.0f, 0.0f, pOffset.x, pOffset.y );
}

void gui::resizeQuadScaled( const Quad pQuad, const glm::vec2 pOffset, const glm::vec2 scale)
{
    resizeQuad(pQuad, pOffset * scale );
}

void gui::setQuadPos( const Quad pQuad, const glm::vec2 pPos )
{
    std::memcpy( &getQuadData( pQuad ), &pPos, sizeof( glm::vec2 ) );
}

unsigned int gui::createQuad( const float pPosX, const float pPosY, 
        const float pWidth, const float pHeight )
{
    return createQuad( glm::vec4( pPosX, pPosY, pWidth, pHeight ) );
}

unsigned int gui::createQuad( const glm::vec4 pQuadData )
{
    getQuadData( ++quadCount ) = pQuadData;
    printf( "Creating Quad %d ( %f, %f, %f, %f )\n", quadCount, 
            getQuadData( quadCount ).x, getQuadData( quadCount ).y, 
            getQuadData( quadCount ).z, getQuadData( quadCount ).w );
    return quadCount;
}

glm::vec4& gui::getQuadData( const Quad pQuad )
{
    glm::vec4& r = allQuads[ pQuad.index - 1 ]; 
    printf( "getQuadData : ( %f, %f, %f, %f )\n", r.x, r.y, r.z, r.w );
    return r;
}

