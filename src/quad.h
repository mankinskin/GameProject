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
	using ID = utils::ID<Quad>;
	constexpr static typename ID::Container& all = ID::container;
	constexpr Quad(glm::vec4 pData)
	  : glm::vec4(pData)
	{}
	void setPos(const glm::vec2 p);
	void move(const glm::vec2 v);
	void resize(const glm::vec2 v);
  };

  typedef utils::ID<Quad> QuadID;

  const unsigned int MAX_QUAD_COUNT = 10000;
  void setQuadPos(const utils::ID<Quad> q, const glm::vec2 p);
  void moveQuad(const utils::ID<Quad> q, const glm::vec2 v);
  utils::ID<Quad> topQuadAtPosition(const float x, const float y);

  void initQuadBuffer();
  void updateQuadBuffer();
  extern gl::StreamStorage<glm::vec4> quadBuffer;
}
