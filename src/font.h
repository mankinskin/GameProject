#pragma once
#include <vector>
#include <string>
#include "fontfile.h"
#include "shader.h"
#include "quad.h"
#include "text.h"

namespace text
{
  struct FontData
  {
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
    FontData(const FontFile& fontfile);
    const FontData::Metric& getMetric(const size_t i) const;

    std::string name;
    float linegap;
    gl::Storage<glm::vec4> uvBuffer;  // uv coordinates of glyphs in the atlas
    gl::Storage<glm::vec2> sizeBuffer; //
    std::vector<Metric> metrics;
    texture::Texture2D atlasTexture;  // contains all glyph textures
    gl::StreamStorage<unsigned int> charBuffer;	// codes of all characters to be drawn
    gl::StreamStorage<glm::vec2> posBuffer;	// char position buffer
  };

  struct Font : public FontData
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

    Font(const FontFile& fontfile)
      : FontData(fontfile)
    {}

    void setCharCode(const size_t, const size_t);
    void setCharPos(const size_t, glm::vec2);
    void pushCharCode(const size_t);
    void pushCharPos(const glm::vec2);
    void reserveChars(const size_t);
    size_t getCharCount() const;
    void render() const;
    void update();
    void use() const;
    void clearTexts();
    void writeWord(Text&, size_t start, size_t length);
    void writeText(Text&);
    void updateText(Text&);
    size_t makeText(glm::vec4);

    // OpenGL
    static gl::VAO fontVAO;
    static shader::Program fontShader;

    // Management
    std::vector<Text> texts;
    std::vector<glm::vec2> positions;
    std::vector<unsigned int> chars;
    size_t charCount = 0;
  };

  extern size_t tabsize;

  void initFontVAO();
  void initFontShader();
  void setupFontShader();

  void loadFonts();
  void updateTexts();
  void renderTexts();
}

