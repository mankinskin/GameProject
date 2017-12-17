#pragma once
#include <vector>
#include <glm\glm.hpp>
namespace gui {
	void setLineColor(size_t pLineIndex, size_t pColorIndex);
	void initLineVAO();
	void renderLines();
	void initLineShader();
	void updateLines();
	void setupLineShader();
}
