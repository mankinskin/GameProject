#include "text.h"

size_t text::tabsize = 4;

utils::Container<text::Textbox> text::Textbox::all = utils::Container<text::Textbox>();

void text::Textbox::setString(const std::string& pStr)
{
  str = pStr;
}
std::string text::Textbox::getString() const
{
  return str;
}

void text::Textbox::lineBreak()
{
  cursor = 0.0f;
  ++line;
}

void text::Textbox::writeWord(size_t start, size_t length)
{
  for (size_t ci = 0; ci < length; ++ci) {
	const unsigned char& c = str[start + ci];
	const Font::Metric& met = font->metrics[ c ];
	font->chars.push_back(c);
	font->positions.push_back(Quad::getPos() + glm::vec2(cursor, -1.0f * font->linegap * line) + met.bearing);
	cursor += met.advance;
  }
}

void text::Textbox::writeString()
{
  // TODO: consider a printer class for values like this
  line = 0;
  cursor = 0.0f;

  bufferBegin = font->charCount;
  font->chars.reserve(bufferBegin + str.size());
  font->positions.reserve(bufferBegin + str.size());

  size_t wordChars = 0;
  float wordLength = 0.0f;

  for (size_t ci = 0; ci < str.size(); ++ci) {
	const unsigned char& c = str[ci];
	const Font::Metric& met = font->metrics[ c ];

	// Word wrapping
	// for every printable char, increase the word size
	// when encountering a whitespace, push the word to
	// the buffer and start a new word
	if (c != ' ' && c != '\t' && c != '\n') {
	  ++wordChars;
	  wordLength += met.advance;
	}
	else if (c == ' ') {
	  writeWord(ci - wordChars, wordChars);
	  wordChars = 0;
	  wordLength = 0.0f;
	  cursor += met.advance;
	}
	else if (c == '\t') {
	  writeWord(ci - wordChars, wordChars);
	  wordChars = 0;
	  wordLength = 0.0f;
	  cursor += font->metrics[' '].advance * tabsize;
	}
	else if (c == '\n') {
	  writeWord(ci - wordChars, wordChars);
	  wordChars = 0;
	  wordLength = 0.0f;
	  lineBreak();
	  continue;
	}
	if (cursor + wordLength > getSize().x) {
	  if (wordLength > getSize().x) {
		writeWord(ci - (wordChars - 1), wordChars - 1);
		wordChars = 1;
		wordLength = met.advance;
	  }
	  lineBreak();
	}
  }
  if (wordChars) {
	writeWord(str.size() - wordChars, wordChars);
  }

  font->chars.shrink_to_fit();
  font->positions.shrink_to_fit();

  line = 0;
  cursor = 0.0f;
}

void text::Textbox::setFont(Font::ID pFont)
{
  font = pFont;
}

void text::updateTextboxes()
{
  for (Textbox& text : Textbox::all) {
	text.writeString();
  }
}
