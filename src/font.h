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
	void reset();
	void reserveChars(const size_t);
	void setCharCode(const size_t, const size_t);
	void setCharPos(const size_t, glm::vec2);
	void pushCharCode(const size_t);
	void pushCharPos(const glm::vec2);
	const Metric& getMetric(const size_t i) const;
	size_t getCharCount() const;

	std::string name;
	float linegap;

	static gl::VAO fontVAO;
	static shader::Program fontShader;
	private:
	std::vector<glm::vec2> positions;
	std::vector<unsigned int> chars;
	std::vector<Metric> metrics;
	size_t charCount = 0;
	void use() const;
	gl::Storage<glm::vec4> uvBuffer;  // uv coordinates of glyphs in the atlas
	gl::Storage<glm::vec2> sizeBuffer; //
	gl::StreamStorage<unsigned int> charBuffer;	// codes of all characters to be drawn
	gl::StreamStorage<glm::vec2> posBuffer;	// char position buffer
	texture::Texture2D atlasTexture;  // contains all glyph textures
  };

  void loadFonts();
  void resetFonts();

  void initFontVAO();

  void initFontShader();
  void setupFontShader();
  void updateFonts();
  void renderFonts();

  void renderFont();
}

