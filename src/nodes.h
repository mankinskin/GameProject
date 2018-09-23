#pragma once
#include "storage.h"
#include <vector>
#include <glm.hpp>
#include "utils/id.h"

//nodes
//nodes hold the information about the transformation of an object. They store a position, an orientation and a scale.
namespace nodes
{
  struct Node
  {
	Node(const glm::mat4& pMat)
	  : mat(pMat)
	{}
	using Container = utils::Container<Node>;
	using ID = typename Container::ID;
	static Container all;
	static ID create();
	static void reserve(const size_t n);
	glm::mat4 mat;

	void setPos(const glm::vec3 v);
	void move(const glm::vec3 v);
	void setRotation(const glm::vec3 v);
	void setScale(const glm::vec3 v);
	void rotate(const float angle, const glm::vec3 v);
	void scale(const glm::vec3 v);
	glm::vec4 getTranslation() const;
	glm::vec3 getPos() const;
	glm::vec3 getScale() const;
	glm::vec4 getRotation() const;
  };
  using NodeID = typename Node::ID;
  void initNodeBuffers();
  void updateNodeBuffers();

  extern gl::StreamStorage<glm::mat4> nodeBuffer;

  const unsigned int MAX_NODES = 1000;
  extern unsigned int num_nodes;

}
