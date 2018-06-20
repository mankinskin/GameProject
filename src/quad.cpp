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
    if ( QuadID::container.size() ) {
        gl::uploadStorage( quadBuffer, 
                sizeof( glm::vec4 )*QuadID::container.size(), &QuadID::container[0] );
    }
}

void gui::Quad::setPos( const glm::vec2 p )
{
    data = glm::vec4( p.x, p.y, data.z, data.w );
}

void gui::Quad::move( const glm::vec2 v )
{
    data += glm::vec4( v.x, v.y, 0.0f, 0.0f );
}

void gui::Quad::resize( const glm::vec2 v )
{
    data += glm::vec4( 0.0f, 0.0f, v.x, v.y );
}

void gui::QuadID::setPos( const glm::vec2 p ) const
{
    this->get().setPos( p );
}

void gui::QuadID::move( const glm::vec2 v ) const
{
    this->get().move( v );
}

void gui::QuadID::resize( const glm::vec2 v ) const
{
    this->get().resize( v );
}

void gui::QuadID::color( const gl::ColorID c ) const
{
    colorQuad( index, c );
}

void gui::setQuadPos( const QuadID q, const glm::vec2 p )
{
    q.setPos( p );
}
void gui::moveQuad( const QuadID q, const glm::vec2 v )
{
    q.move( v );
}

