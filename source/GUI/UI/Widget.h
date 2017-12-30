#pragma once
#include "Quad.h"
#include <tuple>
#include <utility>
#include "Colorings.h"
//-----Widgets
//- Widgets are used to control collections of 2D Quads in an object based manner
namespace gui {

	//WidgetElements stores a tuple of their tamplate parameters Elements... and can construct them from Initializers
	template<typename ...Elems>
	struct Elements {
		static std::vector<std::tuple<Elems...>> all_elements;
		size_t elements_index;
		static const size_t ELEMENT_COUNT = sizeof...(Elems);


		Elements(size_t pElementsIndex)
			:elements_index(pElementsIndex) {	}

		Elements(Elems... pElements)
			:elements_index(all_elements.size())
		{
			all_elements.push_back({ pElements... });
		}

		std::tuple<Elems...>& elements() {
			return all_elements[elements_index];
		}
		template<size_t N>
		auto element() {
			return std::get<N>(elements());
		}
	};
	template<typename ...Elems>
	std::vector<std::tuple<Elems...>> gui::Elements<Elems...>::all_elements;

	//Widget Behaviour
	//
	template<class ...Elems>
	struct widget_pos : public Elements<Elems...> {
		widget_pos(Elements<Elems...> pElements) :Elements<Elems...>(pElements) {}
		glm::vec2 pos = glm::vec2();

		void set(glm::vec2 pPos) {
			all<sizeof...(Elems)>(pPos - pos);
			pos = pPos;
		}
		glm::vec2& get() {
			return pos;
		}
		template<size_t N>
		void all(glm::vec2&& pDir) {
			all<N - 1>(std::move(pDir));
			element<N - 1>(pDir);
		}
		template<>
		void all<0>(glm::vec2&& pDir) {}

		template<size_t N>
		void element(glm::vec2 pDir) {
			std::get<N>(elements()).move(pDir - std::get<N>(elements()).pos.get());
		}
	};

	template<class ...Elems>
	struct widget_move : public Elements<Elems...> {
		widget_move(Elements<Elems...> pElements) :Elements<Elems...>(pElements) {}

		void operator()(glm::vec2 pOffset) {
			all<sizeof...(Elems)>(pOffset);
		}
		template<size_t N>
		void all(glm::vec2& pOffset) {
			all<N - 1>(pOffset);
			element<N - 1>(pOffset);
		}
		template<>
		void all<0>(glm::vec2& pOffset) {}
		template<size_t N>
		void element(glm::vec2& pOffset) {
			std::get<N>(elements()).move(pOffset);
		}
	};
	template<class ...Elems>
	struct widget_stretch : public Elements<Elems...> {
		widget_stretch(Elements<Elems...> pElements) :Elements<Elems...>(pElements) {}


		void operator()(glm::vec2 pDir) {
			all<sizeof...(Elems)>(pDir);
		}

		template<size_t N>
		void all(glm::vec2& pDir) {
			all<N - 1>(pDir);
			element<N - 1>(pDir);
		}
		template<>
		void all<0>(glm::vec2& pDir) {}


		template<size_t N>
		void element(glm::vec2& pDir) {
			std::get<N>(elements()).stretch(pDir);
		}
	};
	template<class... Elems>
	struct widget_color : public Elements<Elems...> {

		widget_color(Elements<Elems...> pElements) :Elements<Elems...>(pElements) {}

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
				static void func(std::tuple<Elems...>& pElems, WidgetColors<Colors...>& pColors) {

				}
			};

		};
		template<class... Colors>
		void operator()(WidgetColors<Colors...> pColors) {
			static_assert(sizeof...(Colors) == sizeof...(Elems), "Color count does not match the element count of Widget");
			col<Colors...>::color_elements<sizeof...(Elems)>::func(elements(), pColors);
		}
	};
	template<class ...Elems>
	class Widget : public Elements<Elems...> {
	public:
		Widget(Elems... pElements)
			:stretch(elements_index), move(elements_index), pos(elements_index), color(elements_index), Elements<Elems...>(pElements...)
		{	}

		widget_pos<Elems...> pos;
		widget_move<Elems...> move;
		widget_stretch<Elems...> stretch;
		widget_color<Elems...> color;
	};

	template<class Widget>
	void move_widget(Widget pWidget, glm::vec2& pOffset) {
		pWidget.move(pOffset);
	}
	template<class Widget, typename X, typename Y>
	void move_widget(Widget pWidget, X pOffsetX, Y pOffsetY) {
		pWidget.move(glm::vec2(pOffsetX, pOffsetY));
	}
	template<class Widget, size_t N, typename X, typename Y>
	void move_widget_element(Widget pWidget, X pOffsetX, Y pOffsetY) {
		pWidget.move.element<N>(glm::vec2(pOffsetX, pOffsetY));
	}
	template<class Widget>
	void stretch_widget(Widget pWidget, glm::vec2& pDir) {
		pWidget.stretch(pDir);
	}
	template<class Widget, typename X, typename Y>
	void stretch_widget(Widget pWidget, X pDirX, Y pDirY) {
		pWidget.stretch(glm::vec2(pDirX, pDirY));
	}
	template<class Widget, size_t N, typename X, typename Y>
	void stretch_widget_element(Widget pWidget, X pDirX, Y pDirY) {
		pWidget.stretch.element<N>(glm::vec2(pDirX, pDirY));
	}
	template<class Widget, size_t N, typename X, typename Y>
	void set_widget_element_pos(Widget pWidget, X pDirX, Y pDirY) {
		pWidget.set_pos.element<N>(glm::vec2(pDirX, pDirY));
	}


}

