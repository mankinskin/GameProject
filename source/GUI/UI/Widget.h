#pragma once
#include "Quad.h"
#include <tuple>
#include <utility>
#include "Colorings.h"
#include <glm\glm.hpp>
//-----Widgets
//- Widgets are used to control collections of 2D Quads in an object based manner
namespace gui {


	template<typename Widget>
	struct WidgetMovePolicy {
		WidgetMovePolicy(std::array<glm::vec2, Widget::ELEMENT_COUNT> pPolicyMatrix)
			:matrix(pPolicyMatrix){}
		std::array<glm::vec2, Widget::ELEMENT_COUNT> matrix;
	};

	template<typename Widget>
	struct WidgetResizePolicy {
		WidgetResizePolicy(std::array<glm::vec4, Widget::ELEMENT_COUNT> pPolicyMatrix)
			:matrix(pPolicyMatrix) {}
		std::array<glm::vec4, Widget::ELEMENT_COUNT> matrix;
	};
	template<typename Widget>
	struct WidgetLayout {
		typedef typename Widget::initer_t initer_t;
		WidgetLayout(initer_t pIniter) 
			:initer(pIniter) {}
		initer_t initer;
	};


	template<typename Widget>
	struct WidgetSetup {
		WidgetSetup(WidgetLayout<Widget> pLayout, WidgetMovePolicy<Widget> pMovePolicy, WidgetResizePolicy<Widget> pResizePolicy)
			:move_policy(pMovePolicy), resize_policy(pResizePolicy), layout(pLayout) {}

		WidgetLayout<Widget> layout;
		WidgetMovePolicy<Widget> move_policy;
		WidgetResizePolicy<Widget> resize_policy;
		operator Widget() {
			return Widget(layout, move_policy, resize_policy);
		}
	};

	template<class... Colors>
	struct WidgetColors {
		WidgetColors(Colors... pColors)
			:colors(pColors...) {}
		std::tuple<Colors...> colors;
	};



	template<class ...Elems>
	class Widget {
	public:
		static const size_t ELEMENT_COUNT = sizeof...(Elems);
		template<size_t N, typename ...Initers>
		struct extract_initers {
			typedef typename std::tuple_element_t<N - 1, std::tuple<Elems...>>::initer_t element_initializer_t;
			typedef typename extract_initers<N - 1, element_initializer_t, Initers...>::result result;
		};
		template<typename ...Rest>
		struct extract_initers<0, Rest...> {
			typedef typename std::tuple<Rest...> result;
		};
		typedef typename extract_initers<ELEMENT_COUNT>::result initer_t;
		template<size_t N>
		struct construct_all {

			static std::tuple<Elems...> func(WidgetLayout<Widget<Elems...>> pLayout) {
				std::tuple<Elems...> re = construct_all<N - 1>::func(pLayout);
				std::get<N - 1>(re) = std::get<N - 1>(pLayout.initer);
				return re;
			}

		};
		template<>
		struct construct_all<0> {
			static std::tuple<Elems...> func(WidgetLayout<Widget<Elems...>> pLayout) {
				return std::tuple<Elems...>();
			}
		};
		Widget() {	}
		Widget(Elems... pElements)
			:elements({ pElements... })
		{	}
		Widget(std::tuple<Elems...> pElements)
			:elements(pElements)
		{	}
		Widget(initer_t pIniter)
			:elements(construct_all<ELEMENT_COUNT>::func(pIniter))
		{	}
		Widget(WidgetLayout<Widget<Elems...>> pLayout, WidgetMovePolicy<Widget<Elems...>> pMovePolicy, WidgetResizePolicy<Widget<Elems...>> pResizePolicy)
			:move_policy(pMovePolicy), resize_policy(pResizePolicy), elements(construct_all<ELEMENT_COUNT>::func(pLayout)) {}
		template<size_t N>
		auto element() {
			return std::get<N>(elements);
		}

		void set_pos(glm::vec2 pNewPos) {
			set_all_pos<ELEMENT_COUNT>(pNewPos);
			pos = pNewPos;
		}
		template<size_t N>
		void set_element_pos(glm::vec2 pNewPos) {
			std::get<N>(elements).set_pos(pNewPos);
		}

		void move(glm::vec2& pOffset) {
			move_all<ELEMENT_COUNT>(pOffset);
			pos += pOffset;
		}
		template<size_t N>
		void move_element(glm::vec2& pOffset) {
			std::get<N>(elements).move(pOffset * move_policy.matrix[N]);
		}

		void move_to(glm::vec2 pNewPos) {
			move_all<ELEMENT_COUNT>((pNewPos - pos) * move_policy.matrix[N]);
			pos = pNewPos;
		}
		template<size_t N>
		void move_element_to(glm::vec2& pNewPos) {
			std::get<N>(elements).move((pNewPos - pos) * move_policy.matrix[N]);
		}

		void resize(glm::vec2& pOffset) {
			resize_all<ELEMENT_COUNT>(pOffset);
		}
		template<size_t N>
		void resize_element(glm::vec2& pOffset) {
			std::get<N>(elements).resize(pOffset * glm::vec2(resize_policy.matrix[N].z, resize_policy.matrix[N].w));
		}

		template<class... Colors>
		void color(WidgetColors<Colors...> pColors) {
			static_assert(sizeof...(Colors) == sizeof...(Elems), "Color count does not match the element count of Widget");
			col<Colors...>::color_elements<sizeof...(Elems)>::func(elements, pColors);
		}
		
	private:

		std::tuple<Elems...> elements;
		glm::vec2 pos;
		WidgetMovePolicy<Widget<Elems...>> move_policy;
		WidgetResizePolicy<Widget<Elems...>> resize_policy;


		template<size_t N>
		void move_all(glm::vec2& pOffset) {//move all elements
			move_all<N - 1>(pOffset);
			move_element<N - 1>(pOffset);
		}
		template<>
		void move_all<0>(glm::vec2& pOffset) {}

		template<size_t N>
		void set_all_pos(glm::vec2& pNewPos) {//move all elements
			set_all_pos<N - 1>(pNewPos);
			set_element_pos<N - 1>(pNewPos);
		}
		template<>
		void set_all_pos<0>(glm::vec2& pNewPos) {}

		template<size_t N>
		void resize_all(glm::vec2& pOffset) {//move all elements
			resize_all<N - 1>(pOffset);

			move_element<N - 1>(pOffset * glm::vec2(resize_policy.matrix[N - 1].x, resize_policy.matrix[N - 1].y));
			resize_element<N - 1>(pOffset);
		}
		template<>
		void resize_all<0>(glm::vec2& pOffset) {}

		template<class... Colors>
		struct col {
			template<size_t I>
			struct color_elements {
				static void func(std::tuple<Elems...>& pElems, WidgetColors<Colors...>& pColors) {
					color_elements<I - 1>::func(pElems, pColors);
					std::get<I - 1>(pElems).color(std::get<I - 1>(pColors.colors));
				}
			};
			template<>
			struct color_elements<0> {
				static void func(std::tuple<Elems...>& pElems, WidgetColors<Colors...>& pColors) {				}
			};

		};


	};

	template<class Widget>
	void move_widget(Widget pWidget, glm::vec2& pOffset) {
		pWidget.move(pOffset);
	}
	template<class Widget>
	void move_widget(Widget pWidget, float pOffsetX, float pOffsetY) {
		pWidget.move(glm::vec2(pOffsetX, pOffsetY));
	}
	template<class Widget, size_t N>
	void move_widget_element(Widget pWidget, float pOffsetX, float pOffsetY) {
		pWidget.move.element<N>(glm::vec2(pOffsetX, pOffsetY));
	}
	template<class Widget>
	void resize_widget(Widget pWidget, glm::vec2& pOffset) {
		pWidget.resize(pOffset);
	}
	template<class Widget>
	void resize_widget(Widget pWidget, float pDirX, float pDirY) {
		pWidget.resize(glm::vec2(pDirX, pDirY));
	}

	//generate Widget with N Quads 
	template<size_t N, typename ...Quads>
	struct gen_quad_group {//MAY BE SLOW TO COMPILE

		typedef typename gen_quad_group<N - 1, Quad, Quads...>::type type;
	};
	template<typename ...Quads>
	struct gen_quad_group<0, Quads...> {//MAY BE SLOW TO COMPILE

		typedef typename Widget<Quads...> type;
	};

	template<size_t QuadCount>
	using QuadGroup = typename gen_quad_group<QuadCount>::type;


	//generate Widget with N Quads 
	template<size_t N, typename Color, typename ...Colors>
	struct gen_color_group {

		typedef typename gen_color_group<N - 1, Color, Color, Colors...>::type type;
	};
	template<typename Color, typename ...Colors>
	struct gen_color_group<0, Color, Colors...> {

		typedef typename WidgetColors<Colors...> type;
	};

	template<size_t ColorCount, typename Color = ConstColor>
	using ColorGroup = typename gen_color_group<ColorCount, Color>::type;

}









