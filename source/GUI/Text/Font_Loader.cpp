#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "Font_Loader.h"
#define _USE_MATH_DEFINES
#include "../../BaseGL/shader/shader.h"
#include "..\..\BaseGL\texture.h"
#include "..\..\GlobalGL\glDebug.h"
#include "../../BaseGL/camera.h"
#include "..\..\BaseGL\VAO.h"
#include "..\..\Global\Debug.h"
#include <algorithm>
#include "../../Utility/dt/dt.h"
#include "../../Utility/dt/image.h"
#include <fstream>
#include "../../BaseGL/ContextWindow.h"

#define DEFAULT_TTF_DIR "../assets//fonts//"
#define DEFAULT_STORE_DIR "../assets//glyph_atlas_fonts//"

struct LoadFontMetrics {
	size_t ppem_x;
	size_t ppem_y;
	size_t scale_x;
	size_t scale_y;
	size_t lineGap;
};

struct LoadGlyphQuad {
	size_t minX;
	size_t minY;
	size_t maxX;
	size_t maxY;
};

struct LoadGlyphMetrics {
	size_t advanceX;
	int xBearing;
	int yBearing;
};

struct LoadAtlas {
	size_t width;
	size_t height;
	int flags;
	std::vector<LoadGlyphQuad> quads;
	std::vector<unsigned char> buffer;
};

struct LoadFont {
	std::string fileName;
	gui::text::FontInstructions instructions;
	LoadFontMetrics fontMetrics;
	LoadAtlas atlas;
	std::vector<LoadGlyphMetrics> metrics;
	std::vector<int> kerningMap;
};

std::vector<LoadFont> loadFontBuffer = {};
std::string font_input_directory = DEFAULT_TTF_DIR;
std::string font_store_directory = DEFAULT_STORE_DIR;
FT_Library ftLib;
int showDistanceField = 1;
/*
PIPELINE FOR FONT LOADING
- Include desired font paths
	- set load instructions for each font
- loadFonts

*/
//template<typename T>
//inline T square(T p) {
//	return p*p;
//}


std::vector<int> getKerningMap(FT_Face pFace, size_t pStartCode, size_t pGlyphCount)
{
	std::vector<int> kerning(pGlyphCount*pGlyphCount);
	//Get Kerning
	if (FT_HAS_KERNING(pFace)) {
		for (size_t g = 0; g < pGlyphCount; ++g) {
			for (size_t t = 0; t < pGlyphCount; ++t) {
				FT_Vector vec = FT_Vector();
				FT_Get_Kerning(pFace, FT_Get_Char_Index(pFace, pStartCode + g), FT_Get_Char_Index(pFace, pStartCode + t), 0, &vec);
				if (vec.x) {
					vec.x = vec.x;
				}
				kerning[g*pGlyphCount + t] = (vec.x) / 64;
			}
		}
	}
	return kerning;
}



void setFontSize(FT_Face& pFace, LoadFontMetrics& pFontMetrics, gui::text::FontInstructions& pInstructions) {

	size_t dpi_x = 100;
	size_t dpi_y = 100;
	if (pInstructions.flags & 1) {
		dpi_x *= pInstructions.upsampling;
		dpi_y *= pInstructions.upsampling;
	}
	FT_Set_Char_Size(pFace, pInstructions.pointSize << 6, 0, dpi_x, dpi_y);
	//FT_Set_Pixel_Sizes(pFace, pInstructions.pointSize * pInstructions.upsampling, 0);
	pFontMetrics.scale_x = pFace->size->metrics.x_scale;
	pFontMetrics.scale_y = pFace->size->metrics.y_scale;
	pFontMetrics.ppem_x = pFace->size->metrics.x_ppem;
	pFontMetrics.ppem_y = pFace->size->metrics.y_ppem;
	//pFontMetrics.lineGap = FT_MulFix(pFace->size->metrics.ascender - pFace->size->metrics.descender, pFace->size->metrics.y_scale)/64;

}





void loadAtlas(FT_Face& pFace, gui::text::FontInstructions& pFontInstructions, LoadFontMetrics& pFontMetrics, LoadAtlas& pAtlas, std::vector<LoadGlyphMetrics>& pMetrics)
{
	const size_t width_glyphs = (size_t)ceil(sqrt((float)pFontInstructions.glyphCount));
	size_t pad_pixels = 0;
	pad_pixels += pad_pixels % 2;
	size_t maxGlyphWidth = FT_MulFix(pFace->bbox.xMax - pFace->bbox.xMin, pFontMetrics.scale_x) / 64; //max width of the actual glyph bitmap
	size_t maxGlyphHeight = FT_MulFix(pFace->bbox.yMax - pFace->bbox.yMin, pFontMetrics.scale_y) / 64;//max height of the actual glyph bitmap

	size_t spread_pixels = 0; // glyph will be rendered with distance field
	float spread_dist = 0.0f;
	float size_scale = 1.0f;
	if (pFontInstructions.flags & 1) {
		size_scale = 1.0f / (float)pFontInstructions.upsampling;
		spread_dist = 5.0f;
		spread_pixels = (size_t)ceil(spread_dist);

		//if this flag is set, FreeType has loaded the glyph bitmaps with a higher resolution than needed (*upsampling)
		//therefore all sizes have to be scaled back down to their actual, screen fitted sizes
		maxGlyphWidth = (size_t)ceil(((float)maxGlyphWidth) * size_scale + spread_dist * 2);
		maxGlyphHeight = (size_t)ceil(((float)maxGlyphHeight) * size_scale + spread_dist * 2);
	}
	const size_t atlasMaxAdvanceX = maxGlyphWidth + pad_pixels * 2;
	const size_t atlasMaxAdvanceY = maxGlyphHeight + pad_pixels * 2;

	const size_t preWidth = (atlasMaxAdvanceX * width_glyphs);
	const size_t preHeight = (atlasMaxAdvanceY * width_glyphs);

	std::vector<unsigned char> buf(preWidth*preHeight);
	pAtlas.quads.resize(pFontInstructions.glyphCount);
	pMetrics.resize(pFontInstructions.glyphCount);

	size_t maxAtlasWidth = 0; //the maximum width of the final atlas (e.g. the maximum line width measured while loading the glyphs line-by-line into the atlas
	size_t thisLineMaxY = 0; // the maximum height of the current line(e.g. the distance to advance by when advancing to the next line)
	glm::uvec2 cursor = glm::uvec2();

	int maxAscend = 0; //for taking an accurate linegap distance. take the max value of all glyphs for these
	int maxDescend = 0;
	for (size_t g = 0; g < pFontInstructions.glyphCount; ++g) {
		//FreeType loads binary bitmap with 1 bit per pixel
		int flags = FT_LOAD_RENDER;
		if (pFontInstructions.flags & 1) {
			//for DT fonts, the bitmap should be a binary image 
			flags = flags | FT_LOAD_TARGET_MONO;
		}
		FT_Load_Char(pFace, pFontInstructions.startCode + g, flags);
		FT_GlyphSlot& gly = pFace->glyph;

		glm::uvec2 outlineSize = glm::uvec2(gly->bitmap.width, gly->bitmap.rows); //the size the the original glyph bitmap, loaded by freetype. if distance field is enabled, spread pixels will be added
		glm::uvec2 dtSize = glm::uvec2(gly->bitmap.width + spread_pixels * 2, gly->bitmap.rows + spread_pixels * 2);
		LoadGlyphMetrics met;
		met.advanceX = size_t((gly->metrics.horiAdvance >> 6) * (size_scale));
		met.xBearing = size_t(((gly->metrics.horiBearingX >> 6)) * size_scale - (int)spread_pixels);//gly->bitmap_left; //
		met.yBearing = size_t(((gly->metrics.horiBearingY >> 6)) * size_scale + (int)spread_pixels);//gly->bitmap_top; //

		glm::uvec2 tileSize = glm::uvec2(outlineSize.x * size_scale + spread_pixels * 2, outlineSize.y * size_scale + spread_pixels * 2);
		//the space dedecated to the current glyph in the atlas (without padding)

		//set atlas uv coords using the direct size of the current glyph, which may actually still need to be downsampled
		//glyph usage data
		LoadGlyphQuad qud;
		qud.minX = cursor.x + pad_pixels;
		qud.minY = cursor.y + pad_pixels;
		qud.maxX = qud.minX + tileSize.x;
		qud.maxY = qud.minY + tileSize.y;


		if (pFontInstructions.flags & 1) {
			
			image<unsigned char> inImage(dtSize.x, dtSize.y);
			image<float> innerOutImg(dtSize.x, dtSize.y);
			image<float> outerOutImg(dtSize.x, dtSize.y);
			size_t pitch = abs(gly->bitmap.pitch);//how many bytes per bmp row

			//decode binary bitmap to full char bitmap by reading it bit by bit
			for (size_t line = 0; line < gly->bitmap.rows; ++line) {//it all rows of binary bitmap
				size_t remaining_pixels = gly->bitmap.width;
				for (size_t byt = 0; byt < pitch; ++byt) {//it all bytes per row
					unsigned char chr = gly->bitmap.buffer[(line*pitch) + byt];
					size_t read_bits = std::min((size_t)8, remaining_pixels);
					remaining_pixels -= read_bits;
					//the byte 
					//it holds the binary values for 8 pixels in the final image
					for (size_t bit = 0; bit < read_bits; ++bit) { //it all 8 bits of this byte
						unsigned char val = (chr >> (7 - bit)) & 1;
						size_t posx = (byt * 8) + bit + spread_pixels;
						size_t posy = line + spread_pixels; //the pos in the freetype bmp
						inImage.data[(posy*dtSize.x) + posx] = val; //the coord in the DT image ( + spread_pixels)
					}
				}
			}


			innerOutImg = *dt(&inImage, 0);//perform distance transform for the glyph image
			outerOutImg = *dt(&inImage, 1);//once inner and outer DT, because we want 
			float innerMaxDistance = 0.0f;

			//find max 
			for (size_t h = 0; h < dtSize.y; ++h) {
				for (size_t w = 0; w < dtSize.x; ++w) {
					innerMaxDistance = std::max(innerMaxDistance, innerOutImg.data[h*dtSize.x + w]);
				}
			}
			innerMaxDistance = std::min(spread_dist, sqrt(innerMaxDistance));
			/*  In      Out
			   43210 | 01234567
			   43210 | 01234567
			   43210 | 01234567
			   43210 | 01234567
			   ^     ^        ^spread_pixels
			   ^     ^Outline
			   ^innerMaxDistance
			*/
			float outerMaxDist = spread_dist;
			float max_range = innerMaxDistance + spread_dist;
			size_t bufpos = (cursor.y + pad_pixels) * preWidth + cursor.x + pad_pixels;
			for (size_t h = 0; h < dtSize.y; ++h) {
				for (size_t w = 0; w < dtSize.x; ++w) {

					float outDist = std::min(spread_dist, sqrt(outerOutImg.data[h*dtSize.x + w])); //values from 0 to outerMaxDistance
					float inDist = std::min(spread_dist, sqrt(innerOutImg.data[h*dtSize.x + w])); //values from 0 to innerMaxDistance

					float val = ((inDist - outDist) + spread_dist) / max_range;
					//distance values are never more than innerMaxDistance
					//outside outline distances are negative
					//add innerMaxDistance to map all distances to a range of [0, innerMaxDistance*2(max_dist)]
					//devide by max_dist to get a range [0.0f, 1.0f] where the glyph center is 1 and the outline is 0.5
					//unfortunately this only applies to the thickest parts of the glyph, since innerMaxDistance is the max distance from the outline of the entire glyph
					//what is needed is 
					val = val*(float)square(size_scale);
					size_t pos = (size_t)((h*size_scale)*preWidth + (w*size_scale));
					buf[bufpos + pos] += (unsigned char)(val *255.0f);
				}

			}
			//endif FONT_LOAD_DT
		}
		else {//regular load

			for (size_t line = 0; line < gly->bitmap.rows; ++line) {
				std::memcpy(&buf[((line + qud.minY)*preWidth) + qud.minX], &gly->bitmap.buffer[(line*gly->bitmap.width)], sizeof(unsigned char)*gly->bitmap.width);
			}
		}


		//advance atlas cursor
		cursor.x += (tileSize.x + pad_pixels * 2);
		thisLineMaxY = std::max(tileSize.y + pad_pixels * 2, thisLineMaxY);
		maxAscend = std::max(maxAscend, met.yBearing);
		maxDescend = std::max(maxDescend, (int)(outlineSize.y) - (met.yBearing));
		pAtlas.quads[g] = qud;
		pMetrics[g] = met;

		//check whether to start a new line of glyphs in the atlas
		if ((cursor.x + (atlasMaxAdvanceX)) > preWidth || g + 1 == pFontInstructions.glyphCount) {
			maxAtlasWidth = std::max(maxAtlasWidth, cursor.x);
			cursor.x = 0;
			cursor.y += thisLineMaxY;

		}

	}

	pAtlas.width = maxAtlasWidth;
	pAtlas.height = cursor.y;

	const size_t totalPixelCount = pAtlas.width * pAtlas.height;
	pAtlas.buffer.resize(totalPixelCount);
	pFontMetrics.lineGap = maxAscend + maxDescend;
	for (size_t h = 0; h < pAtlas.height; ++h) {
		std::memcpy(&pAtlas.buffer[h*pAtlas.width], &buf[h*preWidth], sizeof(unsigned char)*pAtlas.width);
	}
}

void loadFont(LoadFont& pFont)
{


	FT_Face ftface;
	std::string fontPath = font_input_directory + pFont.fileName + ".ttf";
	gui::text::FontInstructions& font_instructions = pFont.instructions;

	FT_New_Face(ftLib, (fontPath).c_str(), 0, &ftface);


	pFont.instructions.pointSize = font_instructions.pointSize;

	setFontSize(ftface, pFont.fontMetrics, pFont.instructions);
	loadAtlas(ftface, font_instructions, pFont.fontMetrics, pFont.atlas, pFont.metrics);
	pFont.kerningMap = getKerningMap(ftface, font_instructions.startCode, font_instructions.glyphCount);


	FT_Done_Face(ftface);

}
void gui::text::initializer::
setFontInputDir(std::string pNewDirectory) {

	font_input_directory = pNewDirectory;
}

void gui::text::initializer::
setFontStoreDir(std::string pNewDirectory) {

	font_store_directory = pNewDirectory;
}
void gui::text::initializer::
initFreeType()
{
	if (FT_Init_FreeType(&ftLib)) {
		debug::pushError("Failed to init FreeType!");
	}

	glDebug::getGLError("initFOntLoader():");
}

size_t gui::text::initializer::
includeFont(std::string pFontFileName, size_t pPointSize, size_t pStartCode, size_t pGlyphCount, int pFlags, size_t pUpsampling)
{
	return includeFont(pFontFileName, FontInstructions(pPointSize, pStartCode, pGlyphCount, pFlags, pUpsampling));
}
void removeFileExtension(std::string& pFileName) {
	size_t nameCharCount = pFileName.size();
	for (size_t i = 0; i < 4; ++i) {
		if (pFileName[nameCharCount - i - 1] == '.') {
			nameCharCount = nameCharCount - i - 1;
			break;
		}
	}
	pFileName.resize(nameCharCount);
}
size_t gui::text::initializer::
includeFont(std::string pFontFileName, FontInstructions& pLoadInstructions = gui::text::FontInstructions())
{
	size_t sz = loadFontBuffer.size();
	LoadFont font;
	removeFileExtension(pFontFileName);
	font.fileName = pFontFileName;
	font.instructions = pLoadInstructions;
	loadFontBuffer.push_back(font);
	return sz;
}
size_t gui::text::initializer::
createFontInstructions(size_t pPointSize, size_t pStartCode, size_t pGlyphCount, int pFlags, size_t pUpsampling)
{
	return createFontInstructions(gui::text::FontInstructions(pPointSize, pStartCode, pGlyphCount, pFlags, pUpsampling));
}

size_t gui::text::initializer::
createFontInstructions(gui::text::FontInstructions & pInstructions)
{
	size_t sz = allFontInstructions.size();
	size_t i = std::find(allFontInstructions.begin(), allFontInstructions.end(), pInstructions) - allFontInstructions.begin();
	if (i < sz) {
		return i;
	}
	allFontInstructions.push_back(pInstructions);
	return sz;
}

//Font file integration
void storeGlyphs(gui::text::Font& pFont, const LoadFont & pLoadFont)
{
	size_t glyCount = pLoadFont.atlas.quads.size();
	std::vector<gui::text::Glyph> glyphs(glyCount);
	pFont.metricOffset = gui::text::allMetrics.size();
	pFont.metricCount = glyCount;
	gui::text::allMetrics.resize(pFont.metricOffset + pFont.metricCount);

	for (size_t g = 0; g < glyCount; ++g) {
		const LoadGlyphQuad& qud = pLoadFont.atlas.quads[g];
		const LoadGlyphMetrics& met = pLoadFont.metrics[g];
		glyphs[g] = gui::text::Glyph((float)qud.minX / (float)pLoadFont.atlas.width, (float)qud.minY / (float)pLoadFont.atlas.height,
			(float)(qud.maxX) / (float)pLoadFont.atlas.width, (float)(qud.maxY) / (float)pLoadFont.atlas.height);
		gui::text::allMetrics[pFont.metricOffset + g] = gui::text::GlyphMetrics((float)(qud.maxX - qud.minX) / ((float)gl::screenWidth / 2.0f), (float)(qud.maxY - qud.minY) / ((float)gl::screenHeight / 2.0f),
			(float)met.advanceX / ((float)gl::screenWidth / 2.0f), (float)met.xBearing / ((float)gl::screenWidth / 2.0f), (float)met.yBearing / ((float)gl::screenHeight / 2.0f));
		//allMetrics[pSize.metricOffset + g] = GlyphMetrics((float)met.width, (float)met.height, (float)met.advanceX, (float)met.xBearing, (float)met.yBearing);
	}
	pFont.glyphStorageIndex = vao::createStorage(sizeof(gui::text::Glyph)*glyCount, &glyphs[0], 0);
	vao::bindStorage(GL_UNIFORM_BUFFER, pFont.glyphStorageIndex);

}

std::pair<size_t, size_t> convertKerning(std::vector<int>& pKerningMap)
{
	size_t count = pKerningMap.size();
	size_t offset = gui::text::allKerning.size();
	gui::text::allKerning.resize(offset + count);
	for (size_t i = 0; i < count; ++i) {
		gui::text::allKerning[offset + i] = (float)pKerningMap[i] / ((float)gl::screenWidth / 2.0f);
	}
	return std::pair<size_t, size_t>(offset, count);
}

gui::text::Font integrateFont(LoadFont& pFont) {
	glDebug::getGLError("before integrateFont():");
	debug::printErrors();
	gui::text::Font font;//TODO make font support multiple fontSizes
	glGenTextures(1, &font.atlasID);
	glBindTexture(GL_TEXTURE_2D, font.atlasID);

	int mipmap_min_flag = GL_NEAREST_MIPMAP_LINEAR;
	int mipmap_mag_flag = GL_NEAREST;

	if (pFont.instructions.flags & 1) {
		mipmap_min_flag = GL_LINEAR_MIPMAP_LINEAR;
		mipmap_mag_flag = GL_LINEAR;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap_min_flag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mipmap_mag_flag);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	storeGlyphs(font, pFont);
	std::pair<size_t, size_t> range = convertKerning(pFont.kerningMap);
	font.kerningOffset = range.first;
	font.kerningCount = range.second;
	font.fontMetric.lineGap = (float)pFont.fontMetrics.lineGap / ((float)gl::screenHeight / 2.0f);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, pFont.atlas.width, pFont.atlas.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &pFont.atlas.buffer[0]);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	size_t off = gui::text::allFontInstructions.size();
	gui::text::allFontInstructions.push_back(pFont.instructions);
	font.instructions = off;
	return font;
}

void gui::text::initializer::
loadFonts()
{

	size_t queueCount = loadFontBuffer.size();
	size_t fontCount = allFonts.size();
	allFonts.resize(fontCount + queueCount);
	for (size_t f = 0; f < queueCount; ++f) {
		LoadFont load_font = loadFontBuffer[f];
		loadFont(load_font);
		Font font = integrateFont(load_font);
		allFonts[fontCount + f] = font;
	}
	fontCount += queueCount;
	loadFontBuffer.clear();
}


//TODO: Font Serialization

