#pragma once
#include "Element.h"
#include "Quad.h"
#include <tuple>
#include <utility>
#include "Colorings.h"

//-----Widgets
//- Widgets are used to control collections of 2D Quads in an object based manner
namespace gui {

	//WidgetElements stores a tuple of their tamplate parameters Elements... and can construct them from Initializers
	template<typename ...Elements>
	struct WidgetElements {

		static std::vector<std::tuple<Elements...>> all_elements;
		size_t elements_index;

		std::tuple<Elements...>& elements() {
			return all_elements[elements_index];
		}
		template<class ...Initializers>
		struct construct_each_of {

			static std::tuple<Elements...> func(std::tuple<Initializers...> pInits) {
				return extract(std::make_index_sequence<sizeof...(Initializers)>(), pInits);
			}
			template<size_t... S>
			static std::tuple<Elements...> extract(std::index_sequence<S...> pInds, std::tuple<Initializers...> pInits) {
				return std::tuple<Elements...>{ { std::tuple_element<S, std::tuple<Elements...>>::type(std::get<S>(pInits)) }... };
			}
		};

		WidgetElements(size_t pElementsIndex)
			:elements_index(pElementsIndex) {	}

		template<class ...Initializers>
		WidgetElements(std::tuple<Initializers...> pInits)
			: elements_index(all_elements.size())
		{
			all_elements.push_back(construct_each_of<Initializers...>::func(pInits));
		}

		WidgetElements(Elements... pElements)
			:elements_index(all_elements.size())
		{
			all_elements.push_back({ pElements }...);
		}
		static const size_t ELEMENT_COUNT = sizeof...(Elements);
		template<size_t N>
		auto element() {
			return std::get<N>(elements());
		}
	};
	template<typename ...Elements>
	std::vector<std::tuple<Elements...>> gui::WidgetElements<Elements...>::all_elements;

	//Widget Behaviour
	//
	template<class ...Elements>
	struct widget_pos : public WidgetElements<Elements...> {
		widget_pos(WidgetElements<Elements...> pWidgetElements) :WidgetElements<Elements...>(pWidgetElements) {}
		glm::vec2 pos = glm::vec2();

		void set(glm::vec2 pPos) {
			all<sizeof...(Elements)>(pPos - pos);
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

	template<class ...Elements>
	struct widget_move : public WidgetElements<Elements...> {
		widget_move(WidgetElements<Elements...> pWidgetElements) :WidgetElements<Elements...>(pWidgetElements) {}

		void operator()(glm::vec2 pOffset) {
			all<sizeof...(Elements)>(pOffset);
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
	template<class ...Elements>
	struct widget_stretch : public WidgetElements<Elements...> {
		widget_stretch(WidgetElements<Elements...> pWidgetElements) :WidgetElements<Elements...>(pWidgetElements) {}


		void operator()(glm::vec2 pDir) {
			all<sizeof...(Elements)>(pDir);
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
	template<>
	struct widget_stretch<Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad> : public WidgetElements<Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad> {
		widget_stretch(WidgetElements<Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad> pWidgetElements) :WidgetElements<Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad>(pWidgetElements) {}

		void operator()(glm::vec2 pDir) {
			all<9>(pDir);
		}

		template<size_t N>
		void all(glm::vec2& pDir) {
			all<N - 1>(pDir);
			element_special<N - 1>(pDir);
		}
		template<>
		void all<0>(glm::vec2& pDir) {}

		template<size_t N>
		void element_special(glm::vec2& pDir) {
			if (N % 3 == 0) {//left side quads
				if (N < 3) {//top left

				}
				else if (N > 2 && N < 6) {//left middle
					std::get<N>(elements()).stretch(glm::vec2(0.0f, pDir.y));
				}
				else {//bottom left
					std::get<N>(elements()).move(glm::vec2(0.0f, pDir.y));
				}
			}
			else if ((N + 2) % 3 == 0) {//x middle quads
				if (N < 3) {//top middle
					std::get<N>(elements()).stretch(glm::vec2(pDir.x, 0.0f));
				}
				else if (N > 2 && N < 6) {//middle middle
					std::get<N>(elements()).stretch(pDir);
				}
				else {//bottom middle
					std::get<N>(elements()).move(glm::vec2(0.0f, pDir.y));
					std::get<N>(elements()).stretch(glm::vec2(pDir.x, 0.0f));
				}
			}
			else {//x right quads
				if (N < 3) {//top right
					std::get<N>(elements()).move(glm::vec2(pDir.x, 0.0f));
				}
				else if (N > 2 && N < 6) {//right middle
					std::get<N>(elements()).move(glm::vec2(pDir.x, 0.0f));
					std::get<N>(elements()).stretch(glm::vec2(0.0f, pDir.y));
				}
				else {//bottom right
					std::get<N>(elements()).move(pDir);
				}
			}

		}
		template<size_t N>
		void element(glm::vec2& pDir) {
			std::get<N>(elements()).stretch(pDir);
		}
	};
	template<class... Elements>
	struct widget_color : public WidgetElements<Elements...> {

		widget_color(WidgetElements<Elements...> pWidgetElements) :WidgetElements<Elements...>(pWidgetElements) {}

		template<class... Colors>
		struct col {
			template<size_t I>
			struct color_elements {
				static void func(std::tuple<Elements...>& pElems, WidgetColors<Colors...>& pColors) {
					color_elements<I - 1>::func(pElems, pColors);
					std::get<I - 1>(pElems).color(std::get<I - 1>(pColors.colors));
				}
			};
			template<>
			struct color_elements<0> {
				static void func(std::tuple<Elements...>& pElems, WidgetColors<Colors...>& pColors) {

				}
			};

		};
		template<class... Colors>
		void operator()(WidgetColors<Colors...> pColors) {
			static_assert(sizeof...(Colors) == sizeof...(Elements), "Color count does not match the element count of Widget");
			col<Colors...>::color_elements<sizeof...(Elements)>::func(elements(), pColors);
		}
	};
	template<class ...Elements>
	class Widget : public WidgetElements<Elements...> {
	public:
		Widget() {}

		template<class ...Initializers>
		Widget(std::tuple<Initializers...> pInits)
			:stretch(elements_index), move(elements_index), pos(elements_index), color(elements_index), WidgetElements<Elements...>(pInits)
		{	}

		Widget(Elements... pElements)
			:stretch(elements_index), move(elements_index), pos(elements_index), color(elements_index), WidgetElements<Elements...>(pElements...)
		{	}

		widget_pos<Elements...> pos;
		widget_move<Elements...> move;
		widget_stretch<Elements...> stretch;
		widget_color<Elements...> color;
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

gui::initSimple<gui::Widget<gui::Quad>>::operator gui::Widget<gui::Quad>() const {
	return Widget<Quad>(createQuad(posX, posY, width, height*aspect_ratio));
}
gui::initMargin<gui::Widget<gui::Quad, gui::Quad>>::operator gui::Widget<gui::Quad, gui::Quad>() const {
	return Widget<Quad, Quad>(
		std::tuple<Quad, Quad>({ createQuad(posX, posY, width, height*aspect_ratio),
		createQuad(posX + margin, posY - margin * aspect_ratio, width - margin * 2.0f, (height - margin * 2.0f)*aspect_ratio) }));
}
gui::initMargin<gui::Widget<gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad>>::operator gui::Widget<gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad, gui::Quad>() const {
	return Widget<Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad>(
		std::tuple<Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad>({
		createQuad(posX, posY, margin, margin*aspect_ratio),
		createQuad(posX + margin, posY, width - margin * 2.0f, margin*aspect_ratio),
		createQuad(posX + width - margin, posY, margin, margin*aspect_ratio),
		createQuad(posX, posY - margin * aspect_ratio, margin, height - margin * 2.0f*aspect_ratio),
		createQuad(posX + margin, posY - margin * aspect_ratio, width - margin * 2.0f, height - margin * 2.0f*aspect_ratio),
		createQuad(posX + width - margin, posY - margin * aspect_ratio, margin, height - margin * 2.0f*aspect_ratio),
		createQuad(posX, posY - (height - margin * aspect_ratio), margin, margin*aspect_ratio),
		createQuad(posX + margin, posY - (height - margin * aspect_ratio), width - margin * 2.0f, margin*aspect_ratio),
		createQuad(posX + width - margin, posY - (height - margin * aspect_ratio), margin, margin*aspect_ratio)
			}));
}

//template<typename... Peers>
//struct Cor {
//
//
//	static std::vector<std::tuple<Peers...>> all_peers;
//	size_t peers_index;
//
//	Cor(Peers... peers):peers_index(all_peers.size())
//	{
//		all_peers.push_back({ peers... });
//	}
//	Cor(size_t ind) :peers_index(ind)
//	{	}
//	std::tuple<Peers...>& getPeers() {
//		return all_peers[peers_index];
//	}
//};
//template<typename... Peers>
//struct Par : public Cor<Peers...>{
//	Par
//	template<class... Args>
//	struct pit {
//		template<size_t I>
//		static void pok(std::tuple<Peers...>& peers, std::tuple<Args...>& args) {
//			pok<I - 1>(peers, args);
//			std::get<I - 1>(peers).take(std::get<I - 1>(args));
//		}
//		template<>
//		static void pok<0>(std::tuple<Peers...>& peers, std::tuple<Args...>& args) {
//
//		}
//	};
//	template<typename... Args>
//	void pitch(Args... args) {
//		pit<Args...>::pok<sizeof...(Peers)>(getPeers(), { args... });
//	}
//};