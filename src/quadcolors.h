#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include <tuple>
#include "gui.h"
#include "utils.h"
#include "color.h"

namespace gui {

	extern unsigned int colorQuadVAO;
	extern unsigned int colorQuadShader;
	extern unsigned int colorQuadBuffer;

	extern unsigned int quadColors[MAX_QUAD_COUNT];

	void initColorQuadShader();
	void setupColorQuadShader();

	void renderColorQuads();

	void initColorQuadVAO();
	void updateColorQuads();
	void renderColorQuads();
}

