#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
  using ButtonBase = Widget<QuadElement<gl::Color>, QuadElement<gl::Color>>;
  struct Button : public ButtonBase
  {
	static constexpr size_t ELEMENT_COUNT = ButtonBase::ELEMENT_COUNT;
	using Base = ButtonBase;
	using Preset = typename Base::Preset;
	using Colors = typename Base::Colors::Colors;
	using Elements = typename Base::Elements;
	using Layout = typename Base::Layout;

	Button(const glm::vec4 q, const Preset pre)
	  : Base(q, pre)
	{
	  using namespace signals;
	  const Base& w = *this;
	  puts("Init Button");
	  link(w.enter, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w), gl::getColor("white")));
	  link(w.leave, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w), std::get<0>(w).color));

	  link(w.press, func(applyColor<std::tuple_element_t<1, Elements>>, std::get<1>(w), gl::getColor("white")));
	  link(w.press, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w), gl::getColor("white")));

	  link(w.release, func(applyColor<std::tuple_element_t<1, Elements>>, std::get<1>(w), std::get<1>(w).color));
	  link(w.release, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w), std::get<0>(w).color));
	}
	template<size_t MARGINX = 2, size_t MARGINY = 2>
	  static const typename Base::Quads genQuads(const glm::vec4 q)
	  {
		return typename Base::Quads{
		  glm::vec4(q.x, q.y, q.z, q.w),
			glm::vec4(q.x + toScreenX(MARGINX), q.y - toScreenY(MARGINY),
				q.z - toScreenX(MARGINX*2),
				q.w - toScreenY(MARGINY*2))};
	  }
  };

  template<size_t MARGINX = 2, size_t MARGINY = 2>
  const typename Button::Layout buttonLayout(Button::genQuads<MARGINX, MARGINY>, {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)}, {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)});
  template<size_t MARGINX = 2, size_t MARGINY = 2>
	const typename Button::Preset buttonPreset(buttonLayout<MARGINX, MARGINY>, {gl::Color(1), gl::Color(12)});
}
