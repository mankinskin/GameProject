#include "Font.h"
#include "text.h"
#include "shader.h"
#include "primitives.h"

unsigned int MAX_CHARS = 1000;
gl::StreamStorage<gui::text::CharQuad> quadStorage;
gl::StreamStorage<unsigned int> charStorage;
std::vector<gui::text::String> allStrings;

unsigned int styleStorage = 0;
std::vector<gui::text::TextStyle> allTextStyles;

gl::VAO gui::text::fontVAO;
unsigned int gui::text::glyphShaderProgram = 0;
std::vector<unsigned int> gui::text::glyphIndexBuffer;
std::vector<gui::text::Font> gui::text::allFonts;
std::vector<gui::text::FontInstructions> gui::text::allFontInstructions;
std::vector<gui::text::GlyphMetrics> gui::text::allMetrics;
std::vector<float> gui::text::allKerning;

void gui::text::initFontShader()
{
	glyphShaderProgram = shader::newProgram( "glyphShapeShader", shader::createModule( "glyphShapeShader.vert" ), shader::createModule( "glyphShapeShader.frag" ) );
	shader::addVertexAttribute( glyphShaderProgram, "pos", 0 );
	shader::addVertexAttribute( glyphShaderProgram, "quad", 1 );
	shader::addVertexAttribute( glyphShaderProgram, "index", 2 );
}

void gui::text::initFontVAO() 
{
	quadStorage = gl::StreamStorage<CharQuad>( "CharQuadBuffer", 
			MAX_CHARS, GL_MAP_WRITE_BIT );
	charStorage = gl::StreamStorage<unsigned int>( "CharBuffer", 
			MAX_CHARS, GL_MAP_WRITE_BIT );

	fontVAO = gl::VAO( "fontVAO" );
	//gl::setVertexAttrib( fontVAO, 0, 0, 2, GL_FLOAT, 0 );
	//gl::setVertexAttrib( fontVAO, 1, 1, 4, GL_FLOAT, 0 );
	//gl::setVertexAttrib( fontVAO, 2, 2, 1, GL_UNSIGNED_INT, 0 );

	glVertexArrayBindingDivisor( fontVAO, 1, 1 );
	glVertexArrayBindingDivisor( fontVAO, 2, 1 );

	glVertexArrayElementBuffer( fontVAO, gl::quadEBO.ID );
	glVertexArrayVertexBuffer( fontVAO, 0, gl::quadVBO.ID, 0, sizeof( float ) * 2 );

	//gl::setVertexArrayVertexStorage( fontVAO, 1, quadStorage, sizeof( CharQuad ) );
	//gl::setVertexArrayVertexStorage( fontVAO, 2, charStorage, sizeof( unsigned int ) );
}

void gui::text::revalidateFontStringIndices()
{
	unsigned int off = 0;
	for ( Font& fon : allFonts ) {
		fon.stringOffset = off;
		off += fon.stringCount;
	}
}

void gui::text::insertFontString( Font& pFont, String pString )
{
	if ( !pFont.stringCount ) {//if first textbox of this font
		pFont.stringOffset = allFontStrings.size();//dedicate a new range of tb indices to this font
	}
	++pFont.stringCount;
	allFontStrings.insert( allFontStrings.begin() + pFont.stringOffset, pString );
}
//unsigned int gui::text::createTextStyle( TextStyle & pStyle )
//{
//	allTextStyles.push_back( pStyle );
//	return allTextStyles.size() - 1;
//}
void gui::text::initStyleBuffer() 
{
	allTextStyles.reserve( 2 );
	//createTextStyle( 1.5f, 0.8f );
	//createTextStyle( 1.2f, 0.8f );
	//styleStorage = gl::createStorage( "FontStyleBuffer", sizeof( TextStyle )*allTextStyles.size(), 0, &allTextStyles[0] );

	//shader::bindUniformBufferToShader( glyphShaderProgram, styleStorage, "StyleBuffer" );
}

void gui::text::updateCharStorage()
{
	if ( charQuadBuffer.size() ) {
		//gl::uploadStorage( quadStorage, sizeof( CharQuad )*charQuadBuffer.size(), &charQuadBuffer[0] );
		//gl::uploadStorage( charStorage, sizeof( unsigned int )*glyphIndexBuffer.size(), &glyphIndexBuffer[0] );
	}
}

void gui::text::clearCharStorage()
{
	for ( Font& font : allFonts ) {
		font.stringOffset = 0;
		font.stringCount = 0;
	}
	allChars.clear();
	glyphIndexBuffer.clear();
	charQuadBuffer.clear();
	allStrings.clear();
	allFontStrings.clear();
}
