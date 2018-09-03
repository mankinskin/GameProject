#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glew.h>
#include "storage.h"

namespace gl
{
  struct VAO
  {
	VAO()
	{}
	VAO(std::string pName)
	  : name(pName)
	{
	  printf("Creating VAO: %s...\n", name.c_str());
	  glCreateVertexArrays(1, &ID);
	}
	void vertexAttrib(unsigned int pBindingIndex, unsigned int pAttributeIndex,
		unsigned int pCount, unsigned int pType,
		unsigned int pOffset, bool pNormalize = false);

	template<typename T>
	  void vertexBuffer(unsigned int pBinding, Storage<T>& pBuffer,
		  unsigned int pStride = sizeof(T))
	  {
		pBuffer.setTarget(GL_ARRAY_BUFFER);
		glVertexArrayVertexBuffer(ID, pBinding, pBuffer.ID, 0, pStride);
	  }

	template<typename T>
	  void elementBuffer(Storage<T>& pBuffer)
	  {
		pBuffer.setTarget(GL_ELEMENT_ARRAY_BUFFER);
		glVertexArrayElementBuffer(ID, pBuffer.ID);
	  }

	void attribDivisor(unsigned int pAttrib, unsigned int pDivisor);

	void bind()
	{
	  glBindVertexArray(ID);
	}
	void unbind()
	{
	  glBindVertexArray(0);
	}
	operator unsigned int()
	{
	  return ID;
	}
	std::string name;
	unsigned int ID;
  };

}
