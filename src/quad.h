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
  struct Quad : public glm::vec4
  {
	using Container = utils::Container<Quad>;
	using ID = typename Container::ID;
	static Container all;
	constexpr Quad(glm::vec4 pData)
	  : glm::vec4(pData)
	{}
	void setPos(const glm::vec2 p);
	void move(const glm::vec2 v);
	void resize(const glm::vec2 v);
  };

  struct QuadID : public Quad::ID
  {
	QuadID()
	  : Quad::ID(Quad::all)
	{}
	QuadID(const size_t i)
	  : Quad::ID(i, Quad::all)
	{}
  };

  using BoundingBox = glm::vec4;
  struct BoundingBoxID : public utils::ID<BoundingBox>
  {
	using Container = utils::Container<BoundingBox>;
	static Container all;
	BoundingBoxID()
	  : utils::ID<BoundingBox>(all)
	{}
	BoundingBoxID(size_t i)
	  : utils::ID<BoundingBox>(i, all)
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
