#pragma once
#include <glm/glm.hpp>
#include <vector>
namespace gui {

	extern std::vector<glm::vec4> allQuads;

	struct quad_pos {
		quad_pos(size_t pIndex) :quad(pIndex) {}
		glm::vec2 pos = glm::vec2();
		size_t quad;

		void set(glm::vec2 pPos) {
			std::memcpy(&allQuads[quad - 1], &pPos, sizeof(glm::vec2));
		}
		glm::vec2 get() {
			return glm::vec2(allQuads[quad - 1]);
		}
	};
	struct Quad {
		Quad() :pos(index), index(-1) {}
		Quad(size_t pIndex) :pos(index), index(pIndex) {}
		
		Quad operator=(size_t i) { return Quad(i); }

		size_t index;
		void move(glm::vec2 dir) {
			allQuads[index-1] += glm::vec4(dir, 0.0f, 0.0f);
		}
		void stretch(glm::vec2 dir) {
			allQuads[index-1] += glm::vec4(0.0f, 0.0f, dir.x, -dir.y);
		}
		quad_pos pos;
		template<size_t N>
		size_t get() {
			return index;
		}
		size_t get() {
			return index;
		}
		template<class ColorType>
		void color(ColorType pColor) {
			colorQuad(index, pColor);
		}
	};
	Quad createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
	Quad createQuad(glm::vec4 pQuad);

	void rasterQuadIndices();


	void initQuadIndexShader();
	void setupQuadIndexShader();
	void reserveQuads(size_t pCount);
	void clearQuads();
	size_t readQuadIndexMap(size_t pPos);
	size_t readQuadIndexMap(size_t pXPos, size_t pYPos);
	float readQuadDepthMap(size_t pPos);
	float readQuadDepthMap(size_t pXPos, size_t pYPos);
	void initQuadBuffer();
	void updateQuadBuffer();
	void readQuadIndexBuffer();
	extern size_t quadIndexShader;
	extern size_t quadIndexVAO;

	const size_t MAX_QUAD_COUNT = 10000;
	extern size_t quadBuffer;
}
