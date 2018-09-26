#include "quad.h"
#include "shader.h"
#include "vao.h"
#include <algorithm>
#include "gldebug.h"
#include "framebuffer.h"
#include "primitives.h"
#include "quadcolors.h"

gl::StreamStorage<glm::vec4> gui::quadBuffer;
typename gui::QuadContainer gui::QuadID::all = typename gui::QuadContainer();
typename gui::QuadContainer gui::BoundingBoxID::all = typename gui::QuadContainer();

void gui::initQuadBuffer()
{
  quadBuffer = gl::StreamStorage<glm::vec4>("QuadBuffer",
	  MAX_QUAD_COUNT, GL_MAP_WRITE_BIT, &QuadID::all[0]);
  quadBuffer.setTarget(GL_UNIFORM_BUFFER);
}

void gui::updateQuadBuffer()
{
  if (QuadID::all.size()) {
	gl::uploadStorage(quadBuffer,
		sizeof(glm::vec4)*QuadID::all.size(), &QuadID::all[0]);
  }
}

void gui::Quad::setPos(const glm::vec2 p)
{
  x = p.x;
  y = p.y;
}

void gui::Quad::move(const glm::vec2 v)
{
  x += v.x;
  y += v.y;
}

void gui::Quad::resize(const glm::vec2 v)
{
  z += v.x;
  w += v.y;
}

void gui::setQuadPos(const QuadID q, const glm::vec2 p)
{
  q->setPos(p);
}
void gui::moveQuad(const QuadID q, const glm::vec2 v)
{
  q->move(v);
}

gui::QuadID gui::topQuadAtPosition(const float x, const float y)
{
  const size_t quadCount = QuadID::all.size();
  for (size_t q = quadCount; !!q; --q) {
	const Quad& quad = QuadID::all[q];
	if ((x > quad.x) && (y < quad.y) && x < (quad.x + quad.z) && y > (quad.y - quad.w)) {
	  return QuadID(q);
	}
  }
  return QuadID();
}
