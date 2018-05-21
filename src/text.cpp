#include "text.h"

std::vector<text::Text> text::texts;
void text::Text::print( const std::string& pStr )
{
	str = pStr;
}

void text::Text::lineBreak()
{
	cursor.x = 0.0f;
	cursor.y -= font->linegap;
}

void text::Text::writeChars()
{
	font->chars.reserve( font->chars.size() + str.size() );
	font->positions.reserve( font->positions.size() + str.size() );
	
	pos = Font::pixel_quantize( pos );
	for ( unsigned int ci = 0; ci < str.size(); ++ci ) {
		const unsigned char& c = str[ci];
		const Font::Metric& met = font->metrics[ c ];
		if ( c == '\n' || cursor.x + met.advance > size.x ) {
			lineBreak();			
			continue;
		}
		font->positions.push_back( Font::pixel_quantize( pos + cursor + met.bearing ) );
		font->chars.push_back( c );
		cursor.x += met.advance;
	}
}

void text::Text::setFont( FontID pFontID )
{
	font.index = pFontID.index;
}

void text::updateTexts()
{
	for( Text& text : texts ) {
		text.writeChars();
	}
}
