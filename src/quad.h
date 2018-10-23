#pragma once
#include <cstring>
#include <glm.hpp>
#include <vector>
#include <tuple>
#include "utils/id.h"
#include "storage.h"
#include "color.h"
#include "signal.h"

namespace gui
{
  // screen quadrants
  // can be assigned color, or anything
  // only provides managed buffer of quads
  // rendering is responsibility of the component using the quad
  struct Quad : public glm::vec4
  {
	constexpr Quad(const glm::vec4 pData)
	  : glm::vec4(pData)
	{}
	Quad(const glm::vec2 pPos, const glm::vec2 pSize)
	  : glm::vec4(pPos.x, pPos.y, pSize.x, pSize.y)
	{}
	glm::vec2 getSize() const { return glm::vec2(z, w); }
	glm::vec2 getPos() const { return glm::vec2(x, y); }
	void setSize(const glm::vec2 s)
	{
	  z = s.x;
	  w = s.y;
	}
	void setPos(const glm::vec2 p)
	{
	  x = p.x;
	  y = p.y;
	}
	void move(const glm::vec2 v)
	{
	  x += v.x;
	  y += v.y;
	}
	void resize(const glm::vec2 v)
	{
	  z += v.x;
	  w += v.y;
	}
  };

  using QuadContainer = utils::Container<Quad, utils::UnmanagedPolicy>;
  struct QuadID : public QuadContainer::ID
  {
	static QuadContainer all;
	using ID = typename QuadContainer::ID;
	QuadID()
	  : QuadContainer::ID(&all)
	{}
	QuadID(const size_t i)
	  : QuadContainer::ID(i, &all)
	{}
  };

  const unsigned int MAX_QUAD_COUNT = 10000;
  void setQuadPos(const QuadID q, const glm::vec2 p);
  void moveQuad(const QuadID q, const glm::vec2 v);
  QuadID topQuadAtPosition(const float x, const float y);

  void initQuadBuffer();
  void updateQuadBuffer();
  extern gl::StreamStorage<glm::vec4> quadBuffer;
}
