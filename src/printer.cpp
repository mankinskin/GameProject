#include "printer.h"


size_t text::tabsize = 4;

typename text::Printer::Container text::Printer::all;

void text::loadFonts()
{
  initFreeType();

  FontFile::setLoadPadding(1);
  puts("Font terminus");
  Printer::all.makeID(Font(FontFile("Terminus.ttf", 12)));
  puts("Font liberation");
  Printer::all.makeID(Font(FontFile("LiberationMono-Regular.ttf", 16)));
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

text::Printer::Printer(const Font& font)
  : Font(font)
{
  posBuffer = gl::StreamStorage<glm::vec2>("PosBuffer", 1000, GL_MAP_WRITE_BIT);
  posBuffer.setTarget(GL_UNIFORM_BUFFER);

  charBuffer = gl::StreamStorage<unsigned int>("CharBuffer", 1000, GL_MAP_WRITE_BIT);
  charBuffer.setTarget(GL_UNIFORM_BUFFER);
}
void text::Printer::writeWord(const Textbox& tb, size_t start, size_t length)
{
  for (size_t ci = 0; ci < length; ++ci) {
	const unsigned char& c = tb.buf[start + ci];
	const Font::Metric& met = getMetric(c);
	pushCharCode(c);
	pushCharPos(tb.getSize() + glm::vec2(cursor, -1.0f * linegap * line) + met.bearing);
	cursor += met.advance;
  }
}

void text::Printer::printTextbox(const Textbox& tb)
{
  line = 0;
  cursor = 0.0f;

  bufferBegin = charCount;
  reserveChars(bufferBegin + tb.buf.size());

  size_t wordLength = 0;
  float wordWidth = 0.0f;

  for (size_t ci = 0; ci < tb.buf.size(); ++ci) {
	const unsigned char& c = tb.buf[ci];
	const Font::Metric& met = getMetric(c);

	// Word wrapping
	// for every printable char, increase the word size.
	// when encountering a whitespace, push the word to
	// the buffer and start a new word
	if (isSpace(c)) {
	  writeWord(tb, ci - wordLength, wordLength);
	  wordLength = 0;
	  wordWidth = 0.0f;
	  cursor += met.advance;
	}
	else if (isTab(c)) {
	  writeWord(tb, ci - wordLength, wordLength);
	  wordLength = 0;
	  wordWidth = 0.0f;
	  cursor += getMetric(' ').advance * tabsize;
	}
	else if (isNewline(c)) {
	  writeWord(tb, ci - wordLength, wordLength);
	  wordLength = 0;
	  wordWidth = 0.0f;
	  lineBreak();
	  continue;
	}
	else {
	  ++wordLength;
	  wordWidth += met.advance;
	}

	if (cursor + wordWidth > tb.getSize().x) {
	  if (wordWidth > tb.getSize().x) {
		writeWord(tb, ci - (wordLength - 1), wordLength - 1);
		wordLength = 1;
		wordWidth = met.advance;
	  }
	  lineBreak();
	}
  }
  if (wordLength) {
	writeWord(tb, tb.buf.size() - wordLength, wordLength);
  }

  line = 0;
  cursor = 0.0f;
}

void text::Printer::lineBreak()
{
  cursor = 0.0f;
  ++line;
}

void text::Printer::reserveChars(const size_t n)
{
  chars.reserve(n);
  positions.reserve(n);
}
void text::Printer::setCharCode(const size_t i, const size_t c)
{
  chars[i] = c;
}
void text::Printer::setCharPos(const size_t i, const glm::vec2 p)
{
  positions[i] = p;
}
void text::Printer::pushCharCode(const size_t c)
{
  chars.push_back(c);
}
void text::Printer::pushCharPos(const glm::vec2 p)
{
  positions.push_back(p);
}
size_t text::Printer::getCharCount() const
{
  return charCount;
}
void text::Printer::uploadChars() const
{
  gl::uploadStorage(charBuffer, sizeof(unsigned int) * chars.size(), &chars[0]);
}

void text::Printer::uploadPositions() const
{
  gl::uploadStorage(posBuffer, sizeof(glm::vec2) * positions.size(), &positions[0]);
}

void text::Printer::update() const
{
	uploadChars();
	uploadPositions();
}

void text::Printer::clear()
{
	chars.clear();
	positions.clear();
}

void text::Printer::use() const
{
	fontShader.bindUniformBuffer(posBuffer, "PosBuffer");
	fontShader.bindUniformBuffer(charBuffer, "CharBuffer");
	fontShader.bindUniformBuffer(uvBuffer, "UVBuffer");
	fontShader.bindUniformBuffer(sizeBuffer, "SizeBuffer");
}

void text::Printer::render() const
{
  if (chars.size()) {
	use();
	fontShader.use();
	fontVAO.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlasTexture.ID);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, chars.size());

	fontVAO.unbind();
	shader::Program::unuse();
  }
}

void text::updateTexts()
{
  for (Printer& printer : Printer::all) {
	printer.clear();
  }
  for (const Printer& printer : Printer::all) {
  }
  for (const Printer& printer : Printer::all) {
	printer.update();
  }
}
void text::renderTexts()
{
  for (const Printer& printer : Printer::all) {
	printer.render();
  }
}
