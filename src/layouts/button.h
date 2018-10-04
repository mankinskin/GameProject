#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
  using ButtonBaseLayout = WidgetLayout<QuadElement<gl::Color>, QuadElement<gl::Color>>;
  struct ButtonLayout : public ButtonBaseLayout
  {
	const size_t marginx = 2;
	const size_t marginy = 2;
	const typename ButtonBaseLayout::Quads genQuads(const glm::vec4 q)
	{
	  return typename ButtonBaseLayout::Quads{
		glm::vec4(q.x, q.y, q.z, q.w),
		  glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(marginy),
			  q.z - toScreenX(marginx*2),
			  q.w - toScreenY(marginy*2))};
	}
	ButtonLayout()
	  : ButtonBaseLayout(
		  {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		  {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)})
	{}
  };

  using ButtonBase = Widget<ButtonLayout, QuadElement<gl::Color>, QuadElement<gl::Color>>;

  struct Button : public ButtonBase
  {
	static constexpr size_t ELEMENT_COUNT = ButtonBase::ELEMENT_COUNT;
	using Base = ButtonBase;
	using Preset = typename Base::Preset;
	using Colors = typename Base::Colors::Colors;
	using Elements = typename Base::Elements;

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
  };

  const typename Button::Preset buttonPreset({gl::Color(1), gl::Color(12)});
}
