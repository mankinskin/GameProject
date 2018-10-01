#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
  using SliderBase = Widget<Button, QuadElement<gl::Color>>;
  struct Slider : public SliderBase
  {
	using Base = SliderBase;
	static constexpr size_t ELEMENT_COUNT = Base::ELEMENT_COUNT;
	using Preset = typename Base::Preset;
	using Elements = typename Base::Elements;
	using Colors = typename Base::Colors::Colors;
	using BoxElement = std::tuple_element_t<0, Elements>;
	using SlideElement = std::tuple_element_t<1, Elements>;
	using Layout = typename Base::Layout;

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
	  const float swidth = s.slide()->z;
	  const float bwidth = s.box()->z;
	  const float width = bwidth - swidth;
	  const float pos = s.slide()->x - s.box()->x;
	  s.targ = s.min + (s.max - s.min) * (pos / width);
	}
	static void setSlidePos(const Slider& s, const float& xv)
	{
	  const float swidth = s.slide()->z;
	  const float bpos = s.box()->x;
	  const float bwidth = s.box()->z;
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

	template<size_t SLIDE_WIDTH = 10>
	  static const typename Base::Quads genQuads(const glm::vec4 q)
	  {
		return typename Base::Quads{
		  glm::vec4(q.x, q.y, q.z, q.w),
			glm::vec4(q.x, q.y,
				toScreenX(SLIDE_WIDTH),
				q.w)};
	  }
  };

  template<size_t SLIDE_WIDTH = 10>
	const typename Slider::Layout sliderLayout(Slider::genQuads<SLIDE_WIDTH>,
		{glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)});
  template<size_t SLIDE_WIDTH = 10>
	const typename Slider::Preset sliderPreset(sliderLayout<>, {buttonPreset<>, gl::Color(12)});
}
