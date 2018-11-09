#pragma once
#include "font.h"
#include <glm.hpp>
#include <vector>
#include "utils/id.h"
#include "quad.h"
#include "viewport.h"

namespace text
{
  struct Text
  {
	  Text()
	  {}
	  Text(std::string s)
		: str(s)
	  {}

	  size_t length() const { return str.length();	}
	  void append(const std::string s)
	  {
		str.insert(str.end(), s.begin(), s.end());
	  }
	  void append(const unsigned char c)
	  {
		str.push_back(c);
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
	protected:
	  std::string str;
  };
  struct Textbox : public gui::Quad, Text
  {
	public:
	  using Text::str;
	  static utils::Container<Textbox> all;
	  struct ID : public utils::ID<Textbox>
	  {
		ID()
		  : utils::ID<Textbox>(&all)
		{}
		ID(const size_t i)
		  : utils::ID<Textbox>(i, &all)
		{}
		ID(const utils::ID<Textbox>& id)
		  : utils::ID<Textbox>(std::forward<const utils::ID<Textbox>&>(id))
		{}
		ID(utils::ID<Textbox>&& id)
		  : utils::ID<Textbox>(std::move(id))
		{}
	  };
	  Textbox(glm::vec2 pPos = glm::vec2(0.0f, 0.0f), glm::vec2 pSize = glm::vec2(1.0f, 1.0f))
		: Quad(gl::pixel_round(pPos), gl::pixel_round(pSize))
		, font(Font::ID(0))
	  {}
	  void setString(const std::string& str);
	  std::string getString() const;
	  void writeString();
	  void setFont(Font::ID);

	  size_t lineCount()
	  {
		return floor(w / font->linegap);
	  }
	private:
	  void writeWord(size_t start, size_t length);
	  void lineBreak();
	  float cursor; // relative to pos
	  size_t line;
	  Font::ID font;
	  size_t bufferBegin = 0; // begin of the data in the font buffers
	  size_t bufferLength = 0;
  };
  extern size_t tabsize;

  void updateTextboxes();
  void updateText();
}
