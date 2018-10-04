#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
  using SliderBaseLayout = WidgetLayout<Button, QuadElement<gl::Color>>;
  struct SliderLayout : public SliderBaseLayout
  {
      static const size_t slideWidth = 10;
      static const typename SliderBaseLayout::Quads genQuads(const glm::vec4 q)
      {
    	return typename SliderBaseLayout::Quads{
    	  glm::vec4(q.x, q.y, q.z, q.w),
    		glm::vec4(q.x, q.y,
    			toScreenX(slideWidth),
    			q.w)};
      }
	SliderLayout()
	  : SliderBaseLayout(
    		{glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
    		{glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)})
	{}
  };
  using SliderBase = Widget<SliderLayout, Button, QuadElement<gl::Color>>;
  struct Slider : public SliderBase
  {
    using Base = SliderBase;
    static constexpr size_t ELEMENT_COUNT = Base::ELEMENT_COUNT;
    using Preset = typename Base::Preset;
    using Elements = typename Base::Elements;
    using Colors = typename Base::Colors::Colors;
    using BoxElement = std::tuple_element_t<0, Elements>;
    using SlideElement = std::tuple_element_t<1, Elements>;

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
      link(this->press, func(applyColor<SlideElement>, slide(), gl::getColor("white")));
      link(this->release, func(applyColor<SlideElement>, slide(), slide().color));
      link(this->hold, refFunc(setSlidePos, (Slider)*this, input::relativeCursorPosition.x));
      link(this->hold, func(setTargetValue, *this));
    }
  };

  const typename Slider::Preset sliderPreset({buttonPreset, gl::Color(12)});
}
