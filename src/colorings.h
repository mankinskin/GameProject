#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include <tuple>
#include "quad.h"

namespace gui {

	struct ConstColor {
		size_t color_index;

		ConstColor(std::string pColorName);
		ConstColor(size_t pIndex)
			:color_index(pIndex){}
		ConstColor():color_index(0){}
	};
	
	const size_t MAX_CONST_COLOR_COUNT = 100;

	extern unsigned int constColorBuffer;
	extern unsigned int constColorVAO;
	extern unsigned int constColorShader;
	extern unsigned int constColoringBuffer;

	extern std::vector<glm::vec4> constColors;  //all registered colors

	template<class ColorType>
	ColorType colorings[MAX_QUAD_COUNT]; //one index into constColors for each quad


	void initConstColorVAO();
	void initConstColorShader();
	void setupConstColorShader();
	void updateConstColors();
	void renderConstColors();
	void uploadConstColors();


	void initColoringVAOs();
	void initColoringShaders();
	void setupColoringShaders();
	void updateColorings();
	void renderColorings();

	void uploadUVRanges();
	size_t createAtlasUVRange(glm::vec4 pUVRange);

	ConstColor createConstColor(glm::vec4 pColor, std::string pColorName);
	size_t getConstColor(std::string pColorName);
	template<class ColorType>
	void colorQuad(size_t pQuad, ColorType pColor);
}

template<class ColorType>
void gui::colorQuad(size_t pQuadID, ColorType pColor) {

	colorings<ColorType>[pQuadID-1] = pColor;
}
