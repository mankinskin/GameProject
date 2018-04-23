#pragma once

namespace texture {

	void initFramebuffers();

	void initGBuffer();
	extern size_t gBuffer;
	extern size_t gPosTexture;
	extern size_t gNormalTexture;
	extern size_t gAmbientTexture;
	extern size_t gDiffuseTexture;
	extern size_t gSpecularTexture;
	extern size_t gDepthRenderbuffer;

	void initGUIFBO();
	extern unsigned int guiFBO;
	extern unsigned int quadIndexTexture;
	extern unsigned int fontColorTexture;
	extern unsigned int guiDepthRenderbuffer;
}
