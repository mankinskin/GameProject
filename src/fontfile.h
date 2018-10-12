#pragma once
#include "image.h"
#include "texture.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <vector>
#include <glm.hpp>
#include <cstring>

namespace text
{
  struct FontFile
  {
	struct Glyphs
	{
	  struct Metric
	  {
		unsigned int advance;
		unsigned int bearingx;
		unsigned int bearingy;

		Metric()
		{}
		Metric(unsigned int adv, unsigned int bx, unsigned int by)
		  : advance (adv)
		  , bearingx(bx)
		  , bearingy(by)
		{}
	  };
	  unsigned int count;
	  std::vector<glm::uvec4> quads;
	  std::vector<Metric> metrics;

	  void resize(size_t size)
	  {
		count = size;
		quads.resize(count);
		metrics.resize(count);
	  }
	  Glyphs()
		: count(0)
	  {}
	};

	Image atlas;
	Glyphs glyphs;
	unsigned int linegap;

	FontFile()
	{}

	std::string write();
	void read(std::string pFilename);

	void setLoadSize(const size_t ptx, const size_t pty = 0);
	void setLoadSize(const glm::uvec2 pt);
	void setLoadDpi(const glm::uvec2 pDpi);
	void setLoadDpi(const size_t ptx, const size_t pty = 0);
	std::string name = "";

	static void setLoadPadding(size_t padPixels);
	private:
	void readFontfile(std::string pFilepath);
	void readFace(std::string pFilepath);
	const std::string FONT_DIR = "assets/fonts/";
	glm::uvec2 size = glm::uvec2(4, 4);
	glm::uvec2 dpi = glm::uvec2(100, 100);
	static unsigned int padding;
	int ft_error = 0;
	unsigned int writeGlyphs(FILE* file);
	unsigned int readGlyphs(FILE* file);
  };

  int initFreeType();
};
