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

static glm::vec2 pixel_size;

using gl::ColorData;
using gl::Color;

void gui::init()
{
  pixel_size = glm::vec2(
	  2.0f / gl::getWidth(),
	  2.0f / gl::getHeight());
}

float gui::pixel_round_x(const float pIn)
{
  return pixel_size.x * round(pIn / pixel_size.x);
}
float gui::pixel_round_y(const float pIn)
{
  return pixel_size.y * round(pIn / pixel_size.y);
}
glm::vec2 gui::pixel_round(const glm::vec2 pIn)
{
  return pixel_size * round(pIn / pixel_size);
}

size_t gui::toPixelsX(const float screenX)
{
  return (size_t)round(screenX / pixel_size.x);
}
size_t gui::toPixelsY(const float screenY)
{
  return (size_t)round(screenY / pixel_size.y);
}
glm::uvec2 gui::toPixels(const glm::vec2 screen)
{
  return glm::uvec2(toPixelsX(screen.x), toPixelsY(screen.y));
}
float gui::toScreenX(const size_t pixelsX)
{
  return pixelsX * pixel_size.x;
}
float gui::toScreenY(const size_t pixelsY)
{
  return pixelsY * pixel_size.y;
}
glm::vec2 gui::toScreen(const glm::uvec2 pixels)
{
  return glm::vec2(toScreenX(pixels.x), toScreenY(pixels.y));
}

void gui::draw()
{
  glDepthFunc(GL_ALWAYS);
  renderColorQuads();
  text::renderFonts();
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
	Button b(glm::vec4(-1.0f + (width/xcount) * (w%xcount), 1.0f - (0.1f * floor(w/xcount)), toScreenX(300), toScreenY(200)), buttonPreset);
	puts("Finished");
  }

  puts("Window");
  for (size_t w = 0; w < 1; ++w) {
    Window b(glm::vec4(-1.0f + (width/xcount) * (w%xcount), 0.0f - (0.1f * floor(w/xcount)), toScreenX(300), toScreenY(200)), windowPreset);
    puts("Finished");
  }
  puts("Slider");
  for (size_t s = 0; s < 3; ++s) {
    Slider b(glm::vec4(-0.8f + (width/xcount) * (s%xcount), -0.5f - (0.1f * floor(s/xcount)), toScreenX(300), toScreenY(10)), Slider::Preset(Slider::Layout(0.0f, 1.0f, gl::Color(1)->x), sliderColors));
    puts("Finished");
  }
  puts("End");
}

