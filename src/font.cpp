#include "font.h"
#include <unistd.h>
#include <fcntl.h>
#include "shader.h"
#include "vao.h"
#include "primitives.h"
#include "app.h"
#include "contextwindow.h"


gl::VAO fontVAO;
unsigned int fontShader;
glm::vec2 text::pixel_size = glm::vec2( 2.0f / (float)1920, 2.0f / (float)1080 );
std::vector<text::Font> text::fonts;

void text::setTargetResolution( const unsigned int rx, const unsigned int ry )
{
	setTargetResolution( glm::uvec2( rx, ry ) );
}

void text::setTargetResolution( const glm::uvec2 pRes )
{
	pixel_size = glm::vec2( 2.0f, 2.0f ) / (glm::vec2)pRes;
}

void text::loadFonts()
{
	initFreeType();
	setTargetResolution( app::mainWindow.width, app::mainWindow.height );
	
	FontFile terminusfont;
	terminusfont.setLoadPadding( 1 );
	terminusfont.setLoadDpi( app::windowMonitor->dpi );
	terminusfont.setLoadSize( 16 );
	terminusfont.read( "Terminus.ttf" );

	FontFile liberationfont;
	liberationfont.setLoadPadding( 1 );
	liberationfont.setLoadDpi( app::windowMonitor->dpi );
	liberationfont.setLoadSize( 16 );
	liberationfont.read( "LiberationMono-Regular.ttf" );
	
	fonts.push_back( Font( terminusfont ) );
	fonts.push_back( Font( liberationfont ) );
}

text::Font::Font( const FontFile& fontfile )
{
	name = fontfile.name;
	atlasTexture = texture::Texture2D( fontfile.atlas );
	texture::setTextureWrapping( atlasTexture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	texture::setTextureFilter( atlasTexture, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST );
	texture::generateMipMap( atlasTexture );

	std::vector<glm::vec4> glyphuvs( fontfile.glyphs.quads.size() );
	for ( unsigned int g = 0; g < fontfile.glyphs.quads.size(); ++g ) {
		glm::uvec4 quad = fontfile.glyphs.quads[ g ];
		glyphuvs[g] = glm::vec4(	(float)quad.x / (float)fontfile.atlas.width,
									(float)quad.y / (float)fontfile.atlas.height,
									(float)quad.z / (float)fontfile.atlas.width,
									(float)quad.w / (float)fontfile.atlas.height );
	}

	std::vector<glm::vec2> sizes( fontfile.glyphs.quads.size() );
	for ( unsigned int g = 0; g < sizes.size(); ++g ) {
		glm::uvec4 quad = fontfile.glyphs.quads[ g ];
		sizes[g] = glm::vec2(	(float)(quad.z) * pixel_size.x,
								(float)(quad.w) * pixel_size.y );
	}
	metrics.resize( fontfile.glyphs.metrics.size() );
	for ( unsigned int g = 0; g < metrics.size(); ++g ) {
		const FontFile::Glyphs::Metric& met = fontfile.glyphs.metrics[ g ];
		metrics[g] = Metric( 
				(float)met.advance * pixel_size.x,
				(float)met.bearingx * pixel_size.x,
				(float)met.bearingy * pixel_size.y );
	}

	linegap = (float)fontfile.linegap * pixel_size.y;

	uvBuffer = gl::Storage<glm::vec4>( "UVBuffer", glyphuvs.size(), 0, &glyphuvs[0] );
	uvBuffer.setTarget( GL_UNIFORM_BUFFER );

	sizeBuffer = gl::Storage<glm::vec2>( "SizeBuffer", sizes.size(), 0, &sizes[0] );
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

void text::Font::uploadChars() const
{
	gl::uploadStorage( charBuffer, sizeof( unsigned int ) * chars.size(), &chars[0] );
}

void text::Font::uploadPositions() const
{
	gl::uploadStorage( posBuffer, sizeof( glm::vec2 ) * positions.size(), &positions[0] );
}

void text::updateFonts()
{
	for ( const Font& font : fonts ) {
		font.uploadChars();
		font.uploadPositions();
	}
}

void text::Font::render() const
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

void text::renderFonts()
{
	for ( const Font& font : fonts ) {
		font.render();
	}
}
