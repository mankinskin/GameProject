#include "font.h"
#include <unistd.h>
#include <fcntl.h>
#include "vao.h"
#include "primitives.h"
#include "app.h"
#include "contextwindow.h"
#include "viewport.h"
#include "utils/char.h"

gl::VAO text::Font::fontVAO;
shader::Program text::Font::fontShader;
typename text::Font::Container text::Font::all;
size_t text::tabsize = 4;


const text::Font::Metric& text::Font::getMetric(const size_t i) const
{
  return metrics[i];
}

void text::Font::loadFontFile(const FontFile& fontfile)
{
  name = fontfile.name;
  atlasTexture = texture::Texture2D(fontfile.atlas);
  texture::setTextureWrapping(atlasTexture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  texture::setTextureFilter(atlasTexture, GL_NEAREST, GL_NEAREST);

  // load glyph UVs
  std::vector<glm::vec4> glyphuvs(fontfile.glyphs.quads.size());
  for (size_t g = 0; g < fontfile.glyphs.quads.size(); ++g) {
    glm::uvec4 quad = fontfile.glyphs.quads[ g ];
    glyphuvs[g] = glm::vec4((float)quad.x / (float)fontfile.atlas.width,
        (float)quad.y / (float)fontfile.atlas.height,
        (float)quad.z / (float)fontfile.atlas.width,
        (float)quad.w / (float)fontfile.atlas.height);
  }

  //load glyph sizes
  std::vector<glm::vec2> sizes(fontfile.glyphs.quads.size());
  for (size_t g = 0; g < sizes.size(); ++g) {
    glm::uvec4 quad = fontfile.glyphs.quads[ g ];
    sizes[g] = glm::vec2((float)(quad.z) * gl::pixel_size().x,
        (float)(quad.w) * gl::pixel_size().y);
  }
  // load glyph metrics
  metrics.resize(fontfile.glyphs.metrics.size());
  for (size_t g = 0; g < metrics.size(); ++g) {
    const FontFile::Glyphs::Metric& met = fontfile.glyphs.metrics[ g ];
    metrics[g] = Metric(
        (float)met.advance * gl::pixel_size().x,
        (float)met.bearingx * gl::pixel_size().x,
        (float)met.bearingy * gl::pixel_size().y);
  }

  linegap = (float)fontfile.linegap * gl::pixel_size().y;

  uvBuffer = gl::Storage<glm::vec4>("UVBuffer", glyphuvs.size(), 0, &glyphuvs[0]);
  uvBuffer.setTarget(GL_UNIFORM_BUFFER);

  sizeBuffer = gl::Storage<glm::vec2>("SizeBuffer", sizes.size(), 0, &sizes[0]);
  sizeBuffer.setTarget(GL_UNIFORM_BUFFER);

  posBuffer = gl::StreamStorage<glm::vec2>("PosBuffer", 1000, GL_MAP_WRITE_BIT);
  posBuffer.setTarget(GL_UNIFORM_BUFFER);

  charBuffer = gl::StreamStorage<unsigned int>("CharBuffer", 1000, GL_MAP_WRITE_BIT);
  charBuffer.setTarget(GL_UNIFORM_BUFFER);
}

void text::initFontVAO()
{
  Font::fontVAO = gl::VAO("FontVAO");

  Font::fontVAO.vertexBuffer(0, gl::quadVBO, 0, sizeof(glm::vec2));
  Font::fontVAO.vertexAttrib(0, 0, 2, GL_FLOAT, 0);
  Font::fontVAO.elementBuffer(gl::quadEBO);
}

void text::initFontShader()
{
  Font::fontShader = shader::Program("FontShader",
      shader::Stage("fontShader.vert"),
      shader::Stage("fontShader.frag"));
  Font::fontShader.addVertexAttribute("corner", 0);
}

void text::setupFontShader()
{
  Font::fontShader.build();
}

size_t text::Font::makeText(glm::vec4 box)
{
  puts("Making text.");
  texts.push_back(Text(box));
  return texts.size() -1;
}
void text::Font::reserveChars(const size_t n)
{
  chars.reserve(n);
  positions.reserve(n);
}

size_t text::Font::getCharCount() const
{
  return charCount;
}

void text::Font::setCharCode(const size_t i, const size_t c)
{
  chars[i] = c;
}
void text::Font::setCharPos(const size_t i, const glm::vec2 p)
{
  positions[i] = p;
}
void text::Font::pushCharCode(const size_t c)
{
  chars.push_back(c);
}
void text::Font::pushCharPos(const glm::vec2 p)
{
  positions.push_back(p);
}

void text::loadFonts()
{
  initFreeType();

  FontFile::setLoadPadding(1);
  puts("Font terminus");
  Font::all.makeID(Font(FontFile("Terminus.ttf", 12)));
  puts("Font liberation");
  Font::all.makeID(Font(FontFile("LiberationMono-Regular.ttf", 16)));
}

void text::Font::use() const
{
  fontShader.bindUniformBuffer(posBuffer, "PosBuffer");
  fontShader.bindUniformBuffer(charBuffer, "CharBuffer");
  fontShader.bindUniformBuffer(uvBuffer, "UVBuffer");
  fontShader.bindUniformBuffer(sizeBuffer, "SizeBuffer");
}
void text::Font::clearTexts()
{
  chars.clear();
  positions.clear();
}

void text::updateTexts()
{
  for (Font& font : Font::all) {
    font.update();
  }
}

void text::renderTexts()
{
  for (const Font& font : Font::all) {
    font.render();
  }
}

void text::Font::render() const
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


size_t printableCharCount(const std::string& s)
{
  size_t count = 0;
  for (const unsigned char& c : s) {
    if (isprint(c)) {
      ++count;
    }
  }
  return count;
}

void text::Font::update()
{
  std::vector<unsigned int> newChars;
  std::vector<glm::vec2> newPositions;

  size_t newCharCount = 0;
  for (Text& text : texts) {
    updateText(text);
    newCharCount += text.bufSize;
  }
  gl::uploadStorage(charBuffer, sizeof(unsigned int) * chars.size(), &chars[0]);
  gl::uploadStorage(posBuffer, sizeof(glm::vec2) * positions.size(), &positions[0]);
}

void text::Font::updateText(Text& text)
{
  if (text.bufSize == 0) {
    writeText(text);
    // text has no range in buffers yet
    // write text to end of buffers
    if (text.length()) {
      // text is not empty
    }
  } else {
    if (text.length() < text.bufSize) {
      // text is shorter than before
      // shrink its buffer range
    } else if (text.length() > text.bufSize) {
      // text is longer than before
      // increase its range
    } else {
      // text is same length as before
    }
  }
  size_t diffPos = 0;
  for (diffPos = 0; diffPos < text.bufSize; ++diffPos) {
    if (text.str[diffPos] != chars[diffPos]) {
      break;
    }
  }
}

void text::Font::writeWord(Text& text, size_t start, size_t length)
{
  for (size_t ci = 0; ci < length; ++ci) {
    const unsigned char& c = text.str[start + ci];
    const Font::Metric& met = getMetric(c);
    pushCharCode(c);
    pushCharPos(text.getPos() + glm::vec2(text.cursor, -linegap * text.line) + met.bearing);
    text.cursor += met.advance;
  }
}

void text::Font::writeText(Text& text)
{
  text.line = 0;
  text.cursor = 0.0f;

  text.bufBegin = charCount;
  text.bufSize = printableCharCount(text.str);

  reserveChars(text.bufBegin + text.str.size());

  size_t wordLength = 0;
  float wordWidth = 0.0f;

  for (size_t ci = 0; ci < text.str.size(); ++ci) {
    const unsigned char& c = text.str[ci];
    const Font::Metric& met = getMetric(c);

    // Word wrapping
    // for every printable char, increase the word size.
    // when encountering a whitespace, push the word to
    // the buffer and start a new word
    if (utils::isSpace(c)) {
      writeWord(text, ci - wordLength, wordLength);
      wordLength = 0;
      wordWidth = 0.0f;
      text.cursor += met.advance;
    }
    else if (utils::isTab(c)) {
      writeWord(text, ci - wordLength, wordLength);
      wordLength = 0;
      wordWidth = 0.0f;
      text.cursor += getMetric(' ').advance * tabsize;
    }
    else if (utils::isNewline(c)) {
      writeWord(text, ci - wordLength, wordLength);
      wordLength = 0;
      wordWidth = 0.0f;
      text.lineBreak();
      continue;
    }
    else {
      ++wordLength;
      wordWidth += met.advance;
    }

    if (text.cursor + wordWidth > text.getSize().x) {
      if (wordWidth > text.getSize().x) {
        writeWord(text, ci - (wordLength - 1), wordLength - 1);
        wordLength = 1;
        wordWidth = met.advance;
      }
      text.lineBreak();
    }
  }
  if (wordLength) {
    writeWord(text, text.str.size() - wordLength, wordLength);
  }

  text.line = 0;
  text.cursor = 0.0f;
}
