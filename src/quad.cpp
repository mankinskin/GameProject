#include "quad.h"
#include "shader.h"
#include "vao.h"
#include <algorithm>
#include "gldebug.h"
#include "framebuffer.h"
#include "primitives.h"
#include "quadcolors.h"

gl::StreamStorage<glm::vec4> gui::quadBuffer;

void gui::initQuadBuffer()
{
    quadBuffer = gl::StreamStorage<glm::vec4>( "QuadBuffer", 
            MAX_QUAD_COUNT, GL_MAP_WRITE_BIT, &QuadID::container[0] );
    quadBuffer.setTarget( GL_UNIFORM_BUFFER );
}

void gui::updateQuadBuffer()
{
    //printf( "quadCount: %u\n", quadCount );
    //for( unsigned int q = 0; q < quadCount; ++q ) {
    //    printf( "%f, %f, %f, %f\n", allQuads[q].x, allQuads[q].y, allQuads[q].z, allQuads[q].w );
    //}
    if ( QuadID::container.size() ) {
        gl::uploadStorage( quadBuffer, 
                sizeof( glm::vec4 )*QuadID::container.size(), &QuadID::container[0] );
    }
}

void gui::QuadID::move( const glm::vec2 pV ) const
{
    this->operator*().data += glm::vec4( pV.x, pV.y, 0.0f, 0.0f );
}

void gui::QuadID::resize( const glm::vec2 pV ) const
{
    this->operator*().data += glm::vec4( 0.0f, 0.0f, pV.x, pV.y );
}
void gui::QuadID::color( const gl::ColorID pColor ) const
{
    colorQuad( index, pColor );
}

//void gui::setQuadPos( const Quad pQuad, const glm::vec2 pPos )
//{
//    std::memcpy( &getQuadData( pQuad ), &pPos, sizeof( glm::vec2 ) );
//}
