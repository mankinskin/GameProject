#include "text.h"

std::vector<text::Text> text::texts;
unsigned int text::tabsize = 4;


void text::Text::setChars( const std::string& pStr )
{
	str = pStr;
}

void text::Text::lineBreak()
{
	cursor = 0.0f;
	++line;
}

void text::Text::tab()
{
    cursor += font->metrics[' '].advance * tabsize; 
    if ( cursor > size.x ) {
        lineBreak();
    }
}

void text::Text::writeChars()
{
    // TODO: consider a printer class for values like this
    line = 0;
    cursor = 0.0f;

    bufferBegin = font->charCount; 
	font->chars.reserve( bufferBegin + str.size() );
	font->positions.reserve( bufferBegin + str.size() );

    size_t wordLength = 0;
    size_t charCount = 0;
    std::vector<unsigned int> lineLengths( lineCount() );	

	for ( unsigned int ci = 0; ci < str.size(); ++ci ) {
		const unsigned char& c = str[ci];
		const Font::Metric& met = font->metrics[ c ];
        // first determine an action to take based on the character
        // - process newline and tab characters
        // - process whitespaces for word wrapping
        // - pushback printable characters to printing queue
        if ( c == ' ' ) {
            lineLengths[line] += wordLength;
            wordLength = 0; 
        }
        else if ( c == '\t' ) {
            lineLengths[line] += wordLength;
            wordLength = 0; 
            tab();
        } 
        else if ( c == '\n' ) {
            lineLengths[line] += wordLength;
            wordLength = 0; 
			lineBreak();			
			continue;
		} else {
            font->chars.push_back( c );
            font->positions.push_back( position + glm::vec2( cursor, -1.0f * font->linegap * line ) + met.bearing );
            ++charCount;
        }

        // advance appropriately 
        if ( cursor + met.advance > size.x ) {
            // automatic linebreak due to advance out of bounds
			lineBreak();			
		} else {
            ++wordLength;
		    cursor += met.advance;
        }
	}

    font->chars.shrink_to_fit();
	font->positions.shrink_to_fit();

    line = 0;
    cursor = 0.0f;
}

void text::Text::setFont( FontID pFontID )
{
	font = pFontID;
}

void text::updateTexts()
{
	for( Text& text : texts ) {
		text.writeChars();
	}
}
