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

unsigned int gui::quadColors[MAX_QUAD_COUNT] = {};

unsigned int gui::colorQuadVAO;
unsigned int gui::colorQuadShader;
gl::StreamStorage gui::colorQuadBuffer;

void gui::initColorQuadVAO() 
{
    puts( "Initializeing Color Quad VAO..." );
    glCreateVertexArrays( 1, &colorQuadVAO );
    glBindVertexArray( colorQuadVAO );

    glVertexArrayElementBuffer( colorQuadVAO, gl::quadEBO.ID );
    glVertexArrayVertexBuffer( colorQuadVAO, 0, gl::quadVBO.ID, 0, sizeof( glm::vec2 ) );
    //gl::setVertexAttrib( colorQuadVAO, 0, 0, 2, GL_FLOAT, 0 );

    colorQuadBuffer = gl::StreamStorage( "QuadColorBuffer", sizeof( unsigned int ) * MAX_QUAD_COUNT, GL_MAP_WRITE_BIT );
    gl::setStorageTarget( colorQuadBuffer, GL_UNIFORM_BUFFER );

    glBindVertexArray( 0 );
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
    //printf( "Uploading %d bytes to storage %s...\n", sizeof( unsigned int )*MAX_QUAD_COUNT, gl::getStorage( colorQuadBuffer ).name.c_str() ); 
    //gl::uploadStorage( colorQuadBuffer, sizeof( unsigned int ) * MAX_QUAD_COUNT, &quadColors[0] );
}

void gui::renderColorQuads() 
{
    glDepthFunc( GL_LEQUAL );

    glBindVertexArray( colorQuadVAO );
    shader::use( colorQuadShader );

    glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_QUAD_COUNT );

    shader::unuse();
    glBindVertexArray( 0 );

    glDepthFunc( GL_LESS );
}

