#ifndef QUAD_H
#define QUAD_H
#include <cstring>
#include <glm.hpp>
#include <vector>
#include "event.h"
#include <tuple>
namespace gui {
	typedef glm::vec4 QuadData;
	extern std::vector<glm::vec4> allQuads;
	size_t createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
	size_t createQuad(glm::vec4 pQuad);

	
	struct Quad {
		typedef glm::vec4 initer_t;
		Quad() :index(-1) {}
		Quad(size_t pIndex) :index(pIndex) {}
		Quad(initer_t pIniter) :index(createQuad(pIniter)) {}
		Quad operator=(size_t i) { return Quad(i); }

		
		size_t index;
		//--
		void set_pos(glm::vec2 pos) {
			std::memcpy(&allQuads[index - 1], &pos, sizeof(glm::vec2));
		}
		void move(glm::vec2 dir) {
			allQuads[index-1] += glm::vec4(dir, 0.0f, 0.0f);
		}
		void resize(glm::vec2 dir) {
			allQuads[index-1] += glm::vec4(0.0f, 0.0f, dir.x, -dir.y);
		}
		glm::vec2 get_pos() {
			return glm::vec2(allQuads[index - 1].x, allQuads[index - 1].y);
		}
		glm::vec2 get_size() {
			return glm::vec2(allQuads[index - 1].z, allQuads[index - 1].w);
		}

		template<class ColorType>
		void color(ColorType pColor) {
			colorQuad(index, pColor);
		}
		//--
		template<size_t N>
		size_t element() {
			return index;
		}
		size_t element() {
			return index;
		}

	};

	void moveQuad(size_t pQuad, glm::vec2 pOffset);
	void resizeQuad(size_t pQuad, glm::vec2 pOffset);
	void setQuadPos(size_t pQuad, glm::vec2 pPos);

	template<typename A, typename B>
	void moveQuad(size_t pQuad, A pOffsetX, B pOffsetY) {
		allQuads[pQuad - 1] += glm::vec4(pOffsetX, pOffsetY, 0.0f, 0.0f);
	}
	template<typename A, typename B>
	void setQuadPos(size_t pQuad, A pPosX, B pPosY) {
		allQuads[pQuad - 1].x = pPosX;
		allQuads[pQuad - 1].y = pPosY;
	}

	template<typename A, typename B>
	void resizeQuad(size_t pQuad, A pOffsetX, B pOffsetY) {
		allQuads[pQuad - 1] += glm::vec4(0.0f, 0.0f, pOffsetX, pOffsetY);
	}
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
	extern unsigned int quadIndexShader;
	extern unsigned int quadIndexVAO;
	extern unsigned int quadBuffer;

	const size_t MAX_QUAD_COUNT = 10000;
}
#endif //QUAD_H
