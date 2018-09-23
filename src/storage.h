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
	  Storage(std::string pName, const std::vector<T>& c, int pFlags)
		: name(pName)
		, capacity(c.size() * sizeof(T))
		, flags(pFlags)
	  {
		create(&c[0]);
	  }
	  Storage(std::string pName, size_type pSize, int pFlags, const void* pData = nullptr)
		: name(pName)
		, capacity(pSize * sizeof(T))
		, flags(pFlags)
	  {
		create(pData);
	  }
	  private:
	  void create(const void* pData)
	  {
		glCreateBuffers(1, &ID);
		if (capacity)
		  glNamedBufferStorage(ID, capacity, pData, flags);
		else
		  debug::warning("Can't create Storage " + name + ": would be empty!");
	  }
	  public:
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
	  private:
	  void map()
	  {
		mappedPtr = glMapNamedBufferRange(Storage<T>::ID, 0,
			Storage<T>::capacity, Storage<T>::flags);
		if (!mappedPtr) {
		  debug::warning("Failed to map Storage " + Storage<T>::name + " !\n");
		}
	  }
	  public:
	  StreamStorage()
	  {}
	  StreamStorage(std::string pName, const std::vector<T>& c, int pFlags)
		: Storage<T>(pName, c, pFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
	  {
		map();
	  }
	  StreamStorage(std::string pName, typename Storage<T>::size_type pSize,
		  int pFlags, const void* pData = nullptr)
		: Storage<T>(pName, pSize, pFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT, pData)
	  {
		map();
	  }

	  void* mappedPtr;
	};

  template<typename T>
	void uploadStorage(const StreamStorage<T>& pStorage, const unsigned int pByteSize, const void* pData)
	{
	  if (pByteSize) {
		if (!pStorage.mappedPtr) {
		  debug::fatal("Attempted to upload to unmapped buffer!");
		  return;
		}
		std::memcpy(pStorage.mappedPtr, pData, pByteSize);
	  }
	}
}

