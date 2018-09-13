#pragma once
#include "font.h"
#include <glm.hpp>
#include <vector>
#include "utils/id.h"

namespace text
{
  struct Text
  {
	public:
	  static utils::Container<Text> all;
	  struct ID : public utils::ID<Text>
	  {
		ID()
		  : utils::ID<Text>(all)
		{}
		ID(size_t i)
		  : utils::ID<Text>(i, all)
		{}
		ID(utils::ID<Text> id)
		  : utils::ID<Text>(id)
		{}
	  };
	  glm::vec2 position;
	  glm::vec2 size;
	  Text(glm::vec2 pPos = glm::vec2(0.0f, 0.0f), glm::vec2 pSize = glm::vec2(1.0f, 1.0f))
		: font(Font::ID(0))
		, position(pixel_quantize(pPos))
		, size(pixel_quantize(pSize))
	  {}
	  void setChars(const std::string& str);
	  void writeChars();
	  void writeWord(unsigned int start, unsigned int length);
	  void setFont(Font::ID);
	  size_t lineCount()
	  {
		return floor(size.y / font->linegap);
	  }
	private:
	  void writeChar(const unsigned char& c, glm::vec2 pos);
	  void lineBreak();
	  float cursor; // relative to pos
	  size_t line;
	  std::string str;
	  unsigned int bufferBegin = 0; // begin of the data in the font buffers
	  Font::ID font;
  };
  extern unsigned int tabsize;

  void updateTexts();
}
