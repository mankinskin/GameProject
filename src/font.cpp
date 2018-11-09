#include "font.h"
#include <unistd.h>
#include <fcntl.h>
#include "vao.h"
#include "primitives.h"
#include "app.h"
#include "contextwindow.h"
#include "viewport.h"

gl::VAO text::Font::fontVAO;
shader::Program text::Font::fontShader;


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

