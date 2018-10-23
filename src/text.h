#pragma once
#include "font.h"
#include <glm.hpp>
#include <vector>
#include "utils/id.h"
#include "quad.h"

namespace text
{
  struct Textbox : public gui::Quad
  {
	public:
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
		: Quad(pixel_quantize(pPos), pixel_quantize(pSize))
		, font(Font::ID(0))
	  {}
	  void setChars(const std::string& str);
	  void writeChars();
	  void writeWord(unsigned int start, unsigned int length);
	  void setFont(Font::ID);
	  size_t lineCount()
	  {
		return floor(w / font->linegap);
	  }
	private:
	  void writeChar(const unsigned char& c, glm::vec2 pos);
	  void lineBreak();

	  Font::ID font;
	  float cursor; // relative to pos
	  size_t line;
	  std::string str;
	  unsigned int bufferBegin = 0; // begin of the data in the font buffers
  };
  extern unsigned int tabsize;

  void updateTextboxes();
}
