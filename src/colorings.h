#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include <tuple>

namespace gui {

	const unsigned int MAX_QUAD_COUNT = 1000;
	extern unsigned int constColorVAO;
	extern unsigned int constColorShader;
	extern unsigned int constColoringBuffer;

	template<class ColorType>
	ColorType colorings[MAX_QUAD_COUNT];
    //one index into constColors for each quad

	void initConstColorVAO();
	void initConstColorShader();
	void setupConstColorShader();
	void updateConstColors();
	void renderConstColors();

	void initColoringVAOs();
	void initColoringShaders();
	void setupColoringShaders();
	void updateColorings();
	void renderColorings();

	void uploadUVRanges();
	unsigned int createAtlasUVRange(glm::vec4 pUVRange);


}

