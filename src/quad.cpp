#include "quad.h"
#include "shader.h"
#include "vao.h"
#include <algorithm>
#include "gldebug.h"
#include "framebuffer.h"
#include "primitives.h"
#include "quadcolors.h"

gl::StreamStorage<glm::vec4> gui::quadBuffer;

void gui::initQuadBuffer()
{
  quadBuffer = gl::StreamStorage<glm::vec4>("QuadBuffer",
	  MAX_QUAD_COUNT, GL_MAP_WRITE_BIT, &Quad::all[0]);
  quadBuffer.setTarget(GL_UNIFORM_BUFFER);
}

void gui::updateQuadBuffer()
{
  if (Quad::all.size()) {
	gl::uploadStorage(quadBuffer,
		sizeof(glm::vec4)*Quad::all.size(), &Quad::all[0]);
  }
}

void gui::Quad::setPos(const glm::vec2 p)
{
  *this = glm::vec4(p.x, p.y, z, w);
}

void gui::Quad::move(const glm::vec2 v)
{
  *this += glm::vec4(v.x, v.y, 0.0f, 0.0f);
}

void gui::Quad::resize(const glm::vec2 v)
{
  *this += glm::vec4(0.0f, 0.0f, v.x, v.y);
}

void gui::setQuadPos(const utils::ID<Quad> q, const glm::vec2 p)
{
  q->setPos(p);
}
void gui::moveQuad(const utils::ID<Quad> q, const glm::vec2 v)
{
  q->move(v);
}

utils::ID<gui::Quad> gui::topQuadAtPosition(const float x, const float y)
{
  constexpr utils::ID<Quad>::Container& quads = utils::ID<Quad>::container;
  const size_t quadCount = quads.size();
  for (size_t q = 0; q < quadCount; ++q) {
	const size_t qi = quadCount - q - 1;
	const Quad& quad = quads[qi];
	if ((x > quad.x) && (y < quad.y) && x < (quad.x + quad.z) && y > (quad.y - quad.w)) {
	  return qi;
	}
  }
  return utils::INVALID_ID;
}
