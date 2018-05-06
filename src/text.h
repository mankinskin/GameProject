#pragma once
#include "font.h"
#include "texture.h"

namespace text
{
	void loadFonts();

	void initFontVAO();
	
	void initFontShader();

	void renderFont();
}
