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
unsigned int gui::colorQuadBuffer;

void gui::initColorQuadVAO() 
{
    puts( "Initializeing Color Quad VAO..." );
    glCreateVertexArrays( 1, &colorQuadVAO );
    glBindVertexArray( colorQuadVAO );

    glVertexArrayElementBuffer( colorQuadVAO, gl::quadEBO + 1 );
    glVertexArrayVertexBuffer( colorQuadVAO, 0, gl::quadVBO + 1, 0, sizeof( glm::vec2 ) );
    vao::setVertexAttrib( colorQuadVAO, 0, 0, 2, GL_FLOAT, 0 );

    colorQuadBuffer = vao::createStorage( "QuadColorBuffer", sizeof( unsigned int ) * MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS );
    vao::createStream( colorQuadBuffer, GL_MAP_WRITE_BIT );
    vao::bindStorage( GL_UNIFORM_BUFFER, colorQuadBuffer );

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
    if( !allQuads.size() ){
        return;
    }
    //printf( "Uploading %d bytes to storage %s...\n", sizeof( unsigned int )*MAX_QUAD_COUNT, vao::getStorage( colorQuadBuffer ).name.c_str() ); 
    vao::uploadStorage( colorQuadBuffer, sizeof( unsigned int ) * MAX_QUAD_COUNT, &quadColors[0] );
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


