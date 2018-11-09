#include "viewport.h"

gl::Viewport gl::screen = gl::Viewport();
gl::Viewport* gl::Viewport::current = nullptr;

// Viewport class member function definitions
gl::Viewport::Viewport()
{}

gl::Viewport::Viewport(const app::Window& window, const float& res)
  : width(window.width)
  , height(window.height)
  , resolution(res)
  , pixel_size(2.0f / width, 2.0f / height)
{}

// binding Viewport instance as global selection
void gl::Viewport::bind()
{
  current = this;
  glViewport(0, 0, width, height);
}

unsigned int gl::getWidth()
{
  return Viewport::current->width;
}

unsigned int gl::getHeight()
{
  return Viewport::current->height;
}

float gl::getAspectRatio()
{
  return getWidth()/getHeight();
}

// rounding pixel values to the Viewports resolution
glm::vec2 gl::Viewport::pixel_round(const glm::vec2& pIn)
{
  return pixel_size * round(pIn / pixel_size);
}
float gl::Viewport::pixel_round_x(const float& pIn)
{
  return pixel_size.x * round(pIn / pixel_size.x);
}
float gl::Viewport::pixel_round_y(const float& pIn)
{
  return pixel_size.y * round(pIn / pixel_size.y);
}

// converting decimal screen coordinates (-1.0, 1.0) to integer pixel sizes (0, <size>)
size_t gl::Viewport::toPixelsX(const float& screenX)
{
  return (size_t)round(screenX / pixel_size.x);
}
size_t gl::Viewport::toPixelsY(const float& screenY)
{
  return (size_t)round(screenY / pixel_size.y);
}
glm::uvec2 gl::Viewport::toPixels(const glm::vec2& screen)
{
  return glm::uvec2(toPixelsX(screen.x), toPixelsY(screen.y));
}

// converting pixel sizes (0, <size>) to screen coordinates (-1.0, 1.0)
float gl::Viewport::toScreenX(const size_t& pixelsX)
{
  return pixelsX * pixel_size.x;
}
float gl::Viewport::toScreenY(const size_t& pixelsY)
{
  return pixelsY * pixel_size.y;
}
glm::vec2 gl::Viewport::toScreen(const glm::uvec2& pixels)
{
  return glm::vec2(toScreenX(pixels.x), toScreenY(pixels.y));
}

// global functions using the currently bound Viewport
const glm::vec2& gl::pixel_size()
{
  return Viewport::current->pixel_size;
}
glm::vec2 gl::pixel_round(const glm::vec2& pIn)
{
  return Viewport::current->pixel_round(pIn);
}
float gl::pixel_round_x(const float& pIn)
{
  return Viewport::current->pixel_round_x(pIn);
}
float gl::pixel_round_y(const float& pIn)
{
  return Viewport::current->pixel_round_y(pIn);
}
size_t gl::toPixelsX(const float& screenX)
{
  return Viewport::current->toPixelsX(screenX);
}
size_t gl::toPixelsY(const float& screenY)
{
  return Viewport::current->toPixelsY(screenY);
}
glm::uvec2 gl::toPixels(const glm::vec2& screen)
{
  return Viewport::current->toPixels(screen);
}
float gl::toScreenX(const size_t& pixelsX)
{
  return Viewport::current->toScreenX(pixelsX);
}
float gl::toScreenY(const size_t& pixelsY)
{
  return Viewport::current->toScreenY(pixelsY);
}
glm::vec2 gl::toScreen(const glm::uvec2& pixels)
{
  return Viewport::current->toScreen(pixels);
}

