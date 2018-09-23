#include "nodes.h"
#include "vao.h"
#include "model.h"

#include <gtc/matrix_transform.hpp>

utils::Container<nodes::Node> nodes::Node::all;
unsigned int nodes::num_nodes;
gl::StreamStorage<glm::mat4> nodes::nodeBuffer;

void nodes::initNodeBuffers()
{
  nodeBuffer = gl::StreamStorage<glm::mat4>("NodeBuffer", MAX_NODES, GL_MAP_WRITE_BIT);
  nodeBuffer.setTarget(GL_UNIFORM_BUFFER);
}

void nodes::updateNodeBuffers()
{
  //printf("Nodes: %lu\n", Node::all.size());
  //for (size_t m = 0; m < Node::all.size(); ++m) {
  //  const Node& node = Node::all[m];
  //  printf("Mat %lu\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\n", m,
  //  	node.mat[0][0], node.mat[0][1], node.mat[0][2], node.mat[0][3],
  //  	node.mat[1][0], node.mat[1][1], node.mat[1][2], node.mat[1][3],
  //  	node.mat[2][0], node.mat[2][1], node.mat[2][2], node.mat[2][3],
  //  	node.mat[3][0], node.mat[3][1], node.mat[3][2], node.mat[3][3]);
  //}
  if (Node::all.size()) {
	gl::uploadStorage(nodeBuffer, sizeof(glm::mat4)*Node::all.size(), &Node::all[0]);
  }
}

nodes::NodeID nodes::Node::create()
{
  return all.makeID(Node(glm::mat4(1.0f)));
}

void nodes::Node::reserve(const size_t n)
{
  all.reserve(num_nodes + n);
}

glm::vec4 nodes::Node::getTranslation() const
{
  return mat[3];
}
glm::vec3 nodes::Node::getPos() const
{
  return mat[3];
}
glm::vec3 nodes::Node::getScale() const
{
  return glm::vec3(mat[0][0], mat[1][1], mat[2][2]);
}
glm::vec4 nodes::Node::getRotation() const
{
  return glm::vec4();	// use quaternions
}
void nodes::Node::setRotation(const glm::vec3 v)
{

}
void nodes::Node::setScale(const glm::vec3 v)
{
}
void nodes::Node::setPos(glm::vec3 v)
{
  mat[3] = glm::vec4(v, 1.0f);
}
void nodes::Node::rotate(const float angle, const glm::vec3 v)
{
  mat = glm::rotate(mat, angle, v);
}
void nodes::Node::scale(const glm::vec3 v)
{
  mat = glm::scale(mat, v);
}
void nodes::Node::move(glm::vec3 v)
{
  mat[3] += glm::vec4(v, 1.0f);
}

