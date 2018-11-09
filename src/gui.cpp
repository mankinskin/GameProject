#include "gui.h"
#include "gldebug.h"
#include "debug.h"
#include "vao.h"
#include "quad.h"
#include "line.h"
#include "quadcolors.h"
#include "framebuffer.h"
#include "widget.h"
#include "input.h"
#include "signal.h"
#include "functor.h"
#include "mouse.h"
#include "signal.h"
#include "app.h"
#include "light.h"
#include "viewport.h"
#include "font.h"
#include "layouts/button.h"
#include "layouts/window.h"
#include "layouts/slider.h"
#include "printer.h"

using gl::ColorData;
using gl::Color;

void gui::draw()
{
  glDepthFunc(GL_ALWAYS);
  renderColorQuads();
  text::renderTexts();
  glDepthFunc(GL_LESS);
}

void gui::initWidgets()
{
  using namespace signals;
  using namespace input;

  const float width = 1.0f;
  const size_t xcount = 2;

  puts("Button");
  for (size_t w = 0; w < 1; ++w) {
	Button b(glm::vec4(-1.0f + (width/xcount) * (w%xcount), 1.0f - (0.1f * floor(w/xcount)), gl::toScreenX(300), gl::toScreenY(200)), buttonPreset);
	puts("Finished");
  }

  puts("Window");
  for (size_t w = 0; w < 1; ++w) {
    Window b(glm::vec4(-1.0f + (width/xcount) * (w%xcount), 0.0f - (0.1f * floor(w/xcount)), gl::toScreenX(300), gl::toScreenY(200)), windowPreset);
    puts("Finished");
  }
  puts("Slider");
  for (size_t s = 0; s < 3; ++s) {
    Slider b(glm::vec4(-0.8f + (width/xcount) * (s%xcount), -0.5f - (0.1f * floor(s/xcount)), gl::toScreenX(300), gl::toScreenY(10)), Slider::Preset(Slider::Layout(0.0f, 1.0f, gl::Color(1)->x), sliderColors));
    puts("Finished");
  }
  puts("End");
}

