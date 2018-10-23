#pragma once
#include <vector>
#include <string>
#include "utils/id.h"
#include "fontfile.h"
#include "shader.h"

namespace text
{
  struct Font
  {
	using Container = utils::Container<Font>;
	static Container all;

	struct ID : public Container::ID
	{
	  ID()
		: utils::ID<Font>(&all)
	  {}
	  ID(const size_t i)
		: utils::ID<Font>(i, &all)
	  {}
	};

	Font()
	{}
	Font(const FontFile& font);

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
	void render() const;
	void uploadChars() const;
	void uploadPositions() const;
	void update() const;

	std::string name;
	std::vector<glm::vec2> positions;
	std::vector<unsigned int> chars;
	std::vector<Metric> metrics;
	size_t charCount = 0;
	float linegap;

	static gl::VAO fontVAO;
	static shader::Program fontShader;
	private:
	void use() const;
	gl::Storage<glm::vec4> uvBuffer;  // uv coordinates of glyphs in the atlas
	gl::Storage<glm::vec2> sizeBuffer; //
	gl::StreamStorage<unsigned int> charBuffer;	// codes of all characters to be drawn
	gl::StreamStorage<glm::vec2> posBuffer;	// char position buffer
	texture::Texture2D atlasTexture;  // contains all glyph textures
  };

  void setTargetResolution(const size_t, const size_t);
  void setTargetResolution(const glm::uvec2);
  extern glm::vec2 pixel_size;

  inline glm::vec2 pixel_quantize(glm::vec2 v)
  {
	return glm::round(v / pixel_size) * pixel_size;
  }

  void loadFonts();

  void initFontVAO();

  void initFontShader();
  void setupFontShader();
  void updateFonts();
  void renderFonts();

  void renderFont();
}

