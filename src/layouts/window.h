#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
  using WindowBaseLayout = WidgetLayout<QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>,  QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>, Button>;

    struct WindowLayout : public WindowBaseLayout
    {
      const size_t headerHeight = 20;
      const size_t marginx = 5;
      const size_t marginy = 5;

      const typename WindowBaseLayout::Quads genQuads(const glm::vec4 q)
      {
    	return typename WindowBaseLayout::Quads {
    	  glm::vec4(q.x, q.y - toScreenY(headerHeight), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),
    		glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(headerHeight), q.z - toScreenX(marginx*2), q.w - toScreenY(marginy + headerHeight)),
    		glm::vec4(q.x + (q.z - toScreenX(marginx)), q.y - toScreenY(headerHeight), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),

    		glm::vec4(q.x, q.y - (q.w - toScreenY(marginy)), toScreenX(marginx), toScreenY(marginy)),
    		glm::vec4(q.x + toScreenX(marginx), q.y - (q.w - toScreenY(marginy)), q.z - toScreenX(marginx*2), toScreenY(marginy)),
    		glm::vec4(q.x + q.z - toScreenX(marginx), q.y - (q.w - toScreenY(marginy)), toScreenX(marginx), toScreenY(marginy)),

    		glm::vec4(q.x, q.y, q.z, toScreenY(headerHeight))};
      }
      WindowLayout()
    	: WindowBaseLayout(
    		{glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
    		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
    		{glm::vec4(0.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, -1.0f),
    		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f),
    		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)})
    	{}
    };
  using WindowBase = Widget<WindowLayout, QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>,  QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>, Button>;
  struct Window : public WindowBase
  {
    static constexpr const size_t ELEMENT_COUNT = 7;
    using Base = WindowBase;
    using Preset = typename Base::Preset;
    using Colors = typename Base::Colors;


    Window(const glm::vec4 q, const Preset pre)
      : Base(q, pre)
    {
      using namespace signals;
      using Colors = Base::Colors::Colors;
      using Elements = Base::Elements;
      const Base& w = *this;

      puts("Init Window");
      link(std::get<6>(w).hold, refFunc(moveWidget<Base>, (Base)w, input::cursorFrameDelta));
      link(std::get<5>(w).hold, refFunc(resizeWidget<Base>, (Base)w, input::cursorFrameDelta));
      link(std::get<2>(w).hold, refFunc(resizeWidgetX<Base>, (Base)w, input::cursorFrameDelta.x));
      link(std::get<4>(w).hold, refFunc(resizeWidgetY<Base>, (Base)w, input::cursorFrameDelta.y));
    }
  };

  const typename Window::Preset windowPreset = typename Window::Preset({gl::Color(1), gl::Color(10), gl::Color(1), gl::Color(1), gl::Color(1), gl::Color(1), buttonPreset});
}
