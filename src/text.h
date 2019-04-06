#pragma once
#include <string>
#include "quad.h"

namespace text {
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
}
