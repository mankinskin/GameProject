#pragma once
#include "gui.h"
#include <glm.hpp>
#include <vector>
#include "utils/id.h"
#include "quad.h"
#include "viewport.h"
#include "printer.h"

namespace text
{
  struct Text
  {
	  Text()
	  {}
	  Text(const std::string& b)
		: buf(b)
	  {}

	  size_t length() const { return buf.length();	}
	  void insert(const std::string& b, const size_t at)
	  {
		buf.insert(buf.begin() + at, b.begin(), b.end());
	  }
	  void insert(const unsigned char& c, const size_t at)
	  {
		buf.insert(buf.begin() + at, c);
	  }
	  void append(const std::string& b)
	  {
		insert(b, buf.size());
	  }
	  void append(const unsigned char c)
	  {
		insert(c, buf.size());
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
	  std::string buf;
  };

  struct Textbox : public gui::Quad, Text
  {
	public:
	using Text::buf;
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
	  , printer(Printer::ID(0))
	{}

	void setString(const std::string& str);
	void setFont(const size_t printerID);
	std::string getString() const;

	template<typename T>
	  void operator<<(const T str)
	  {
		Text::operator<<(str);
	  }
	Printer::ID printer;
	size_t bufferBegin = 0; // begin of the data in the font buffers
	void writeWord(size_t start, size_t length);
	void writeText();
	void lineBreak();
	float cursor;
	size_t line;
  };
  void updateTextboxes();
}
