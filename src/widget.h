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
	void setWidgetPos(const W& pW, const glm::vec2& pV)
	{
	  pW.setPos(pV);
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

  //template<typename... Elems>
  //    struct WidgetLayout
  //    {
  //        using Quads = typename utils::tuple_generator<sizeof...(Elems), glm::vec4>::type;

  //        struct Preset
  //        {
  //            constexpr Preset(const MovePolicy m, const ResizePolicy r)
  //                : movepolicy(m)
  //                , resizepolicy(r)
  //            {}
  //            constexpr Preset(const MovePolicy m, const ResizePolicy r)
  //                : movepolicy(m)
  //                , resizepolicy(r)
  //            {}
  //            MovePolicy movepolicy;
  //            ResizePolicy resizepolicy;
  //        };

  //        constexpr WidgetLayout(Quads qs, Preset pre)
  //            : quads(qs)
  //            , movepolicy(pre.movepolicy)
  //            , resizepolicy(pre.resizepolicy)
  //        {}
  //        const Quads quads;
  //        const MovePolicy movepolicy;
  //        const ResizePolicy resizepolicy;
  //    };
  //struct NullLayout
  //{
  //    struct Preset {};
  //    constexpr NullLayout(const Preset)
  //    {}
  //};
  //using NullPreset = typename NullLayout::Preset;

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

	QuadElement() = delete;
	QuadElement(const QuadElement&) = default;
	QuadElement(QuadElement&&) = default;
	QuadElement& operator=(const QuadElement&) = default;
	QuadElement& operator=(QuadElement&&) = default;
	~QuadElement() = default;
	//{
	//  printf("DestroyinDestroying QuadElement\n%lu quads.\n", Quad::all.size());
	//  //QuadID::free();
	//};

	const Col color;
  };
  template<typename Col>
	void applyColor_imp(const QuadElement<Col>& elem, const Col col)
	{
	  colorQuad(elem, col);
	}

  template<typename... Elems>
	struct Widget : public BoundingBoxID, std::tuple<Elems...>, WidgetSignals<Elems...>
  {
	static constexpr size_t ELEMENT_COUNT = sizeof...(Elems);
	static constexpr size_t QUAD_COUNT = utils::sum(Elems::QUAD_COUNT...);

	using Colors = WidgetColors<Elems...>;
	using Signals = WidgetSignals<Elems...>;
	using Elements = std::tuple<Elems...>;
	using Quads = typename utils::tuple_generator<sizeof...(Elems), glm::vec4>::type;
	using MovePolicy = std::array<glm::vec2, sizeof...(Elems)>;
	using ResizePolicy = std::array<glm::vec4, sizeof...(Elems)>;

	using Signals::hold;

	const MovePolicy movepolicy;
	const ResizePolicy resizepolicy;

	struct Preset
	{
	  Preset(const Quads(&genQs)(const glm::vec4), const MovePolicy mp, const ResizePolicy rp, const std::tuple<typename Elems::Preset...> subs)
		: genQuads(genQs)
		  , movepolicy(mp)
		  , resizepolicy(rp)
		  , subpresets(subs)
	  {}
	  const Quads(&genQuads)(const glm::vec4);
	  const MovePolicy movepolicy;
	  const ResizePolicy resizepolicy;
	  const std::tuple<typename Elems::Preset...> subpresets;
	};

	Widget(const glm::vec4& q, const Preset& preset)
	  : BoundingBoxID(BoundingBoxID::all.makeID(q))
	  , Elements(std::move(utils::convert_tuple<Elems...>(preset.genQuads(q), preset.subpresets)))
	  , Signals((Elements)*this)
	  , movepolicy(preset.movepolicy)
	  , resizepolicy(preset.resizepolicy)
	{
	  puts("Creating Widget");
	}
	Widget() = delete;
	Widget(const Widget&) = default;
	Widget(Widget&&) = default;
	Widget& operator=(const Widget&) = default;
	Widget& operator=(Widget&&) = default;
	~Widget() = default;

	void move_n(utils::_index<0> i, const glm::vec2 v) const
	{}

	template<size_t N>
	  void move_n(utils::_index<N> i, const glm::vec2 v) const
	  {
		move_n(utils::_index<N-1>(), v);
		std::get<N-1>((Elements)*this).move(v * movepolicy[N-1]);
	  }
	void resize_n(utils::_index<0> i, const glm::vec2 v) const
	{}
	template<size_t N>
	  void resize_n(utils::_index<N> i, const glm::vec2 v) const
	  {
		resize_n(utils::_index<N-1>(), v);
		std::get<N-1>((Elements)*this).move(v * glm::vec2(resizepolicy[N-1].x, resizepolicy[N-1].y));
		std::get<N-1>((Elements)*this).resize(v * glm::vec2(resizepolicy[N-1].z, resizepolicy[N-1].w));
	  }
	void move(const glm::vec2 v) const
	{
	  (*this)->move(v);
	  move_n(utils::_index<ELEMENT_COUNT>(), v);
	}
	void resize(const glm::vec2 v) const
	{
	  (*this)->resize(v);
	  resize_n(utils::_index<ELEMENT_COUNT>(), v);
	}
	void setPos(const glm::vec2 p)
	{
	  move(p - glm::vec2((*this)->x, (*this)->y));
	}
	void setSize(const glm::vec2 s)
	{
	  resize(s - glm::vec2((*this)->z, (*this)->w));
	}
  };

  template<typename... Elems>
	void applyColor_imp(const Widget<Elems...>& wid, const WidgetColors<Elems...> cols)
	{
	  applyColor_imp_n(wid.elements, cols, utils::_index<sizeof...(Elems)>());
	}
}

