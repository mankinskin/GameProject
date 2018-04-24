#pragma once
#include <stddef.h>

namespace texture {

	void initFramebuffers();

	void initGBuffer();
	extern unsigned int gBuffer;
	extern unsigned int gPosTexture;
	extern unsigned int gNormalTexture;
	extern unsigned int gAmbientTexture;
	extern unsigned int gDiffuseTexture;
	extern unsigned int gSpecularTexture;
	extern unsigned int gDepthRenderbuffer;

	void initGUIFBO();
	extern unsigned int guiFBO;
	extern unsigned int quadIndexTexture;
	extern unsigned int fontColorTexture;
	extern unsigned int guiDepthRenderbuffer;
}

