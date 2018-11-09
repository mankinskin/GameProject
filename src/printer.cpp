#include "printer.h"
#include "text.h"


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

text::Printer::Printer(const Font& font)
  : Font(font)
{
  posBuffer = gl::StreamStorage<glm::vec2>("PosBuffer", 1000, GL_MAP_WRITE_BIT);
  posBuffer.setTarget(GL_UNIFORM_BUFFER);

  charBuffer = gl::StreamStorage<unsigned int>("CharBuffer", 1000, GL_MAP_WRITE_BIT);
  charBuffer.setTarget(GL_UNIFORM_BUFFER);
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
  updateTextboxes();
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
