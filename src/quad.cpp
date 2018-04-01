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

std::vector<glm::vec4> gui::allQuads;
unsigned int gui::quadBuffer;
unsigned int gui::quadIndexShader;
unsigned int gui::quadIndexVAO;
std::vector<unsigned int> quadIndexMap;
std::vector<float> quadDepthMap;
unsigned int gui::quadIndexBuffer;


void gui::updateQuadBuffer()
{
	if ( allQuads.size() ) 
    {
		vao::uploadStorage( quadBuffer, 
                sizeof( glm::vec4 )*allQuads.size(), &allQuads[0] );
	}
}

void gui::rasterQuadIndices()
{
	if ( allQuads.size() ) 
    {
		glDepthMask( 0 );
		glDepthFunc( GL_LEQUAL );
		glBindVertexArray( quadIndexVAO );
		shader::use( quadIndexShader );

		glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 
                0, allQuads.size() );

		shader::unuse();
		glBindVertexArray( 0 );
		glDepthFunc( GL_LESS );
		glDepthMask( 1 );
	}
}

unsigned int gui::createQuad( const float pPosX, const float pPosY, 
        const float pWidth, const float pHeight )
{
	allQuads.emplace_back( pPosX, pPosY, pWidth, pHeight );
    printf( "Creating Quad %u: %f %f %f %f\n", 
            allQuads.size(), pPosX, pPosY, pWidth, pHeight );
	return allQuads.size();
}

unsigned int gui::createQuad( const glm::vec4 pQuad )
{
	allQuads.push_back( pQuad );
    printf( "Creating Quad %u: %f %f %f %f\n", 
            allQuads.size(), pQuad.x, pQuad.y, pQuad.z, pQuad.w );
	return allQuads.size();
}
void gui::reserveQuads( const unsigned int pCount )
{
	allQuads.reserve( allQuads.size() + pCount );
}
void gui::initQuadBuffer()
{
	quadBuffer = vao::createStorage( "QuadBuffer", 
            MAX_QUAD_COUNT * sizeof( glm::vec4 ), 0, 
            GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS );
	vao::createStream( quadBuffer, GL_MAP_WRITE_BIT );
	vao::bindStorage( GL_UNIFORM_BUFFER, quadBuffer );

	glCreateVertexArrays( 1, &quadIndexVAO );
	glVertexArrayElementBuffer( quadIndexVAO, gl::quadEBO + 1 );
	glVertexArrayVertexBuffer( quadIndexVAO, 0, 
            gl::quadVBO + 1, 0, sizeof( glm::vec2 ) );

	vao::setVertexAttrib( quadIndexVAO, 0, 0, 2, GL_FLOAT, 0 );

	quadIndexMap.resize( gl::getWidth() * gl::getHeight() );
	quadDepthMap.resize( gl::getWidth() * gl::getHeight() );
}

void gui::initQuadIndexBuffer()
{
	quadIndexBuffer = vao::createStorage( "QuadIndexBuffer", 
            gl::getWidth() * gl::getHeight() * sizeof( unsigned char ), 
            nullptr, GL_MAP_READ_BIT | vao::MAP_PERSISTENT_FLAGS );
	vao::bindStorage( GL_PIXEL_PACK_BUFFER, quadIndexBuffer );
	vao::createStream( quadIndexBuffer, 
            GL_MAP_READ_BIT | vao::MAP_PERSISTENT_FLAGS );
}

void gui::readQuadIndexBuffer()
{
	glBindBuffer( GL_PIXEL_PACK_BUFFER, quadIndexBuffer + 1 );
	glReadPixels( 0, 0, gl::getWidth(), gl::getHeight(), 
            GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0 );
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

void gui::clearQuads()
{
	allQuads.clear();
}

unsigned int gui::readQuadIndexMap( const unsigned int pPos )
{
	return *( ( unsigned char* )
            vao::getMappedPtr( quadIndexBuffer ) + pPos );
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

void gui::moveQuad( const unsigned int pQuad, const glm::vec2 pOffset )
{
	allQuads[pQuad - 1] += glm::vec4( pOffset.x, pOffset.y, 0.0f, 0.0f );
}

void gui::resizeQuad( const unsigned int pQuad, const glm::vec2 pOffset )
{
	allQuads[pQuad - 1] += glm::vec4( 0.0f, 0.0f, pOffset.x, pOffset.y );
}

void gui::setQuadPos( const unsigned int pQuad, const glm::vec2 pPos )
{
	std::memcpy( &allQuads[pQuad - 1], &pPos, sizeof( glm::vec2 ) );
}
gui::QuadData gui::getQuadData( unsigned int pQuadIndex )
{
    return allQuads[pQuadIndex];
}
void gui::colorQuad( Quad pQuad, gl::ColorIt pColor )
{
    printf( "Coloring Quad %u with color %u\n", pQuad.index, pColor.index );
    quadColors[pQuad.index-1] = pColor.index;
}
