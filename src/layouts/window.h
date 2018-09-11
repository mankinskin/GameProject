#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
  using WindowBase = Widget<QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>,  QuadElement<gl::Color>, QuadElement<gl::Color>, QuadElement<gl::Color>, Button>;
  struct Window : public WindowBase
  {
	static constexpr const size_t ELEMENT_COUNT = 7;
	using Base = WindowBase;
	using Preset = typename Base::Preset;
	using Colors = typename Base::Colors;

	static const size_t headerHeight = 20;
	static const size_t marginx = 5;
	static const size_t marginy = 5;


	Window(const glm::vec4 q, const Preset pre)
	  : Base(q, pre)
	{
	  using namespace signals;
	  using Colors = Base::Colors::Colors;
	  using Elements = Base::Elements::Elements;
	  const Base& w = *this;

	  link(std::get<6>(w.elements).hold, refFunc(moveWidget<Base>, (Base)w, input::cursorFrameDelta));
	  link(std::get<5>(w.elements).hold, refFunc(resizeWidget<Base>, (Base)w, input::cursorFrameDelta));
	  link(std::get<2>(w.elements).hold, refFunc(resizeWidgetX<Base>, (Base)w, input::cursorFrameDelta.x));
	  link(std::get<4>(w.elements).hold, refFunc(resizeWidgetY<Base>, (Base)w, input::cursorFrameDelta.y));
	}

	static const typename Base::Quads genQuads(const glm::vec4 q)
	{
	  return typename Base::Quads {
		glm::vec4(q.x, q.y - toScreenY(headerHeight), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),
		  glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(headerHeight), q.z - toScreenX(marginx*2), q.w - toScreenY(marginy + headerHeight)),
		  glm::vec4(q.x + (q.z - toScreenX(marginx)), q.y - toScreenY(headerHeight), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),

		  glm::vec4(q.x, q.y - (q.w - toScreenY(marginy)), toScreenX(marginx), toScreenY(marginy)),
		  glm::vec4(q.x + toScreenX(marginx), q.y - (q.w - toScreenY(marginy)), q.z - toScreenX(marginx*2), toScreenY(marginy)),
		  glm::vec4(q.x + q.z - toScreenX(marginx), q.y - (q.w - toScreenY(marginy)), toScreenX(marginx), toScreenY(marginy)),

		  glm::vec4(q.x, q.y, q.z, toScreenY(headerHeight))};
	}
  };
  const typename Window::Preset windowPreset = typename Window::Preset(Window::genQuads, {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)}, {glm::vec4(0.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)}, {gl::Color(1), gl::Color(10), gl::Color(1), gl::Color(1), gl::Color(1), gl::Color(1), buttonPreset<5, 5>});
}
