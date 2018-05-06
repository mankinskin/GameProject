#pragma once
#include <stddef.h>
#include "texture.h"

namespace texture {

	void initFramebuffers();

	void initGBuffer();
	extern unsigned int gBuffer;
	extern Texture2D gPosTexture;
	extern Texture2D gNormalTexture;
	extern Texture2D gAmbientTexture;
	extern Texture2D gDiffuseTexture;
	extern Texture2D gSpecularTexture;
	extern unsigned int gDepthRenderbuffer;

	void initGUIFBO();
	extern unsigned int guiFBO;
	extern Texture2D quadIndexTexture;
	extern Texture2D fontColorTexture;
	extern unsigned int guiDepthRenderbuffer;
}

