#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <tuple>
#include "Quad.h"
namespace gui {

	template<class... Colors>
	struct WidgetColors {
		WidgetColors(Colors... pColors)
			:colors(pColors...) {}
		std::tuple<Colors...> colors;
	};

	struct ConstColor {
		ConstColor(std::string pColorName);
		ConstColor(size_t pIndex)
			:color_index(pIndex){}
		ConstColor():color_index(0){}
		size_t color_index;
	};
	
	const size_t MAX_CONST_COLOR_COUNT = 100;

	extern size_t constColorBuffer;
	extern size_t constColorVAO;
	extern std::vector<glm::vec4> constColors;
	template<class ColorType>
	extern ColorType colorings[MAX_QUAD_COUNT];//one index into constColors for each quad
	extern size_t constColorShader;
	extern size_t constColoringBuffer;

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
	template<class ColorType>
	void colorQuad(size_t pQuad, ColorType pColor);

}

template<class ColorType>
void gui::colorQuad(size_t pQuadID, ColorType pColor) {

	colorings<ColorType>[pQuadID-1] = pColor;
}