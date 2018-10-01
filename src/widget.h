#pragma once
#include <array>
#include <tuple>
#include <utility>
#include <vector>
#define GLM_FORCE_PURE
#include <glm.hpp>
#include "color.h"
#include "quad.h"
#include "quadcolors.h"
#include "utils/id.h"
#include "utils/array_utils.h"
#include "utils/tuple_utils.h"
#include "signal.h"
#include "quadsignals.h"
#include "functor.h"
#include "mouse.h"
#include "event.h"


//-----Widgets
// This class is rather complex. It also does a little too many things in one place.
// However it provides a very efficient and stable, yet extremely flexible foundation for a class hierarchy
//
// To create a Widget composed of 2D quads you have to provide some data:
// - the initial data to be loaded into each element(quad) of the widget
// - the policies about how the element should pass an input command to its
// elements

namespace gui
{
  template<typename... Ts>
	struct WidgetSignals
	{
	  using SignalType = signals::SignalListener<signals::And, signals::SignalListener<signals::Or, typename Ts::Signals::SignalType...>, signals::SignalListener<signals::Nor, typename Ts::Signals::SignalType...>>;
	  using HoverType = signals::SignalListener<signals::Flip, SignalType, SignalType>;
	  using HoldType = signals::SignalListener<signals::Flip, signals::SignalListener<signals::And, HoverType, signals::ButtonSignals<input::MouseKey>::SignalType>, signals::ButtonSignals<input::MouseKey>::SignalType>;
	  using PressType = signals::SignalListener<signals::Clicker, typename HoldType::Set, typename HoldType::Reset>;
	  using ReleaseType = signals::SignalListener<signals::Clicker, typename HoldType::Set, typename HoldType::Reset>;

	  template<bool S, size_t... Ns>
		constexpr SignalType gen_event(const std::tuple<Ts...>& elems, const std::index_sequence<Ns...>) const
		{
		  return signals::ifAll(
			  signals::ifAny(std::get<Ns>(elems).event(utils::_bool<S>())...),
			  signals::ifNone(std::get<Ns>(elems).event(utils::_bool<!S>())...));
		}

	  constexpr const SignalType event(utils::_bool<true>) const
	  {
		return enter;
	  }
	  constexpr const SignalType event(utils::_bool<false>) const
	  {
		return leave;
	  }
	  constexpr WidgetSignals(const std::tuple<Ts...>& os)
		: enter(gen_event<true>(os, std::make_index_sequence<sizeof...(Ts)>()))
		  , leave(gen_event<false>(os, std::make_index_sequence<sizeof...(Ts)>()))
		  , hover(flip(enter, leave))
		  , hold(flip(ifAll(hover, input::Mouse::lmb->on), input::Mouse::lmb->off))
		  , press(clicker(hold, true))
		  , release(clicker(hold, false))
	  {}

	  const SignalType enter;
	  const SignalType leave;
	  const HoverType hover;
	  const HoldType hold;
	  const PressType press;
	  const ReleaseType release;
	};


  template<typename W>
	void moveWidget(const W& pW, const glm::vec2& pV)
	{
	  pW.move(pV);
	}
  template<typename W>
	void moveWidgetX(const W& pW, const float& pV)
	{
	  pW.move(glm::vec2(pV, 0.0f));
	}
  template<typename W>
	void moveWidgetY(const W& pW, const float& pV)
	{
	  pW.move(glm::vec2(0.0f, pV));
	}
  template<typename W>
	void setWidgetPos(const W& pW, const glm::vec2& pV)
	{
	  pW.setPos(pV);
	}
  template<typename W>
	void setWidgetPosX(const W& pW, const float& pV)
	{
	  pW->x = pV;
	}
  template<typename W>
	void setWidgetPosY(const W& pW, const float& pV)
	{
	  pW->y = pV;
	}
  template<typename W>
	void resizeWidget(const W& pW, const glm::vec2& pV)
	{
	  pW.resize(pV);
	}
  template<typename W>
	void setWidgetSize(const W& pW, const glm::vec2& pV)
	{
	  pW.setSize(pV);
	}
  template<typename W>
	void resizeWidgetX(const W& pW, const float& pV)
	{
	  pW.resize(glm::vec2(pV, 0.0f));
	}
  template<typename W>
	void resizeWidgetY(const W& pW, const float& pV)
	{
	  pW.resize(glm::vec2(0.0f, pV));
	}

  template<typename... Elems>
	struct WidgetColors : public std::tuple<typename Elems::Colors...>
  {
	using Colors = std::tuple<typename Elems::Colors...>;
	constexpr WidgetColors(const Colors cs)
	  : Colors(cs)
	{}
	const std::tuple<typename Elems::Preset...> subs;
  };

  template<typename... Elems>
	void applyColor_imp_qs(const std::tuple<Elems...>& elems, const WidgetColors<Elems...> cols, utils::_index<0>)
	{}
  template<typename... Elems>
	void applyColor_imp(const std::tuple<Elems...>& elems, const WidgetColors<Elems...> cols)
	{
	  applyColor_imp_n(elems, cols, utils::_index<sizeof...(Elems)>());
	}
  template<typename Elem>
	void applyColor(const Elem& e, const typename Elem::Colors col)
	{
	  applyColor_imp(e, col);
	}
  template<typename... Elems, size_t N>
	void applyColor_imp_n(const std::tuple<Elems...>& elems, const WidgetColors<Elems...> cols, utils::_index<N>)
	{
	  applyColor_imp_n(elems, cols, utils::_index<N-1>());
	  applyColor(std::get<N-1>(elems), std::get<N-1>(cols.colors));
	}

  template<typename Col>
	struct QuadElement : public QuadID, signals::QuadSignals<QuadID>
  {
	static constexpr size_t QUAD_COUNT = 1;
	using Signals = signals::QuadSignals<QuadID>;
	using Colors = Col;
	using Preset = Col;

	QuadElement(const Quad&& q, const Col&& col)
	  : QuadID(QuadID::all.makeID(std::move(q)))
	  , Signals((QuadID)*this)
	  , color(std::move(col))
	{
	  printf("Creating QuadElement\n%lu quads.\n", QuadID::all.size());
	  colorQuad((QuadID)*this, color);
	}

	float x() const
	{
	  return (*this)->x;
	}
	float y() const
	{
	  return (*this)->y;
	}
	float width() const
	{
	  return (*this)->z;
	}
	float height() const
	{
	  return (*this)->w;
	}
	glm::vec2 pos() const
	{
	  return glm::vec2(x(), y());
	}
	glm::vec2 size() const
	{
	  return glm::vec2(width(), height());
	}

	const Col color;
	void move(const glm::vec2 v) const
	{
	  (*this)->move(v);
	}
	void resize(const glm::vec2 v) const
	{
	  (*this)->resize(v);
	}
	void setPos(const glm::vec2 p) const
	{
	  (*this)->setPos(p);
	}
  };
  template<typename Col>
	void applyColor_imp(const QuadElement<Col>& elem, const Col col)
	{
	  colorQuad(elem, col);
	}

  template<typename... Elems>
	struct WidgetLayout
	{
	  using Quads = typename utils::tuple_generator<sizeof...(Elems), glm::vec4>::type;
	  using MovePolicy = std::array<glm::vec2, sizeof...(Elems)>;
	  using ResizePolicy = std::array<glm::vec4, sizeof...(Elems)>;

	  WidgetLayout(const Quads(&layout)(const glm::vec4), const MovePolicy mp, const ResizePolicy rp)
		: genQuads(layout)
		, move(mp)
		, resize(rp)
	  {}
	  const Quads(&genQuads)(const glm::vec4);
	  const MovePolicy move;
	  const ResizePolicy resize;
	};

  template<typename... Elems>
	struct Widget : public std::tuple<Elems...>, WidgetSignals<Elems...>
  {
	static constexpr size_t ELEMENT_COUNT = sizeof...(Elems);
	static constexpr size_t QUAD_COUNT = utils::sum(Elems::QUAD_COUNT...);

	using Colors = WidgetColors<Elems...>;
	using Signals = WidgetSignals<Elems...>;
	using Elements = std::tuple<Elems...>;
	using Quads = typename utils::tuple_generator<sizeof...(Elems), glm::vec4>::type;
	using Layout = WidgetLayout<Elems...>;

	using Signals::hold;
	const Layout layout;

	struct Preset
	{
	  Preset(const Layout l, const std::tuple<typename Elems::Preset...> subs)
		: layout(l)
		, subpresets(subs)
	  {}
	  const Layout layout;
	  const std::tuple<typename Elems::Preset...> subpresets;
	};

	Widget(const glm::vec4& q, const Preset& preset)
	  : Elements(std::move(utils::convert_tuple<Elems...>(preset.layout.genQuads(q), preset.subpresets)))
	  , Signals((Elements)*this)
	  , layout(preset.layout)
	{
	  puts("Creating Widget");
	}

	template<size_t... Ns>
	float x_ns(std::index_sequence<Ns...>) const
	{
	  return utils::min(std::get<Ns>(*this).x()...);
	}
	float x() const
	{
	  return x_ns(std::make_index_sequence<sizeof...(Elems)>());
	}
	template<size_t... Ns>
	float y_ns(std::index_sequence<Ns...>) const
	{
	  return utils::min(std::get<Ns>(*this).y()...);
	}
	float y() const
	{
	  return y_ns(std::make_index_sequence<sizeof...(Elems)>());
	}
	template<size_t... Ns>
	float width_ns(std::index_sequence<Ns...>) const
	{
	  return utils::max(std::get<Ns>(*this).x() + std::get<Ns>(*this).width()...) - x();
	}
	float width() const
	{
	  return width_ns(std::make_index_sequence<sizeof...(Elems)>());
	}
	template<size_t... Ns>
	float height_ns(std::index_sequence<Ns...>) const
	{
	  return utils::max(std::get<Ns>(*this).y() - std::get<Ns>(*this).height()...) - y();
	}
	float height() const
	{
	  return height_ns(std::make_index_sequence<sizeof...(Elems)>());
	}
	glm::vec2 pos() const
	{
	  return glm::vec2(x(), y());
	}
	glm::vec2 size() const
	{
	  return glm::vec2(width(), height());
	}

	void move_n(utils::_index<0> i, const glm::vec2 v) const
	{}

	template<size_t N>
	  void move_n(utils::_index<N> i, const glm::vec2 v) const
	  {
		move_n(utils::_index<N-1>(), v);
		std::get<N-1>(*this).move(v * layout.move[N-1]);
	  }
	void resize_n(utils::_index<0> i, const glm::vec2 v) const
	{}
	template<size_t N>
	  void resize_n(utils::_index<N> i, const glm::vec2 v) const
	  {
		resize_n(utils::_index<N-1>(), v);
		std::get<N-1>(*this).move(v * glm::vec2(layout.resize[N-1].x, layout.resize[N-1].y));
		std::get<N-1>(*this).resize(v * glm::vec2(layout.resize[N-1].z, layout.resize[N-1].w));
	  }
	void move(const glm::vec2 v) const
	{
	  move_n(utils::_index<ELEMENT_COUNT>(), v);
	}
	void resize(const glm::vec2 v) const
	{
	  resize_n(utils::_index<ELEMENT_COUNT>(), v);
	}
	void setPos(const glm::vec2 p)
	{
	  move(p - size());
	}
	void setSize(const glm::vec2 s)
	{
	  resize(s - size());
	}
  };

  template<typename... Elems>
	void applyColor_imp(const Widget<Elems...>& wid, const WidgetColors<Elems...> cols)
	{
	  applyColor_imp_n(wid.elements, cols, utils::_index<sizeof...(Elems)>());
	}
}

