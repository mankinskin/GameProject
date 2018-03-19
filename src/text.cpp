#include "text.h"
#include "shader.h"
#include "gldebug.h"
#include <algorithm>
#include "camera.h"
#include "debug.h"
#include <gtc/matrix_transform.hpp>
#include "framebuffer.h"
#include "quad.h"
#include "font_loader.h"

struct TextboxMetrics {
	TextboxMetrics( unsigned int pFont, glm::vec2 pGlyphScale, float pAdvanceScale, float pLineGapScale )
		:font( pFont ), glyphScale( pGlyphScale ), advanceScale( pAdvanceScale ), lineGapScale( pLineGapScale ) {}
	TextboxMetrics( unsigned int pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale )
		:font( pFont ), glyphScale( glm::vec2( pGlyphScaleX, pGlyphScaleY ) ), advanceScale( pAdvanceScale ), lineGapScale( pLineGapScale ) {}
	unsigned int font;
	glm::vec2 glyphScale;
	float advanceScale;
	float lineGapScale;
};


struct Textbox {
	Textbox()
		:metrics( 0 ), chars( gui::text::String() ), pos( 0 ), size( 0 ), marging( 0.0f ), flags( 0 ) {}

	gui::text::String chars;
	unsigned int metrics;
	unsigned int pos;
	unsigned int size;
	float marging = 0.0f;
	unsigned int flags;
};



struct TextColor {
	TextColor()
		:color( glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) ) {}
	TextColor( float r, float g, float b, float a )
		:color( glm::vec4( r, g, b, a ) ) {}
	TextColor( glm::vec4& pColor )
		:color( pColor ) {}
	glm::vec4 color = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
};



struct TextboxGlyphs {
	TextboxGlyphs( unsigned int pGlyphCount ) {
		quads.resize( pGlyphCount );
		glyphIndices.resize( pGlyphCount );
		lines.reserve( pGlyphCount );
		line_sizes.reserve( pGlyphCount );
	}
	std::vector<gui::text::String> lines;
	std::vector<glm::vec2> line_sizes;
	std::vector<unsigned int> glyphIndices;
	std::vector<gui::text::CharQuad> quads;
};

std::vector<Textbox> allTextboxes;
std::vector<TextboxMetrics> allTextboxMetrics;
std::vector<unsigned int> textboxStringIndices;
std::vector<glm::vec2> allTextboxPositions;
std::vector<glm::vec2> allTextboxSizes;
std::vector<glm::vec4> allTextColors;

std::vector<gui::text::String> gui::text::allFontStrings;
std::vector<gui::text::CharQuad> gui::text::charQuadBuffer;
std::vector<unsigned char> gui::text::allChars;

//std::vector<std::string> gui::text::allTextColorNames;


void gui::text::initFonts()
{
	/*FONTS
	times,
	FreeSans,
	Input_Regular_Mono,
	Input_Light_Mono,
	FreeMono,
	C64_Pro_Mono
	C64_Pro_Regular,
	Ubuntu_Regular_Mono,
	SourceCodePro_Regular,
	SourceCodePro_Medium,
	VCR_OSD_MONO
	Generic
	justabit
	BetterPixels
	UnnamedFont
	TinyUnicode
	Hack-Regular
	DroidSansMonoSlashed
	*/
	gui::text::initializer::initFreeType();
	gui::text::initFontVAO();
	gui::text::initializer::includeFont( "Generic.ttf", 12, 30, 200, 0, 1 );
	gui::text::initializer::loadFonts();
}

void gui::text::reserveTextboxSpace( unsigned int pCount ) {

	allTextboxes.reserve( pCount );
	allTextboxPositions.reserve( pCount );
	allTextboxSizes.reserve( pCount );
}

unsigned int gui::text::createTextbox( unsigned int pPosIndex, unsigned int pSizeIndex, unsigned int pMetrics, int pFlags, float pMarging ) {
	Textbox tb;
	tb.pos = pPosIndex;
	tb.size = pSizeIndex;
	tb.metrics = pMetrics;
	tb.marging = pMarging;
	tb.flags = pFlags;
	allTextboxes.push_back( tb );
	return allTextboxes.size() - 1;
}

unsigned int gui::text::createTextbox( unsigned int pQuad, unsigned int pMetrics, int pFlags, float pMarging ) {
	glm::vec4 qd = allQuads[pQuad - 1];
	return createTextbox( qd, pMetrics, pFlags, pMarging );
}

unsigned int gui::text::createTextbox( glm::vec4 pQuad, unsigned int pMetrics, int pFlags, float pMarging )
{
	allTextboxPositions.emplace_back( pQuad.x, pQuad.y );
	allTextboxSizes.emplace_back( pQuad.z, pQuad.w );
	return createTextbox( allTextboxPositions.size() - 1, allTextboxSizes.size() - 1, pMetrics, pFlags, pMarging );
}

unsigned int gui::text::createTextbox( glm::vec2 pTopLeft, glm::vec2 pSize, unsigned int pMetrics, int pFlags, float pMarging )
{
	allTextboxPositions.push_back( pTopLeft );
	allTextboxSizes.push_back( pSize );
	return createTextbox( allTextboxPositions.size() - 1, allTextboxSizes.size() - 1, pMetrics, pFlags, pMarging );
}

void gui::text::setTextboxString( unsigned int pTextbox, String pString )
{
	Textbox& tb = allTextboxes[pTextbox];
	tb.chars = pString;
}
void gui::text::setTextboxString( unsigned int pTextbox, std::string pString )
{
	setTextboxString( pTextbox, String( pString ) );
}

void gui::text::revalidateTextboxCharIndices()
{
	unsigned int off = 0;
	for ( Textbox& tb : allTextboxes ) {
		tb.chars.offset = off;
		off += tb.chars.count;
	}
}

unsigned int gui::text::createTextboxMetrics( unsigned int pFont, float pGlyphScaleX, float pGlyphScaleY, float pAdvanceScale, float pLineGapScale )
{
	allTextboxMetrics.emplace_back( pFont, glm::vec2( pGlyphScaleX, pGlyphScaleY ), pAdvanceScale, pLineGapScale );
	return allTextboxMetrics.size() - 1;
}



//unsigned int gui::text::createTextStyle( float pThickness, float pHardness )
//{
//	return createTextStyle( TextStyle( pThickness, pHardness ) );
//}

unsigned int gui::text::createTextColor( glm::vec4 pColor )
{
	allTextColors.push_back( pColor );
	return allTextColors.size() - 1;
}


void loadTextboxGlyphs( Textbox& pTextbox, TextboxMetrics& pTextMetrics, gui::text::Font& pFont, TextboxGlyphs& pGlyphs )
{
	float cursor = 0.0f;
	gui::text::FontInstructions& font_inst = gui::text::allFontInstructions[pFont.instructions];

	glm::vec2 thisLineSize = glm::vec2();
	gui::text::String thisLineStr = gui::text::String( 0, 0 );
	unsigned int str_char_off = 0; //the offset of the current string ( in pGlyphs.glyphIndices & pGlyphs.quads )
	unsigned int line_char_off = 0;//the offset of the current line
	unsigned int lineCharCount = 0;
	float thisLineGreatestAscend = 0.0f;
	float thisLineGreatestDescend = 0.0f;

	unsigned int str_nonCharCount = 0;
	//iterate string chars
	for ( unsigned int c = 0; c < pTextbox.chars.count; ++c ) {
		//get glyph metrics
		unsigned charCode = gui::text::allChars[pTextbox.chars.offset + c];
		unsigned int glyphIndex = std::max( ( unsigned int )0, std::min( charCode - font_inst.startCode, font_inst.glyphCount - 1 ) );
		gui::text::GlyphMetrics& met = gui::text::allMetrics[pFont.metricOffset + glyphIndex];

		if ( charCode != '\n' ) {
			//append char to all chars
			gui::text::CharQuad qd( cursor + met.bearingX* pTextMetrics.glyphScale.x, met.bearingY* pTextMetrics.glyphScale.y, met.width * pTextMetrics.glyphScale.x, met.height * pTextMetrics.glyphScale.y );
			thisLineGreatestAscend = std::max( thisLineGreatestAscend, met.bearingY );
			thisLineGreatestDescend = std::max( thisLineGreatestDescend, met.height - met.bearingY );
			pGlyphs.quads[str_char_off + c - str_nonCharCount] = qd;
			pGlyphs.glyphIndices[str_char_off + c - str_nonCharCount] = glyphIndex;

			cursor += met.advanceX * pTextMetrics.advanceScale;
			++lineCharCount;
			
		}
		else {
			++str_nonCharCount;
			thisLineSize.y = std::max( thisLineSize.y, thisLineGreatestAscend + thisLineGreatestDescend );
			thisLineSize.x = cursor;
			pGlyphs.line_sizes.push_back( thisLineSize );

			thisLineStr = gui::text::String( line_char_off, lineCharCount );
			pGlyphs.lines.push_back( thisLineStr );

			thisLineGreatestAscend = 0.0f;
			thisLineGreatestDescend = 0.0f;
			cursor = 0.0f;
			line_char_off += lineCharCount;
			lineCharCount = 0;
		}
	}//end loop over string chars
	str_char_off += pTextbox.chars.count - str_nonCharCount;
	thisLineSize.y = std::max( thisLineSize.y, thisLineGreatestAscend + thisLineGreatestDescend );
	thisLineSize.x = cursor;
	pGlyphs.line_sizes.push_back( thisLineSize );

	thisLineStr = gui::text::String( line_char_off, lineCharCount );
	pGlyphs.lines.push_back( thisLineStr );
	pGlyphs.line_sizes.shrink_to_fit();
	pGlyphs.lines.shrink_to_fit();
	pGlyphs.glyphIndices.resize( str_char_off );
	pGlyphs.quads.resize( str_char_off );
}
void transformTextboxGlyphs( Textbox& pTextbox, TextboxGlyphs& pGlyphs )
{
	glm::vec2 tb_pos = allTextboxPositions[pTextbox.pos];
	glm::vec2 tb_size = allTextboxSizes[pTextbox.size];
	TextboxMetrics& textMetrics = allTextboxMetrics[pTextbox.metrics];
	gui::text::Font& font = gui::text::allFonts[textMetrics.font];
	unsigned int line_count = pGlyphs.lines.size();
	float allLineHeight = font.fontMetric.lineGap * line_count * textMetrics.lineGapScale;
	float first_line_height = font.fontMetric.lineGap * textMetrics.lineGapScale;
	glm::vec2 cursor = glm::vec2( tb_pos.x, tb_pos.y );
	if ( pTextbox.flags & TEXT_LAYOUT_FREE_LINES ) {
		allLineHeight = 0.0f;
		first_line_height = pGlyphs.line_sizes[0].y;
		for ( unsigned int l = 0; l < pGlyphs.lines.size(); ++l ) {
			allLineHeight += pGlyphs.line_sizes[l].y * textMetrics.lineGapScale;
		}
	}
	if ( pTextbox.flags & TEXT_LAYOUT_CENTER_Y ) {
		cursor.y = ( cursor.y - ( tb_size.y / 2.0f - first_line_height ) );
	}
	for ( unsigned int l = 0; l < pGlyphs.lines.size(); ++l ) {
		gui::text::String& line = pGlyphs.lines[l];
		glm::vec2& line_size = pGlyphs.line_sizes[l];

		if ( pTextbox.flags & TEXT_LAYOUT_FREE_LINES ) {
			cursor.y -= pGlyphs.line_sizes[l].y * textMetrics.lineGapScale;
		}
		else {
			cursor.y -= font.fontMetric.lineGap;
		}

		if ( pTextbox.flags & TEXT_LAYOUT_CENTER_X ) {
			cursor.x = tb_pos.x + ( tb_size.x / 2.0f ) - line_size.x / 2.0f;
		}
		else if ( pTextbox.flags & TEXT_LAYOUT_BOUND_LEFT ) {
			cursor.x = tb_pos.x + pTextbox.marging;
		}
		else if ( pTextbox.flags & TEXT_LAYOUT_BOUND_RIGHT ) {
			cursor.x = ( tb_pos.x + tb_size.x ) - ( line_size.x + pTextbox.marging );
		}
		for ( unsigned int g = 0; g < line.count; ++g ) {
			gui::text::CharQuad& qd = pGlyphs.quads[line.offset + g];
			qd.pos += cursor;
		}

	}

}

void gui::text::loadTextboxes()
{
	for ( unsigned int t = 0; t < allTextboxes.size(); ++t ) {

		Textbox& tb = allTextboxes[t];
		TextboxMetrics& textMetrics = allTextboxMetrics[tb.metrics];
		Font& font = allFonts[textMetrics.font];

		TextboxGlyphs tbGlyphs( tb.chars.count );
		//load word strings and get style and color indices
		//load lines and their sizes
		//transform all glyphs with formatting
		loadTextboxGlyphs( tb, textMetrics, font, tbGlyphs );
		transformTextboxGlyphs( tb, tbGlyphs );

		unsigned int buffer_offset = charQuadBuffer.size();
		insertFontString( font, String( buffer_offset, tbGlyphs.quads.size() ) ); //insert the string of this textbox for this font

		charQuadBuffer.resize( buffer_offset + tbGlyphs.quads.size() );
		std::memcpy( &charQuadBuffer[buffer_offset], &tbGlyphs.quads[0], sizeof( CharQuad )*tbGlyphs.quads.size() );

		glyphIndexBuffer.resize( buffer_offset + tbGlyphs.glyphIndices.size() );
		std::memcpy( &glyphIndexBuffer[buffer_offset], &tbGlyphs.glyphIndices[0], sizeof( unsigned int )*tbGlyphs.glyphIndices.size() );
	}
	revalidateFontStringIndices();
}
void gui::text::setupGlyphShader() {
	shader::bindUniformBufferToShader( glyphShaderProgram, gl::generalUniformBuffer, "GeneralUniformBuffer" );
}
void gui::text::renderGlyphs()
{
	glBindVertexArray( fontVAO );
	shader::use( glyphShaderProgram );
	float ref = 0.5f;
	glDisable( GL_DEPTH_TEST );
	glAlphaFunc( GL_GREATER, ref );
	glActiveTexture( GL_TEXTURE0 );
	for ( unsigned int fo = 0; fo < allFonts.size(); ++fo ) {
		Font& font = allFonts[fo];
		glBindTexture( GL_TEXTURE_2D, font.atlasID );
		shader::bindUniformBufferToShader( glyphShaderProgram, font.glyphStorageIndex, "GlyphBuffer" );

		for ( unsigned int s = 0; s < font.stringCount; ++s ) {
			String& str = allFontStrings[font.stringOffset + s];
			shader::setUniform( glyphShaderProgram, "styleIndex", ( unsigned int )0 );

			glDrawElementsInstancedBaseInstance( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, str.count, str.offset );

		}
	}
	glEnable( GL_DEPTH_TEST );
	glAlphaFunc( GL_ALWAYS, 0.0f );
	shader::unuse();
	glBindVertexArray( 0 );
}

gui::text::String::String( std::string pString )
	:offset( allChars.size() ), count( pString.size() ) {
	allChars.insert( allChars.end(), pString.begin(), pString.end() );
}


