#include "quad.h"
#include "shader.h"
#include "vao.h"
#include <algorithm>
#include "gldebug.h"
#include "framebuffer.h"
#include "primitives.h"

gl::StreamStorage<glm::vec4> gui::quadBuffer;
std::array<glm::vec4, gui::MAX_QUAD_COUNT> allQuads;
size_t gui::quadCount = 0;


gui::Quad::Quad( glm::vec4 pData ) 
    :ID( ++quadCount )
{
    allQuads[ ID - 1 ] = pData;
}

void gui::initQuadBuffer()
{
    quadBuffer = gl::StreamStorage<glm::vec4>( "QuadBuffer", 
            MAX_QUAD_COUNT, GL_MAP_WRITE_BIT, &allQuads[0] );
    quadBuffer.setTarget( GL_UNIFORM_BUFFER );
}

void gui::updateQuadBuffer()
{
    //printf( "quadCount: %u\n", quadCount );
    //for( unsigned int q = 0; q < quadCount; ++q ) {
    //    printf( "%f, %f, %f, %f\n", allQuads[q].x, allQuads[q].y, allQuads[q].z, allQuads[q].w );
    //}
    if ( quadCount ) {
        gl::uploadStorage( quadBuffer, 
                sizeof( glm::vec4 )*quadCount, &allQuads[0] );
    }
}

void gui::moveQuad( const Quad pQuad, const glm::vec2 pOffset )
{
    printf("Moving Quad %d\n", pQuad.ID );
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

glm::vec4& gui::getQuadData( const Quad pQuad )
{
    return allQuads[ pQuad.ID - 1 ]; 
}

