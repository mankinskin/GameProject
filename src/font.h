#pragma once
#include <vector>
#include <string>
#include "fontfile.h"
#include "shader.h"

namespace text
{
  struct Font
  {
	Font()
	{}
	Font(const FontFile& fontfile)
	{
	  loadFontFile(fontfile);
	}

	struct Metric
	{
	  Metric()
	  {}
	  Metric(float adv, float bx, float by)
		: advance(adv)
		, bearing(glm::vec2(bx, by))
	  {}
	  float advance;
	  glm::vec2 bearing;
	};

	void loadFontFile(const FontFile& fontfile);
	const Metric& getMetric(const size_t i) const;

	std::string name;
	float linegap;

	static gl::VAO fontVAO;
	static shader::Program fontShader;
	protected:
	gl::Storage<glm::vec4> uvBuffer;  // uv coordinates of glyphs in the atlas
	gl::Storage<glm::vec2> sizeBuffer; //
	std::vector<Metric> metrics;
	texture::Texture2D atlasTexture;  // contains all glyph textures
  };

  void initFontVAO();
  void initFontShader();
  void setupFontShader();

  void loadFonts();
}

