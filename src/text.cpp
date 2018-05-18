#include "text.h"
#include "shader.h"
#include "vao.h"
#include "primitives.h"
#include "app.h"
#include "contextwindow.h"

gl::VAO fontVAO;
unsigned int fontShader;
glm::uvec2 text::LoadedFont::resolution = glm::uvec2( 1920, 1080 );

text::LoadedFont text::mainFont;

using namespace text;

void text::LoadedFont::setTargetResolution( const unsigned int rx, const unsigned int ry )
{
	setTargetResolution( glm::uvec2( rx, ry ) );
}
void text::LoadedFont::setTargetResolution( const glm::uvec2 pRes )
{
	resolution = pRes;
}
void text::loadFonts()
{
	initFreeType();
	
	Font font;
	font.setLoadPadding( 0 );
	font.setLoadDpi( app::windowMonitor->dpi * glm::uvec2( 2, 2 ) );
	font.setLoadSize( 6 );
	font.read( "Terminus.ttf" );
	
	LoadedFont::setTargetResolution( app::mainWindow.width, app::mainWindow.height );
	mainFont = LoadedFont( font );
}

text::LoadedFont::LoadedFont( const Font& font )
{
	atlasTexture = texture::Texture2D( font.atlas );
	texture::setTextureWrapping( atlasTexture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	texture::setTextureFilter( atlasTexture, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST );
	texture::generateMipMap( atlasTexture );

	std::vector<glm::vec4> glyphuvs( font.glyphs.quads.size() );
	for ( unsigned int g = 0; g < font.glyphs.quads.size(); ++g ) {
		glm::uvec4 quad = font.glyphs.quads[ g ];
		glyphuvs[g] = glm::vec4(	(float)quad.x / (float)font.atlas.width,
									(float)quad.y / (float)font.atlas.height,
									(float)quad.z / (float)font.atlas.width,
									(float)quad.w / (float)font.atlas.height );
	}

	std::vector<glm::vec2> sizes( font.glyphs.quads.size() );
	for ( unsigned int g = 0; g < sizes.size(); ++g ) {
		glm::uvec4 quad = font.glyphs.quads[ g ];
		sizes[g] = glm::vec2(	(float)(quad.z) / ((float)resolution.x / 2.0f ),
								(float)(quad.w) / ((float)resolution.y / 2.0f ) );
	}
	metrics.resize( font.glyphs.metrics.size() );
	for ( unsigned int g = 0; g < metrics.size(); ++g ) {
		const Font::Glyphs::Metric& met = font.glyphs.metrics[ g ];
		metrics[g] = LoadedMetric( 
				(float)met.advance / ((float)resolution.x / 2.0f ),
				(float)met.bearingx / ((float)resolution.x / 2.0f ),
				(float)met.bearingy / ((float)resolution.y / 2.0f ) );

	}


	uvBuffer = gl::Storage<glm::vec4>( "UVBuffer", 255, 0, &glyphuvs[0] );
	uvBuffer.setTarget( GL_UNIFORM_BUFFER );

	sizeBuffer = gl::Storage<glm::vec2>( "SizeBuffer", 255, 0, &sizes[0] );
	sizeBuffer.setTarget( GL_UNIFORM_BUFFER );

	posBuffer = gl::StreamStorage<glm::vec2>( "PosBuffer", 1000, GL_MAP_WRITE_BIT );
	posBuffer.setTarget( GL_UNIFORM_BUFFER );

	charBuffer = gl::StreamStorage<unsigned int>( "CharBuffer", 1000, GL_MAP_WRITE_BIT );
	charBuffer.setTarget( GL_UNIFORM_BUFFER );
}

void text::initFontVAO()
{
	fontVAO = gl::VAO( "FontVAO" );

	fontVAO.vertexBuffer( 0, gl::quadVBO, sizeof(glm::vec2) ); 
	fontVAO.vertexAttrib( 0, 0, 2, GL_FLOAT, 0 ); 
	fontVAO.elementBuffer( gl::quadEBO ); 
}

void text::initFontShader()
{
	fontShader = shader::newProgram( "FontShader", 
			shader::createModule( "fontShader.vert" ),
			shader::createModule( "fontShader.frag" ) );
	shader::addVertexAttribute( fontShader, "corner", 0 );
}

void text::setupFontShader()
{
}

void text::LoadedFont::uploadChars()
{
	gl::uploadStorage( charBuffer, sizeof( unsigned int ) * chars.size(), &chars[0] );
}

void text::LoadedFont::uploadPositions()
{
	gl::uploadStorage( posBuffer, sizeof( glm::vec2 ) * positions.size(), &positions[0] );
}
void text::LoadedFont::print( const std::string& str, glm::vec2 pos )
{
	chars.resize( 1000 );
	positions.resize( 1000 );

	glm::vec2 cursor;
	for ( unsigned int c = 0; c < 1000; ++c ) {
		//const unsigned char& c = str[ci];
		const LoadedMetric& met = metrics[ c ];
		positions[c] = ( pos + cursor + met.bearing );
		chars[c] = ( c );
		cursor.x += met.advance;
		if ( cursor.x > 1.0f ) {
			cursor.x = 0.0f;
			cursor.y -= 0.1f;
		}
	}
}
void text::LoadedFont::render()
{
	if ( chars.size() ) {
		shader::bindUniformBufferToShader( fontShader, posBuffer, "PosBuffer" );
		shader::bindUniformBufferToShader( fontShader, charBuffer, "CharBuffer" );
		shader::bindUniformBufferToShader( fontShader, uvBuffer, "UVBuffer" );
		shader::bindUniformBufferToShader( fontShader, sizeBuffer, "SizeBuffer" );
		shader::use( fontShader );
		fontVAO.bind();

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, atlasTexture.ID );

		glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, chars.size() );

		fontVAO.unbind();
		shader::unuse();
	}
}

