#include "text.h"
#include "font.h"

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

bool isSpace(const unsigned char c)
{
  return c == ' ';
}
bool isTab(const unsigned char c)
{
  return c == '\t';
}
bool isNewline(const unsigned char c)
{
  return c == '\n';
}

bool isWhitespace(const unsigned char c)
{
  return isSpace(c) || isTab(c) || isNewline(c);
}

bool isBackspace(const unsigned char c)
{
  return c == '\r';
}

void text::Textbox::writeWord(size_t start, size_t length)
{
  for (size_t ci = 0; ci < length; ++ci) {
	const unsigned char& c = str[start + ci];
	const Font::Metric& met = font->getMetric(c);
	font->pushCharCode(c);
	font->pushCharPos(Quad::getPos() + glm::vec2(cursor, -1.0f * font->linegap * line) + met.bearing);
	cursor += met.advance;
  }
}

void text::Textbox::writeString()
{
  line = 0;
  cursor = 0.0f;

  bufferBegin = font->getCharCount();
  font->reserveChars(bufferBegin + str.size());

  size_t wordLength = 0;
  float wordWidth = 0.0f;

  for (size_t ci = 0; ci < str.size(); ++ci) {
	const unsigned char& c = str[ci];
	const Font::Metric& met = font->getMetric(c);

	// Word wrapping
	// for every printable char, increase the word size.
	// when encountering a whitespace, push the word to
	// the buffer and start a new word
	if (isSpace(c)) {
	  writeWord(ci - wordLength, wordLength);
	  wordLength = 0;
	  wordWidth = 0.0f;
	  cursor += met.advance;
	}
	else if (isTab(c)) {
	  writeWord(ci - wordLength, wordLength);
	  wordLength = 0;
	  wordWidth = 0.0f;
	  cursor += font->getMetric(' ').advance * tabsize;
	}
	else if (isNewline(c)) {
	  writeWord(ci - wordLength, wordLength);
	  wordLength = 0;
	  wordWidth = 0.0f;
	  lineBreak();
	  continue;
	}
	else {
	  ++wordLength;
	  wordWidth += met.advance;
	}

	if (cursor + wordWidth > this->getSize().x) {
	  if (wordWidth > this->getSize().x) {
		writeWord(ci - (wordLength - 1), wordLength - 1);
		wordLength = 1;
		wordWidth = met.advance;
	  }
	  lineBreak();
	}
  }
  if (wordLength) {
	writeWord(str.size() - wordLength, wordLength);
  }

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

void text::updateText()
{
  resetFonts();
  updateTextboxes();
  updateFonts();
}
