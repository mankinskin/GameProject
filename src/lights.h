#pragma once 
#include <vector> 
#include <glm\glm.hpp>

namespace lighting {
	struct LightIndexRange {
		LightIndexRange(size_t pOffset, size_t pCount)
			:offset(pOffset), count(pCount) {}
		size_t offset;
		size_t count;
	};

	extern std::vector<glm::vec4> allLightData;
	extern std::vector<LightIndexRange> allLightIndexRanges;
	extern size_t lightVAO;
	extern size_t lightIndexVBO;
	extern size_t lightDataUBO;
	extern size_t lightShaderProgram;
	extern size_t MAX_LIGHT_COUNT;

	void initLighting();
	void initLightVAO();
	void initLightDataBuffer();
	void updateLightDataBuffer();
	void updateLightIndexRangeBuffer();
	void initLightShader();
	void renderLights();
	void setupLightShader();
	size_t createLight(glm::vec4& pPos, glm::vec4& pColor);
	size_t createLight(glm::vec4& pPos, glm::vec4& pColor, glm::vec4& pFrustum);
	void setLightPos(size_t pLightIndex, glm::vec3& pPos);
	void setLightPos(size_t pLightIndex, glm::vec4& pPos);
	void setLightColor(size_t pLightIndex, glm::vec3& pColor);
	void setLightColor(size_t pLightIndex, glm::vec4& pColor);
	
	glm::vec4 & getLightColor(size_t pLightIndex);

	void reserveLightSpace(size_t pCount);//reserves 3 vec4s and one index range for count
	void reservePointLightSpace(size_t pCount);//reserves only 2 vec4 (and one index range) per light
}

