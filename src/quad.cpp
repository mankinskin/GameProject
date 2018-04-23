#include "Quad.h"
#include "shader.h"
#include "VAO.h"
#include <algorithm>
#include "gldebug.h"
#include "framebuffer.h"
#include "primitives.h"
#include "quadcolors.h"

gl::StreamStorage<glm::vec4> gui::quadBuffer;
std::array<glm::vec4, gui::MAX_QUAD_COUNT> allQuads;
size_t gui::quadCount = 0;


gui::Quad::Quad( glm::vec4 pData ) 
    :ID( ++quadCount )
{
    printf("Creating Quad %u\n", ID );
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

void gui::Quad::move( const glm::vec2 pV ) const
{
    printf("Moving Quad %d\n", ID );
    getQuadData( ID ) += glm::vec4( pV.x, pV.y, 0.0f, 0.0f );
}

void gui::Quad::resize( const glm::vec2 pV ) const
{
    getQuadData( ID ) += glm::vec4( 0.0f, 0.0f, pV.x, pV.y );
}
void gui::Quad::color( const gl::ColorIt pColor ) const
{
    colorQuad( ID, pColor );
}

//void gui::setQuadPos( const Quad pQuad, const glm::vec2 pPos )
//{
//    std::memcpy( &getQuadData( pQuad ), &pPos, sizeof( glm::vec2 ) );
//}

glm::vec4& gui::getQuadData( const unsigned int pID )
{
    return allQuads[ pID - 1 ]; 
}
