#pragma once
#include "Quad.h"
#include <tuple>
#include "Colorings.h"
#include <glm\glm.hpp>
#include "Widget.h"
#include <initializer_list>
#include "GUI.h"
namespace gui {


	template<size_t N, typename ...Rest>
	struct gen_quad_group {//MAY BE SLOW TO COMPILE

		typedef typename gen_quad_group<N - 1, Quad, Rest...>::type type;
	};
	template<typename ...Rest>
	struct gen_quad_group<0, Rest...> {//MAY BE SLOW TO COMPILE

		typedef Widget<Rest...> type;
	};

	template<size_t QuadCount>
	using QuadGroup = typename gen_quad_group<QuadCount>::type;

	template<size_t QuadCount, size_t InitValueCount>
	struct QuadLayout {
		QuadLayout(std::array<float, InitValueCount> pInitValues):
			initValues{ pInitValues }{}

		std::array<float, InitValueCount> initValues;//values used
		std::array<float, QuadCount * InitValueCount * 4> layoutMatrix;//a matrix for weighting the initValues. 


		template<size_t Q, size_t P, size_t In>
		struct evaluator {
			static float next(std::array<float, InitValueCount>& initvals, std::array<float, QuadCount * InitValueCount * 4>& layout) {
				return initvals[In - 1] * layout[(Q * InitValueCount * 4) + (P * InitValueCount) + In-1] + evaluator<Q, P, In-1>::next(initvals, layout);
			}
		};
		template<size_t Q, size_t P>
		struct evaluator<Q, P, 0> {
			static float next(std::array<float, InitValueCount>& initvals, std::array<float, QuadCount * InitValueCount * 4>& layout) {
				return 0.0f;
			}
		};

		template<size_t Q, size_t P>
		float eval(std::array<float, InitValueCount>& pInitValues) {
			return evaluator<Q, P, InitValueCount>::next(pInitValues, layoutMatrix);
		}
		template<size_t Q>
		Quad create_next(std::array<float, InitValueCount>& pInitValues) {
			return createQuad(pixel_round_x(eval<Q, 0>(pInitValues)), pixel_round_y(eval<Q, 1>(pInitValues)), pixel_round_x(eval<Q, 2>(pInitValues)), pixel_round_y(eval<Q, 3>(pInitValues)));
		}

		//each quad will be initialized by adding all weighted initValues into each createQuad parameter
		template<size_t ...S>
		QuadGroup<QuadCount> create_fun(std::index_sequence<S...> sq, std::array<float, InitValueCount>& pValues) {//creat group
			return QuadGroup<QuadCount>({ create_next<S>(pValues)... });
		}
		QuadGroup<QuadCount> create(std::array<float, InitValueCount> pInitValues) {//creat group
			return create_fun(std::make_index_sequence<QuadCount>(), pInitValues);
		}
		QuadGroup<QuadCount> create() {//creat group
			return create_fun(std::make_index_sequence<QuadCount>(), initValues);
		}
		template<size_t Q>
		void setQuadInitialization(std::array<float, InitValueCount*4> pInitializers) {
			std::memcpy(&layoutMatrix[Q * InitValueCount * 4], &pInitializers[0], sizeof(float) * InitValueCount * 4);
		}
		template<size_t Q, size_t P, size_t I>
		void setInitValueWeight(float pWeight) {
			layoutMatrix[Q * InitValueCount * 4 + (P*InitValueCount) + I] = pWeight;
		}
	};


}

