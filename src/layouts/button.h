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
	using Colors = typename Base::Colors;
	using Elements = typename Base::Elements;

	struct Layout : public Base::Layout
	{
	  const size_t marginx = 2;
	  const size_t marginy = 2;
	  const typename Base::Quads genQuads(const glm::vec4 q) const
	  {
		return typename Base::Quads{
		  glm::vec4(q.x, q.y, q.z, q.w),
			glm::vec4(q.x + gl::toScreenX(marginx), q.y - gl::toScreenY(marginy),
				q.z - gl::toScreenX(marginx*2),
				q.w - gl::toScreenY(marginy*2))};
	  }

	  Layout(size_t mx = 2, size_t my = 2)
		: Base::Layout({gui::QuadLayout(), gui::QuadLayout()}, {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
			{glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)})
	  , marginx(mx)
	  , marginy(my)
	  {}
	};

	struct Preset : public Layout, Base::Colors
	{
	  Preset(const Layout layout, typename Base::Colors colors)
		: Layout(layout)
		, Base::Colors(colors)
	  {}
	};

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

  const typename Button::Layout buttonLayout;
  const typename Button::Colors buttonColors({gl::Color(1), gl::Color(12)});
  const typename Button::Preset buttonPreset(buttonLayout, buttonColors);
}
