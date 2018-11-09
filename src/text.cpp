#include "text.h"
#include "font.h"
#include "printer.h"

utils::Container<text::Textbox> text::Textbox::all = utils::Container<text::Textbox>();

void text::Textbox::setString(const std::string& pStr)
{
  buf = pStr;
}
void text::Textbox::setFont(const size_t printerID)
{
  printer = printerID;
}
std::string text::Textbox::getString() const
{
  return buf;
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
	const unsigned char& c = buf[start + ci];
	const Font::Metric& met = printer->getMetric(c);
	printer->pushCharCode(c);
	printer->pushCharPos(gui::Quad::getPos() + glm::vec2(cursor, -printer->linegap * line) + met.bearing);
	cursor += met.advance;
  }
}

void text::Textbox::writeText()
{
  line = 0;
  cursor = 0.0f;

  bufferBegin = printer->charCount;
  printer->reserveChars(bufferBegin + buf.size());

  size_t wordLength = 0;
  float wordWidth = 0.0f;

  for (size_t ci = 0; ci < buf.size(); ++ci) {
	const unsigned char& c = buf[ci];
	const Font::Metric& met = printer->getMetric(c);

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
	  cursor += printer->getMetric(' ').advance * tabsize;
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

	if (cursor + wordWidth > getSize().x) {
	  if (wordWidth > getSize().x) {
		writeWord(ci - (wordLength - 1), wordLength - 1);
		wordLength = 1;
		wordWidth = met.advance;
	  }
	  lineBreak();
	}
  }
  if (wordLength) {
	writeWord(buf.size() - wordLength, wordLength);
  }

  line = 0;
  cursor = 0.0f;
}

void text::Textbox::lineBreak()
{
  cursor = 0.0f;
  ++line;
}
void text::updateTextboxes()
{
  for (Textbox& tb : Textbox::all) {
	tb.writeText();
  }
}
