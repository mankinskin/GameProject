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
	constexpr Quad(glm::vec4 pData)
	  : glm::vec4(pData)
	{}
	void setPos(const glm::vec2 p);
	void move(const glm::vec2 v);
	void resize(const glm::vec2 v);
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

  struct BoundingBoxID : public QuadContainer::ID
  {
	static QuadContainer all;
	BoundingBoxID()
	  : QuadContainer::ID(&all)
	{}
	BoundingBoxID(const size_t i)
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
