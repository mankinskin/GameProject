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
}

void text::Text::writeWord( unsigned int start, unsigned int length )
{
    for ( unsigned int ci = 0; ci < length; ++ci ) {
        const unsigned char& c = str[start + ci];
		const Font::Metric& met = font->metrics[ c ];
        font->chars.push_back( c );
        font->positions.push_back( position + glm::vec2( cursor, -1.0f * font->linegap * line ) + met.bearing );
        cursor += met.advance;
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

    size_t wordChars = 0;
    float wordLength = 0.0f;

	for ( unsigned int ci = 0; ci < str.size(); ++ci ) {
		const unsigned char& c = str[ci];
		const Font::Metric& met = font->metrics[ c ];

        // Word wrapping
        // for every printable char, increase the word size
        // when encountering a whitespace, push the word to 
        // the buffer and start a new word
		if ( c != ' ' && c != '\t' && c != '\n' ) {
            ++wordChars;
		    wordLength += met.advance;
        } 
        else if ( c == ' ' ) {
            writeWord( ci - wordChars, wordChars );
            wordChars = 0; 
            wordLength = 0.0f; 
		    cursor += met.advance;
        }
        else if ( c == '\t' ) {
            writeWord( ci - wordChars, wordChars );
            wordChars = 0; 
            wordLength = 0.0f; 
            tab();
        } 
        else if ( c == '\n' ) {
            writeWord( ci - wordChars, wordChars );
            wordChars = 0; 
            wordLength = 0.0f; 
			lineBreak();			
            continue;
        }
        if ( cursor + wordLength > size.x ) {
            if ( wordLength > size.x ) {
                writeWord( ci - (wordChars - 1), wordChars - 1 );
                wordChars = 1; 
                wordLength = met.advance; 
            }
			lineBreak();			
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
