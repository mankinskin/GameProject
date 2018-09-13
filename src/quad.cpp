#include "quad.h"
#include "shader.h"
#include "vao.h"
#include <algorithm>
#include "gldebug.h"
#include "framebuffer.h"
#include "primitives.h"
#include "quadcolors.h"

gl::StreamStorage<glm::vec4> gui::quadBuffer;
typename gui::Quad::Container gui::Quad::all = typename gui::Quad::Container();
typename gui::BoundingBox::Container gui::BoundingBox::all = typename gui::BoundingBox::Container();

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
  const size_t quadCount = Quad::all.size();
  for (size_t q = quadCount; !!q; --q) {
	const Quad& quad = Quad::all[q];
	if ((x > quad.x) && (y < quad.y) && x < (quad.x + quad.z) && y > (quad.y - quad.w)) {
	  return QuadID(q);
	}
  }
  return QuadID();
}
