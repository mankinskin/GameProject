#include "font.h"
#include "text.h"
#include "shader.h"

size_t MAX_CHARS = 1000;
unsigned int quadStorage = 0;
unsigned int charStorage = 0;
std::vector<gui::text::String> allStrings;

size_t styleStorage = 0;
std::vector<gui::text::TextStyle> allTextStyles;

unsigned int gui::text::fontVAO = 0;
unsigned int gui::text::glyphShaderProgram = 0;
std::vector<size_t> gui::text::glyphIndexBuffer;
std::vector<gui::text::Font> gui::text::allFonts;
std::vector<gui::text::FontInstructions> gui::text::allFontInstructions;
std::vector<gui::text::GlyphMetrics> gui::text::allMetrics;
std::vector<float> gui::text::allKerning;

void gui::text::initFontShader()
{
	glyphShaderProgram = shader::newProgram("glyphShapeShader", shader::createModule("glyphShapeShader.vert"), shader::createModule("glyphShapeShader.frag"));
	shader::addVertexAttribute(glyphShaderProgram, "pos", 0);
	shader::addVertexAttribute(glyphShaderProgram, "quad", 1);
	shader::addVertexAttribute(glyphShaderProgram, "index", 2);
}

void gui::text::
initFontVAO() {
	quadStorage = vao::createStorage(MAX_CHARS * sizeof(CharQuad), nullptr, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	charStorage = vao::createStorage(MAX_CHARS * sizeof(size_t), nullptr, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	vao::createStream(quadStorage, GL_MAP_WRITE_BIT);
	vao::createStream(charStorage, GL_MAP_WRITE_BIT);
	glCreateVertexArrays(1, &fontVAO);

	vao::setVertexAttrib(fontVAO, 0, 0, 2, GL_FLOAT, 0);
	vao::setVertexAttrib(fontVAO, 1, 1, 4, GL_FLOAT, 0);
	vao::setVertexAttrib(fontVAO, 2, 2, 1, GL_UNSIGNED_INT, 0);

	glVertexArrayBindingDivisor(fontVAO, 1, 1);
	glVertexArrayBindingDivisor(fontVAO, 2, 1);

	glVertexArrayElementBuffer(fontVAO, gl::quadEBO + 1);
	glVertexArrayVertexBuffer(fontVAO, 0, gl::quadVBO + 1, 0, sizeof(float) * 2);

	vao::setVertexArrayVertexStorage(fontVAO, 1, quadStorage, sizeof(CharQuad));
	vao::setVertexArrayVertexStorage(fontVAO, 2, charStorage, sizeof(size_t));
}

void gui::text::revalidateFontStringIndices()
{
	size_t off = 0;
	for (Font& fon : allFonts) {
		fon.stringOffset = off;
		off += fon.stringCount;
	}
}
void gui::text::insertFontString(Font & pFont, String pString)
{
	if (!pFont.stringCount) {//if first textbox of this font
		pFont.stringOffset = allFontStrings.size();//dedicate a new range of tb indices to this font
	}
	++pFont.stringCount;
	allFontStrings.insert(allFontStrings.begin() + pFont.stringOffset, pString);
}
//size_t gui::text::createTextStyle(TextStyle & pStyle)
//{
//	allTextStyles.push_back(pStyle);
//	return allTextStyles.size() - 1;
//}
void gui::text::initStyleBuffer() {

	allTextStyles.reserve(2);
	//createTextStyle(1.5f, 0.8f);
	//createTextStyle(1.2f, 0.8f);
	styleStorage = vao::createStorage(sizeof(TextStyle)*allTextStyles.size(), &allTextStyles[0], 0);

	shader::bindUniformBufferToShader(glyphShaderProgram, styleStorage, "StyleBuffer");
}

void gui::text::updateCharStorage()
{
	if (charQuadBuffer.size()) {
		vao::uploadStorage(quadStorage, sizeof(CharQuad)*charQuadBuffer.size(), &charQuadBuffer[0]);
		vao::uploadStorage(charStorage, sizeof(size_t)*glyphIndexBuffer.size(), &glyphIndexBuffer[0]);
	}
}

void gui::text::clearCharStorage()
{
	for (Font& font : allFonts) {
		font.stringOffset = 0;
		font.stringCount = 0;
	}
	allChars.clear();
	glyphIndexBuffer.clear();
	charQuadBuffer.clear();
	allStrings.clear();
	allFontStrings.clear();
}
