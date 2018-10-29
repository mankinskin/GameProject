#include "font.h"
#include <unistd.h>
#include <fcntl.h>
#include "vao.h"
#include "primitives.h"
#include "app.h"
#include "contextwindow.h"


glm::vec2 text::pixel_size = glm::vec2(2.0f / (float)1920, 2.0f / (float)1080);
typename text::Font::Container text::Font::all = typename text::Font::Container();
gl::VAO text::Font::fontVAO;
shader::Program text::Font::fontShader;

void text::Font::reserveChars(const size_t n)
{
  chars.reserve(n);
  positions.reserve(n);
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
size_t text::Font::getCharCount() const
{
  return charCount;
}
const text::Font::Metric& text::Font::getMetric(const size_t i) const
{
  return metrics[i];
}
void text::setTargetResolution(const size_t rx, const size_t ry)
{
  setTargetResolution(glm::uvec2(rx, ry));
}

void text::setTargetResolution(const glm::uvec2 pRes)
{
  pixel_size = glm::vec2(2.0f, 2.0f) / (glm::vec2)pRes;
}

void text::loadFonts()
{
  initFreeType();
  setTargetResolution(app::mainWindow.width, app::mainWindow.height);

  FontFile::setLoadPadding(1);
  puts("Font terminus");
  Font::all.makeID(Font(FontFile("Terminus.ttf", 12)));
  puts("Font liberation");
  Font::all.makeID(Font(FontFile("LiberationMono-Regular.ttf", 16)));
}

text::Font::Font(const FontFile& fontfile)
{
  loadFontFile(fontfile);
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
	sizes[g] = glm::vec2((float)(quad.z) * pixel_size.x,
		(float)(quad.w) * pixel_size.y);
  }
  // load glyph metrics
  metrics.resize(fontfile.glyphs.metrics.size());
  for (size_t g = 0; g < metrics.size(); ++g) {
	const FontFile::Glyphs::Metric& met = fontfile.glyphs.metrics[ g ];
	metrics[g] = Metric(
		(float)met.advance * pixel_size.x,
		(float)met.bearingx * pixel_size.x,
		(float)met.bearingy * pixel_size.y);
  }

  linegap = (float)fontfile.linegap * pixel_size.y;

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

void text::Font::uploadChars() const
{
  gl::uploadStorage(charBuffer, sizeof(unsigned int) * chars.size(), &chars[0]);
}

void text::Font::uploadPositions() const
{
  gl::uploadStorage(posBuffer, sizeof(glm::vec2) * positions.size(), &positions[0]);
}

void text::Font::update() const
{
	uploadChars();
	uploadPositions();
}
void text::Font::reset()
{
	chars.clear();
	positions.clear();
}
void text::resetFonts()
{
  for (Font& font : Font::all) {
	font.reset();
  }
}
void text::updateFonts()
{
  for (const Font& font : Font::all) {
	font.update();
  }
}

void text::Font::use() const
{
	fontShader.bindUniformBuffer(posBuffer, "PosBuffer");
	fontShader.bindUniformBuffer(charBuffer, "CharBuffer");
	fontShader.bindUniformBuffer(uvBuffer, "UVBuffer");
	fontShader.bindUniformBuffer(sizeBuffer, "SizeBuffer");
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

void text::renderFonts()
{
  for (const Font& font : Font::all) {
	font.render();
  }
}

