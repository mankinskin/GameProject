#pragma once
#include <vector>
#include <string>
#include "fontfile.h"
#include "shader.h"
#include "quad.h"

namespace text
{
  struct Text : public gui::Quad
  {
    Text(const glm::vec4& b)
      : Quad(b)
    {}

    size_t length() const { return str.length();	}
    void insert(const std::string& b, const size_t at)
    {
      str.insert(str.begin() + at, b.begin(), b.end());
    }
    void insert(const unsigned char& c, const size_t at)
    {
      str.insert(str.begin() + at, c);
    }
    void append(const std::string& b)
    {
      insert(b, str.size());
    }
    void append(const unsigned char c)
    {
      insert(c, str.size());
    }
    template<typename T>
      void operator<<(const T c)
      {
        append(c);
      }
    template<typename T>
      void operator+=(const T s)
      {
        append(s);
      }
    void operator=(const std::string s)
    {
      str = s;
    }
    operator std::string() const
    {
      return str;
    }
    void lineBreak()
    {
      cursor = 0.0f;
      ++line;
    }
    std::string str;  // is updated by a textbox
    float cursor = 0.0f;
    size_t line = 0;
    size_t bufBegin = 0;
    size_t bufSize = 0;
  };
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
    struct TextCache
    {
      std::string str;
    };

    void loadFontFile(const FontFile& fontfile);
    const Metric& getMetric(const size_t i) const;
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

    std::string name;
    float linegap;

    static gl::VAO fontVAO;
    static shader::Program fontShader;

    gl::Storage<glm::vec4> uvBuffer;  // uv coordinates of glyphs in the atlas
    gl::Storage<glm::vec2> sizeBuffer; //
    std::vector<Metric> metrics;
    texture::Texture2D atlasTexture;  // contains all glyph textures

    std::vector<Text> texts;
    std::vector<TextCache> textCaches;
    std::vector<glm::vec2> positions;
    std::vector<unsigned int> chars;
    gl::StreamStorage<unsigned int> charBuffer;	// codes of all characters to be drawn
    gl::StreamStorage<glm::vec2> posBuffer;	// char position buffer
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

