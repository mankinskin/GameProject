#pragma once
#include <vector>
#include <string>
#include <glm.hpp>
#include <tuple>
#include "gui.h"
#include "utils.h"
#include "color.h"
#include "storage.h"
#include "vao.h"
#include "quad.h"

namespace gui 
{
	extern gl::VAO colorQuadVAO;
	extern unsigned int colorquadShader;
	extern gl::StreamStorage<unsigned int> colorQuadBuffer;

	void initColorquadShader();
	void setupColorquadShader();

	void renderColorQuads();

	void initColorQuadVAO();
	void updateColorQuads();
	void renderColorQuads();
    void colorQuad( const unsigned int pID, const gl::ColorIt pColor );
}

