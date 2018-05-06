#include "text.h"
#include "shader.h"
#include "vao.h"
#include "primitives.h"

texture::Texture2D atlasTexture;
gl::VAO fontVAO;
unsigned int fontShader;

using namespace text;

void text::loadFonts()
{
	initFreeType();
	
	Font font( "LiberationMono-Regular.ttf" );
	font.read();

	atlasTexture = texture::Texture2D( font.atlas );
	texture::setTextureWrapping( atlasTexture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	texture::setTextureFilter( atlasTexture, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST );
	texture::generateMipMap( atlasTexture );
}

void text::initFontVAO()
{
	fontVAO = gl::VAO( "FontVAO" );
	fontVAO.vertexBuffer( 0, gl::quadVBO, sizeof(glm::vec2) ); 
	fontVAO.elementBuffer( gl::quadEBO ); 
	fontVAO.vertexAttrib( 0, 0, 2, GL_FLOAT, 0 ); 
}

void text::initFontShader()
{
	fontShader = shader::newProgram( "FontShader", 
			shader::createModule( "fontShader.vert" ),
			shader::createModule( "fontShader.frag" ) );
}
	
void text::renderFont()
{
	shader::use( fontShader );
	fontVAO.bind();

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, atlasTexture.ID );

	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

	fontVAO.unbind();
	shader::unuse();
}
