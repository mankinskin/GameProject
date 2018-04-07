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
unsigned int gui::quadIndexVAO;
std::vector<unsigned int> quadIndexMap;
std::vector<float> quadDepthMap;
gl::Storage gui::quadIndexBuffer;
gl::Storage gui::quadBuffer;

size_t quadCount = 0;

void gui::updateQuadBuffer()
{
	if ( quadCount ) 
    {
		//gl::uploadStorage( quadBuffer, 
        //        sizeof( glm::vec4 )*allQuads.size(), &allQuads[0] );
	}
}

void gui::rasterQuadIndices()
{
	if ( quadCount ) 
    {
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

unsigned int gui::createQuad( const float pPosX, const float pPosY, 
        const float pWidth, const float pHeight )
{
	return createQuad( glm::vec4( pPosX, pPosY, pWidth, pHeight ) );
}

unsigned int gui::createQuad( const glm::vec4 pQuad )
{
    getQuadData( ++quadCount ) = pQuad;
    printf( "Creating Quad %d ( %f, %f, %f, %f )\n", quadCount, 
            getQuadData( quadCount ).x, getQuadData( quadCount ).y, 
            getQuadData( quadCount ).z, getQuadData( quadCount ).w );
	return quadCount;
}

void gui::reserveQuads( const unsigned int pCount )
{
	//allQuads.reserve( allQuads.size() + pCount );
}

void gui::initQuadBuffer()
{
	quadBuffer = gl::createStorage( "QuadBuffer", 
            MAX_QUAD_COUNT * sizeof( glm::vec4 ), 
            GL_MAP_WRITE_BIT | gl::MAP_PERSISTENT_FLAGS );
	//gl::createStream( quadBuffer, GL_MAP_WRITE_BIT );
	gl::setStorageTarget( quadBuffer, GL_UNIFORM_BUFFER );

	glCreateVertexArrays( 1, &quadIndexVAO );
	glVertexArrayElementBuffer( quadIndexVAO, gl::quadEBO.ID );
	glVertexArrayVertexBuffer( quadIndexVAO, 0, 
            gl::quadVBO.ID, 0, sizeof( glm::vec2 ) );

	//gl::setVertexAttrib( quadIndexVAO, 0, 0, 2, GL_FLOAT, 0 );

    reserveQuads( MAX_QUAD_COUNT );
	quadIndexMap.resize( gl::getWidth() * gl::getHeight() );
	quadDepthMap.resize( gl::getWidth() * gl::getHeight() );
}

void gui::initQuadIndexBuffer()
{
	quadIndexBuffer = gl::createStorage( "QuadIndexBuffer", 
            gl::getWidth() * gl::getHeight() * sizeof( unsigned int ), 
            GL_MAP_READ_BIT | gl::MAP_PERSISTENT_FLAGS );
	gl::setStorageTarget( quadIndexBuffer, GL_PIXEL_PACK_BUFFER );
	//gl::createStream( quadIndexBuffer, GL_MAP_READ_BIT | gl::MAP_PERSISTENT_FLAGS );
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
	return *( ( unsigned int* )
            gl::getMappedPtr( quadIndexBuffer ) + pPos );
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

glm::vec4& gui::getQuadData( const Quad pQuad )
{
    return *( (glm::vec4*)gl::getMappedPtr( quadBuffer ) + ( pQuad.index - 1 ) );
}

void gui::colorQuad( Quad pQuad, gl::ColorIt pColor )
{
    printf( "Coloring Quad %u with color %u\n", pQuad.index, pColor.index );
    quadColors[pQuad.index - 1] = pColor.index;
}
