#pragma once
#include <vector>
#include <glm.hpp>

namespace gui
{
	struct LineGroup {
		LineGroup(unsigned int pLineOffset, unsigned int pLineCount)
			:lineOffset(pLineOffset), lineCount(pLineCount){}
		unsigned int lineOffset;
		unsigned int lineCount;
	};

	void toggleLineGroup(unsigned int pLineGroup);
	unsigned int getLineCount();

	const int DEFAULT_FLAG = 1;
	unsigned int createLineGroup(unsigned int pLineOffset, unsigned int pLineCount, int pFlag = DEFAULT_FLAG);
	unsigned int createLine(glm::vec4 pVertexAPos, unsigned int pColorIndexA, glm::vec4 pVertexBPos, unsigned int pColorIndexB);
	unsigned int createLine(glm::vec4 pVertexAPos, glm::vec4 pVertexBPos, unsigned int pColorIndex);
	unsigned int createLine(unsigned int pVertexA, unsigned int pVertexB);
	unsigned int createLineVertex(glm::vec4 pPos, unsigned int pColorIndex);
	unsigned int createLineVertex(unsigned int pPosIndex, unsigned int pColorIndex);
	unsigned int createLineVertexPosition(glm::vec4 pPos);
	void setLineColor(unsigned int pLineIndex, unsigned int pColorIndex);
	void setLineVertexColor(unsigned int pLineIndex, unsigned int pVertex, unsigned int pColorIndex);

	void initLineVAO();
	void updateLinePositions();
	void renderLines();
	void initLineShader();
	void updateLineBuffers();
	void setupLineShader();

}
