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
		size_t advance;
		size_t bearingx;
		size_t bearingy;

		Metric()
		{}
		Metric(size_t adv, size_t bx, size_t by)
		  : advance (adv)
		  , bearingx(bx)
		  , bearingy(by)
		{}
	  };
	  size_t count;
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
	size_t linegap;

	FontFile()
	{}
	FontFile(const std::string pFilePath, const size_t pointSize = 11)
	{
	  setLoadDpi(app::mainWindow.monitor->dpi);
	  setLoadSize(pointSize);
	  read(pFilePath);
	}

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
	int ft_error = 0;
	size_t writeGlyphs(FILE* file);
	size_t readGlyphs(FILE* file);
	static size_t padding;
  };

  int initFreeType();
};
