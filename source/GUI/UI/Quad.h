#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../../Input/Event.h"
#include <tuple>
namespace gui {
	typedef typename glm::vec4 QuadData;
	extern std::vector<glm::vec4> allQuads;
	size_t createQuad(float pPosX, float pPosY, float pWidth, float pHeight);
	size_t createQuad(glm::vec4 pQuad);

	struct Quad {
		typedef typename QuadData initer_t;
		Quad() :index(-1) {}
		Quad(size_t pIndex) :index(pIndex) {}
		Quad(initer_t pIniter) :index(createQuad(pIniter)) {}
		Quad operator=(size_t i) { return Quad(i); }

		
		size_t index;
		//--
		void move(glm::vec2 dir) {
			allQuads[index-1] += glm::vec4(dir, 0.0f, 0.0f);
		}
		void resize(glm::vec2 dir) {
			allQuads[index-1] += glm::vec4(0.0f, 0.0f, dir.x, -dir.y);
		}
		glm::vec2 get_pos() {
			return glm::vec2(allQuads[index - 1].x, allQuads[index - 1].y);
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


	namespace experimental{
		//template<typename ...Elements>
		//struct Widget {
		//	Widget() {}
		//	typedef typename std::tuple<Elements::initer_t...> initer_t;//recursive construction of initializer type
		//
		//	Widget(initer_t pIniter) :elements(pIniter) {}
		//	template<size_t N>
		//	size_t element() {
		//		return std::get<N>(elements);
		//	}
		//	std::tuple<Elements...> elements;
		//};
		//
		//template<>//actually a Quad
		//struct Widget<float, float, float, float> {
		//
		//	typedef typename std::tuple<float, float, float, float> initer_t;
		//	Widget() {}
		//	Widget(float pX, float pY, float pWidth, float pHeight) :index(createQuad(pX, pY, pWidth, pHeight)) {}
		//	Widget(std::tuple<float, float, float, float> pIniter) :index(createQuad(pIniter)) {}
		//
		//
		//	size_t index;
		//
		//	template<size_t N>
		//	size_t element() {
		//		return index;
		//	}
		//	size_t element() {
		//		return index;
		//	}
		//};
	}
}
