#include "fontfile.h"
#include "utils/path.h"

FT_Library ftLib;
size_t text::FontFile::padding = 0;

void text::FontFile::setLoadPadding(size_t padPixels)
{
  padding = padPixels;
}

void text::FontFile::setLoadSize(const size_t ptx, const size_t pty)
{
  setLoadSize(glm::uvec2(ptx, pty));
}
void text::FontFile::setLoadSize(const glm::uvec2 pt)
{
  if (!pt.x) {
	size.x = pt.y;
	size.y = pt.y;
  }
  else if (!pt.y) {
	size.x = pt.x;
	size.y = pt.x;
  }
}

void text::FontFile::setLoadDpi(const size_t ptx, const size_t pty)
{
  setLoadDpi(glm::uvec2(ptx, pty));
}
void text::FontFile::setLoadDpi(const glm::uvec2 pDpi)
{
  if (!pDpi.x) {
	dpi.x = pDpi.y;
	dpi.y = pDpi.y;
  }
  else if (!pDpi.y) {
	dpi.x = pDpi.x;
	dpi.y = pDpi.x;
  }
}

void text::FontFile::read(std::string pFilename)
{
  std::string filepath = FONT_DIR + pFilename;
  std::string extension = path::getExtension(pFilename);
  name = pFilename;
  if (extension == "font") {
	readFontfile(filepath);
  }
  else {
	readFace(filepath);
  }
}

void text::FontFile::readFace(std::string pFilepath)
{
  FT_Face face;
  ft_error = FT_New_Face(ftLib, pFilepath.c_str(), 0, &face);
  if (ft_error || !face) {
	printf("Could not load font %s\n", pFilepath.c_str());
	return;
  }

  FT_Set_Char_Size(face, size.x*64, size.y*64, dpi.x, dpi.y);
  glyphs.resize(face->num_glyphs);

  size_t max_glyph_width = (face->bbox.xMax - face->bbox.xMin) / 64;
  linegap = (face->bbox.yMax - face->bbox.yMin) / 64;
  size_t row_length = std::ceil(std::sqrt(face->num_glyphs));
  size_t max_atlas_width = row_length * max_glyph_width;

  size_t row_max_height = 0;
  size_t max_row_width = 0;

  glm::uvec2 cursor = glm::uvec2(0, 0);

  // precalculate glyph quads
  FT_Int32 load_mode = FT_LOAD_TARGET_MONO;
  FT_Render_Mode_ render_mode = FT_RENDER_MODE_MONO;

  for (size_t gi = 0; gi < glyphs.count; ++gi) {
	FT_Load_Char(face, gi, load_mode);
	FT_Render_Glyph(face->glyph, render_mode);
	FT_GlyphSlot glyph = face->glyph;

	Glyphs::Metric& met = glyphs.metrics[ gi ];
	met = Glyphs::Metric(
		glyph->metrics.horiAdvance/64,
		glyph->metrics.horiBearingX/64,
		glyph->metrics.horiBearingY/64);

	glm::uvec4& quad = glyphs.quads[gi];

	size_t width = glyph->bitmap.width;
	size_t height = glyph->bitmap.rows;

	quad = glm::uvec4(
		cursor.x + padding,
		cursor.y + padding,
		width,
		height);

	cursor.x += width + padding * 2;
	row_max_height = std::max(height + padding * 2, row_max_height);

	// at the end of a row, do a line break
	if ((gi + 1) % row_length == 0) {
	  max_row_width = std::max((size_t)cursor.x, max_row_width);
	  cursor.x = 0;
	  cursor.y += row_max_height;
	  row_max_height = 0;
	}
  }

  // if glyphs.count is not divisible by row_length
  // a new row had already been started but not finished
  if (glyphs.count % row_length) {
	cursor.y += row_max_height;
  }
  atlas.width = max_row_width;
  atlas.height = cursor.y;
  cursor = glm::uvec2(0, 0);
  atlas.pixels = (unsigned char*)malloc(atlas.width * atlas.height);

  // now write glyph bitmaps to glyph quads in atlas
  for (size_t gi = 0; gi < glyphs.count; ++gi) {
	FT_Load_Char(face, gi, load_mode);
	FT_Render_Glyph(face->glyph, render_mode);
	FT_Bitmap& bitmap = face->glyph->bitmap;

	glm::uvec4& quad = glyphs.quads[ gi ];

	if (load_mode & FT_LOAD_TARGET_MONO) {
	  for (size_t row = 0; row < bitmap.rows; ++row) {
		for (size_t byte = 0; byte < bitmap.pitch; ++byte) {
		  const unsigned char& bitmap_byte = bitmap.buffer[row * bitmap.pitch + byte];
		  for (size_t bit = 0; bit < 8; ++bit) {
			// take each bit from the glyph bitmap and map it onto a byte in the atlas
			atlas.pixels[(quad.y + row) * atlas.width + quad.x + byte * 8 + bit] =
			  (unsigned char)((bitmap_byte >> (7 - bit)) & 1) * 255;
		  }
		}
	  }
	} else {
	  for (size_t row = 0; row < bitmap.rows; ++row) {
		std::memcpy(&atlas.pixels[(quad.y + row) * atlas.width + quad.x],
			&bitmap.buffer[ row * bitmap.pitch ], bitmap.pitch);
	  }
	}
  }
  printf("Loaded Atlas: %s\nWidth %lu\nHeight: %lu\nSize: %u\nDpi: %u %u\n",
	  name.c_str(), atlas.width, atlas.height, size.x, dpi.x, dpi.y);
  printf("Linegap val: %lu\n", linegap);

  FT_Done_Face(face);
}

void text::FontFile::readFontfile(std::string pFilepath)
{
  FILE* file = fopen(pFilepath.c_str(), "rb");

  if (!file) {
	printf("Failed to open file %s\n", pFilepath.c_str());
	return;
  }

  size_t glyphs_bytes = readGlyphs(file);
  if (glyphs_bytes == 0) {
	printf("Font file %s invalid!", pFilepath.c_str());
	return;
  }
  fseek(file, glyphs_bytes, SEEK_SET);
  atlas.read(file);
  fclose(file);
}

size_t text::FontFile::readGlyphs(FILE* file)
{
  fread(&glyphs.count, sizeof(unsigned int), 1, file);
  if (glyphs.count == 0) {
	return 0;
  }
  glyphs.quads.resize(glyphs.count);
  glyphs.metrics.resize(glyphs.count);
  fread(&glyphs.quads[0], sizeof(glm::uvec4), glyphs.count, file);
  fread(&glyphs.metrics[0], sizeof(Glyphs::Metric), glyphs.count, file);

  size_t read_size = sizeof(unsigned int);
  read_size += sizeof(glm::uvec4) * glyphs.quads.size();
  read_size += sizeof(Glyphs::Metric) * glyphs.metrics.size();

  return read_size;
}

std::string text::FontFile::write()
{
  std::string outname = std::string(name + ".font");
  FILE* file = fopen(outname.c_str(), "wb");

  size_t glyphs_size = writeGlyphs(file);
  fseek(file, glyphs_size, SEEK_SET);
  atlas.write(file);

  fclose(file);
  return outname;
}

size_t text::FontFile::writeGlyphs(FILE* file)
{
  fwrite(&glyphs.count, sizeof(unsigned int), 1, file);
  fwrite(&glyphs.quads[0], sizeof(glm::uvec4), glyphs.count, file);
  fwrite(&glyphs.metrics[0], sizeof(Glyphs::Metric), glyphs.count, file);

  size_t write_size = sizeof(unsigned int);
  write_size += sizeof(glm::uvec4) * glyphs.quads.size();
  write_size += sizeof(Glyphs::Metric) * glyphs.metrics.size();

  return write_size;
}

int text::initFreeType()
{
  if (FT_Init_FreeType(&ftLib)) {
	puts("Failed to init FreeType!");
	return -1;
  }
  return 0;
}

