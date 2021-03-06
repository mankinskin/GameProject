#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"
#include "slider.h"

namespace gui
{
  using WindowBase = Widget<QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>,  QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>, Button, Slider>;
  struct Window : public WindowBase
  {
    using Base = WindowBase;
    static constexpr const size_t ELEMENT_COUNT = Base::ELEMENT_COUNT;
    using Colors = typename Base::Colors;

	struct Layout : public Base::Layout
	{
      const size_t headerHeight = 20;
      const size_t marginx;
      const size_t marginy;

      const typename Base::Quads genQuads(const glm::vec4 q) const
      {
    	return typename Base::Quads {
		  glm::vec4(q.x, q.y - gl::toScreenY(headerHeight), gl::toScreenX(marginx), q.w - gl::toScreenY(marginy + headerHeight)),
			glm::vec4(q.x + gl::toScreenX(marginx), q.y - gl::toScreenY(headerHeight), q.z - gl::toScreenX(marginx*2), q.w - gl::toScreenY(marginy + headerHeight)),
			glm::vec4(q.x + (q.z - gl::toScreenX(marginx)), q.y - gl::toScreenY(headerHeight), gl::toScreenX(marginx), q.w - gl::toScreenY(marginy + headerHeight)),

			glm::vec4(q.x, q.y - (q.w - gl::toScreenY(marginy)), gl::toScreenX(marginx), gl::toScreenY(marginy)),
			glm::vec4(q.x + gl::toScreenX(marginx), q.y - (q.w - gl::toScreenY(marginy)), q.z - gl::toScreenX(marginx*2), gl::toScreenY(marginy)),
			glm::vec4(q.x + q.z - gl::toScreenX(marginx), q.y - (q.w - gl::toScreenY(marginy)), gl::toScreenX(marginx), gl::toScreenY(marginy)),

			glm::vec4(q.x, q.y, q.z, gl::toScreenY(headerHeight)), glm::vec4(q.x + gl::toScreenX(marginx), q.y - gl::toScreenY(headerHeight), q.z - gl::toScreenX(marginx*2), gl::toScreenY(headerHeight))};
      }
	  Layout(const size_t mx = 3, const size_t my = 3)
		: Base::Layout({gui::QuadLayout(), gui::QuadLayout(), gui::QuadLayout(), gui::QuadLayout(), gui::QuadLayout(), gui::QuadLayout(), Button::Layout(mx, my), Slider::Layout(0.0f, 100.0f, camera::main_camera.pos.x)},
			{glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
			{glm::vec4(0.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)})
		, marginx(mx)
		, marginy(my)
		{}
	};
	struct Preset : public Layout, Base::Colors
	{
      Preset(const Layout layout, const typename Base::Colors colors)
		: Layout(layout)
		, Base::Colors(colors)
    	{}
	};


    Window(const glm::vec4 q, const Preset pre)
      : Base(q, pre)
    {
      using namespace signals;
      using Colors = Base::Colors::Colors;
      using Elements = Base::Elements;
      const Base& w = *this;

      puts("Init Window");
      link(std::get<6>(w).hold, refFunc(moveWidget<Base>, (Base)w, input::Cursor::frameDelta));
      link(std::get<5>(w).hold, refFunc(resizeWidget<Base>, (Base)w, input::Cursor::frameDelta));
      link(std::get<2>(w).hold, refFunc(resizeWidgetX<Base>, (Base)w, input::Cursor::frameDelta.x));
      link(std::get<4>(w).hold, refFunc(resizeWidgetY<Base>, (Base)w, input::Cursor::frameDelta.y));
    }
  };

  const typename Window::Layout windowLayout;
  const typename Window::Colors windowColors({gl::Color(1), gl::Color(10), gl::Color(1), gl::Color(1), gl::Color(1), gl::Color(1), buttonColors, sliderColors});
  const typename Window::Preset windowPreset(windowLayout, windowColors);
}
