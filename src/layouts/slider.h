#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
  using SliderBase = Widget<QuadElement<gl::Color>, QuadElement<gl::Color>>;

  struct Slider : public SliderBase
  {
    using Base = SliderBase;
    static constexpr size_t ELEMENT_COUNT = Base::ELEMENT_COUNT;
    using Elements = typename Base::Elements;
    using Colors = typename Base::Colors;
    using BoxElement = std::tuple_element_t<0, Elements>;
    using SlideElement = std::tuple_element_t<1, Elements>;


	struct Layout : public Base::Layout
	{

	  static const size_t slideWidth = 10;
	  const typename Base::Quads genQuads(const glm::vec4 q) const
	  {
		return typename Base::Quads{
		  glm::vec4(q.x, q.y, q.z, q.w),
			glm::vec4(q.x, q.y,
				toScreenX(slideWidth),
				q.w)};
	  }
	  Layout()
		: Base::Layout({gui::QuadLayout(), gui::QuadLayout()}, {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)}, {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)})
		{}
	};
	struct Preset : public Layout, Base::Colors
	{
	  Preset(const Layout layout, const Base::Colors colors)
		: Layout(layout)
		  , Base::Colors(colors)
	  {}
	};

    const BoxElement& box() const
    {
      return std::get<0>(*this);
    }
    const SlideElement& slide() const
    {
      return std::get<1>(*this);
    }
    // slider values
    float& targ;  // the value to set by the slider
    float min;	  // the value when the slide is left
    float max;	  // the value when the slide is right

    static void setTargetValue(const Slider& s)
    {
      const float swidth = s.slide().width();
      const float bwidth = s.box().width();
      const float width = bwidth - swidth;
      const float pos = s.slide().x() - s.box().x();
      s.targ = s.min + (s.max - s.min) * (pos / width);
    }
    static void setSlidePos(const Slider& s, const float& xv)
    {
      const float swidth = s.slide().width();
      const float bpos = s.box().x();
      const float bwidth = s.box().width();
      setWidgetPosX<SlideElement>(s.slide(), std::min(bpos + (bwidth - swidth), std::max(bpos, xv - swidth/2.0f)));
    }
    Slider(const glm::vec4 q, const Preset pre, float pMin, float pMax, float& t)
      : Base(q, pre)
      , targ(std::forward<float&>(t))
      , min(pMin)
      , max(pMax)
    {
      using namespace signals;
      puts("Init Slider");
      link(this->press, proc(func(input::Cursor::toggleHide), func(applyColor<SlideElement>, slide(), gl::getColor("white"))));
      link(this->release, proc(func(input::Cursor::toggleHide), func(applyColor<SlideElement>, slide(), slide().color)));
      link(this->hold, refFunc(setSlidePos, (Slider)*this, input::Cursor::relPos.x));
      link(this->hold, func(setTargetValue, *this));
    }
  };

  const typename Slider::Layout sliderLayout;
  const typename Slider::Colors sliderColors({gl::Color(1), gl::Color(12)});
  const typename Slider::Preset sliderPreset(sliderLayout, sliderColors);
}
