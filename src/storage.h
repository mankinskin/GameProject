#pragma once
#include <cstring>
#include <string>
#include <glew.h>
#include "debug.h"

namespace gl
{
  extern int SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
  extern int UNIFORM_BUFFER_OFFSET_ALIGNMENT;
  extern int MAX_UNIFORM_BUFFER_BINDINGS;
  extern int MIN_MAP_BUFFER_ALIGNMENT;

  unsigned int getNewTargetBinding(const unsigned int pTarget);

  template<typename T>
	struct Storage
	{
	  using size_type = size_t;
	  Storage()
	  {}
	  Storage(std::string pName, size_type pSize,
		  int pFlags, const void* pData = nullptr)
		:name(pName), capacity(pSize * sizeof(T)), flags(pFlags)
	  {
		glCreateBuffers(1, &ID);
		glNamedBufferStorage(ID, capacity, pData, flags);
	  }

	  void setTarget(unsigned int pTarget)
	  {
		target = pTarget;
		binding = getNewTargetBinding(pTarget);
		if (pTarget == GL_UNIFORM_BUFFER
			|| pTarget == GL_SHADER_STORAGE_BUFFER
			|| pTarget == GL_ATOMIC_COUNTER_BUFFER
			|| pTarget == GL_TRANSFORM_FEEDBACK_BUFFER) {
		  glBindBufferBase(pTarget, binding, ID);
		}
	  }

	  std::string name;
	  unsigned int ID;
	  size_type capacity;
	  int flags;
	  unsigned int target;
	  unsigned int binding;
	};

  template<typename T>
	struct StreamStorage : public Storage<T>
	{
	  StreamStorage()
	  {
	  }
	  StreamStorage(std::string pName, typename Storage<T>::size_type pSize,
		  int pFlags, const void* pData = nullptr)
		: Storage<T>(pName, pSize, pFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT, pData)
	  {
		mappedPtr = glMapNamedBufferRange(Storage<T>::ID, 0,
			Storage<T>::capacity, Storage<T>::flags);
		if (!mappedPtr) {
		  debug::pushError("Failed to map Storage " + Storage<T>::name + " !\n");
		}
	  }

	  void* mappedPtr;
	};

  template<typename T>
	void uploadStorage(const StreamStorage<T>& pStorage, const unsigned int pByteSize, const void* pData)
	{
	  if (pByteSize) {
		if (!pStorage.mappedPtr) {
		  debug::pushError("Attempted to upload to unmapped buffer!", debug::Error::Fatal);
		  return;
		}
		std::memcpy(pStorage.mappedPtr, pData, pByteSize);
	  }
	}
}

