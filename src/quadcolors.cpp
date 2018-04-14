#include "quadcolors.h"
#include <algorithm>
#include "gui.h"
#include "vao.h"
#include "shader.h"
#include "gl.h"
#include "gldebug.h"
#include "texture.h"
#include "primitives.h"
#include "quad.h"
#include <array>

std::array<unsigned int, gui::MAX_QUAD_COUNT> quadColors;
gl::StreamStorage<unsigned int> gui::colorQuadBuffer;

gl::VAO gui::colorQuadVAO;
unsigned int gui::colorQuadShader;

void gui::initColorQuadVAO() 
{
    colorQuadVAO = gl::VAO( "colorQuadVAO" );

    colorQuadVAO.elementBuffer( gl::quadEBO );
    colorQuadVAO.vertexBuffer( 0, gl::quadVBO );
    colorQuadVAO.vertexAttrib( 0, 0, 2, GL_FLOAT, 0 );

    colorQuadBuffer = gl::StreamStorage<unsigned int>( "QuadColorBuffer", 
            MAX_QUAD_COUNT, GL_MAP_WRITE_BIT );
    colorQuadBuffer.setTarget( GL_UNIFORM_BUFFER );
}

void gui::initColorQuadShader() 
{
    colorQuadShader = shader::newProgram( "colorQuadShader", 
            shader::createModule( "colorQuadShader.vert" ), 
            shader::createModule( "colorQuadShader.frag" ) );
    shader::addVertexAttribute( colorQuadShader, "corner_pos", 0 );
}

void gui::setupColorQuadShader() 
{
    shader::bindUniformBufferToShader( colorQuadShader, quadBuffer, "QuadBuffer" );
    shader::bindUniformBufferToShader( colorQuadShader, colorQuadBuffer, "QuadColorBuffer" );
    shader::bindUniformBufferToShader( colorQuadShader, gl::colorBuffer, "ColorBuffer" );
}

void gui::updateColorQuads()
{
    gl::uploadStorage( colorQuadBuffer, sizeof( unsigned int ) * quadCount, &quadColors[0] );
}

void gui::renderColorQuads() 
{
    glDepthFunc( GL_LEQUAL );

    colorQuadVAO.bind();
    shader::use( colorQuadShader );

    glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, quadCount );

    shader::unuse();
    colorQuadVAO.unbind();

    glDepthFunc( GL_LESS );
}

void gui::colorQuad( Quad pQuad, gl::ColorIt pColor )
{
    printf( "Coloring Quad %u with color %u\n", pQuad.index, pColor.index );
    quadColors[pQuad.index] = pColor.index;
}

