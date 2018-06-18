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
static shader::Program colorQuadShader;

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
    colorQuadShader = shader::Program( "colorQuadShader", 
            shader::Stage( "colorQuadShader.vert" ), 
            shader::Stage( "colorQuadShader.frag" ) );
    colorQuadShader.addVertexAttribute( "corner_pos", 0 );
}

void gui::setupColorQuadShader() 
{
    colorQuadShader.build();
    colorQuadShader.bindUniformBuffer( quadBuffer, "QuadBuffer" );
    colorQuadShader.bindUniformBuffer( colorQuadBuffer, "QuadColorBuffer" );
    colorQuadShader.bindUniformBuffer( gl::colorBuffer, "ColorBuffer" );
}

void gui::updateColorQuads()
{
    gl::uploadStorage( colorQuadBuffer, sizeof( unsigned int ) * QuadID::container.size(), &quadColors[0] );
}

void gui::renderColorQuads() 
{
    glDepthFunc( GL_LEQUAL );

    colorQuadVAO.bind();
    colorQuadShader.use();

    glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, QuadID::container.size() );

    shader::Program::unuse();
    colorQuadVAO.unbind();

    glDepthFunc( GL_LESS );
}

void gui::colorQuad( const QuadID pID, const gl::ColorID pColor )
{
    printf( "Coloring Quad %u with color %u\n", pID, pColor.index );
    quadColors[ pID.index ] = pColor.index;
}

