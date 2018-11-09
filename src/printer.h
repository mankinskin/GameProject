#pragma once
#include "font.h"
#include "text.h"

namespace text
{
  struct PrintedText;

  struct Printer : public Font
  {
	using Container = utils::Container<Printer>;
	static Container all;

	struct ID : public Container::ID
	{
	  ID()
		: utils::ID<Printer>(&all)
	  {}
	  ID(const size_t i)
		: utils::ID<Printer>(i, &all)
	  {}
	};

	Printer(const Font& font);
	void use() const;
	void render() const;
	void uploadChars() const;
	void uploadPositions() const;
	void update() const;
	void clear();
	void reserveChars(const size_t);
	void setCharCode(const size_t, const size_t);
	void setCharPos(const size_t, glm::vec2);
	void pushCharCode(const size_t);
	void pushCharPos(const glm::vec2);
	size_t getCharCount() const;
	std::vector<PrintedText> texts;
	std::vector<glm::vec2> positions;
	std::vector<unsigned int> chars;
	gl::StreamStorage<unsigned int> charBuffer;	// codes of all characters to be drawn
	gl::StreamStorage<glm::vec2> posBuffer;	// char position buffer
	size_t charCount = 0;
	size_t bufferBegin = 0; // begin of the data in the font buffers
	void writeWord(const Textbox&, size_t start, size_t length);
	void printTextbox(const Textbox&);
	void lineBreak();
	float cursor;
	size_t line;
  };

  struct PrintedText
  {
	std::string str;
	glm::vec4 box;
	Printer::ID printer;
  };

  extern size_t tabsize;
  void updateTexts();
  void renderTexts();
}
