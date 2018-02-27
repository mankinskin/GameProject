#pragma once
#include <vector>
#include <glm.hpp>

namespace gui
{
	struct LineGroup {
		LineGroup(size_t pLineOffset, size_t pLineCount)
			:lineOffset(pLineOffset), lineCount(pLineCount){}
		size_t lineOffset;
		size_t lineCount;
	};
	const int DEFAULT_FLAG = 1;
	void toggleLineGroup(size_t pLineGroup);
	size_t getLineCount();
	size_t createLineGroup(size_t pLineOffset, size_t pLineCount, int pFlag = DEFAULT_FLAG);
	size_t createLine(glm::vec4 pVertexAPos, size_t pColorIndexA, glm::vec4 pVertexBPos, size_t pColorIndexB);
	size_t createLine(glm::vec4 pVertexAPos, glm::vec4 pVertexBPos, size_t pColorIndex);
	size_t createLine(size_t pVertexA, size_t pVertexB);
	size_t createLineVertex(glm::vec4 pPos, size_t pColorIndex);
	size_t createLineVertex(size_t pPosIndex, size_t pColorIndex);
	size_t createLineVertexPosition(glm::vec4 pPos);
	void setLineColor(size_t pLineIndex, size_t pColorIndex);
	void setLineVertexColor(size_t pLineIndex, size_t pVertex, size_t pColorIndex);
	void initLineVAO();
	void updateLinePositions();
	void renderLines();
	void initLineShader();
	void updateLineBuffers();
	void setupLineShader();
}
