#pragma once
#include <vector>
#include <functional>
#include <type_traits>
#include <stdexcept>
#include <algorithm>

namespace utils
{
  const size_t INVALID_ID = -1;

  template<typename T>
	class UnmanagedPolicy;
  template<typename T>
	class ManagedPolicy;

  template<typename T, template<class> class Management = UnmanagedPolicy>
	struct ID
  {
	using Preset = T;
	size_t index;
	typename Management<T>::ArrayType* container;

	constexpr ID()
	{}
	constexpr ID(const size_t i, typename Management<T>::ArrayType* _c)
	  : container(_c)
	  , index(i)
	{}
	constexpr ID(typename Management<T>::ArrayType* _c)
	  : container(_c)
	  , index(INVALID_ID)
	{}

	constexpr T* operator->() const
	{
	  if (!container) throw std::runtime_error("ID container uninitialized!");
	  return &container->get(index);
	}
	constexpr T& operator*() const
	{
	  if (!container) throw std::runtime_error("ID container uninitialized!");
	  return container->get(index);
	}
	constexpr operator size_t() const
	{
	  return index;
	}
	constexpr void free()
	{
	  container->free(index);
	}
	constexpr inline bool operator==(const ID<T, Management>& o) const
	{
	  return index == o.index && container == o.container;
	}
	constexpr inline bool operator!=(const ID<T, Management>& o) const
	{
	  return index != o.index || container != o.container;
	}
  };

  template<typename T, template<class> class Management = UnmanagedPolicy>
	struct Container : public std::vector<T>, Management<T>
  {
	using ID = ID<T, Management>;
	using Management<T>::allocate;
	constexpr Container()
	  : Management<T>(this)
	{}
	constexpr void clear()
	{
	  Management<T>::clear();
	  std::vector<T>::clear();
	};
	constexpr ID makeID(T&& t)
	{
	  return ID(allocate(std::move(t)), this);
	}
	constexpr ID makeID(const T& t)
	{
	  return ID(allocate(std::forward<const T&>(t)), this);
	}
  };
  template<typename T>
	struct UnmanagedPolicy
	{
	  using IDType = ID<T, UnmanagedPolicy>;
	  using ArrayType = Container<T, UnmanagedPolicy>;
	  ArrayType* container;
	  constexpr UnmanagedPolicy(ArrayType* c)
		: container(c)
	  {}
	  constexpr size_t allocate(const T& t)
	  {
		container->push_back(t);
		return container->size() - 1;
	  }
	  constexpr size_t allocate(T&& t)
	  {
		container->emplace_back(t);
		return container->size() - 1;
	  }
	  constexpr void clear()
	  {
	  }
	  constexpr T& get(const size_t i) const
	  {
		if (i >= container->size())
		  throw std::runtime_error("ID index (" + std::to_string(i) + ") out of bounds " + std::to_string(container->size()));
		return (*container)[i];
	  }
	};

  template<typename T>
	struct ManagedPolicy
	{
	  using IDType = ID<T, ManagedPolicy>;
	  using ArrayType = Container<T, ManagedPolicy>;
	  ArrayType* container;
	  std::vector<size_t> access;
	  constexpr ManagedPolicy(ArrayType* c)
		: container(c)
	  {}
	  constexpr size_t allocate(const T& t)
	  {
		access.emplace_back(container->size());
		container->push_back(std::forward<const T&>(t));
		return access.size() - 1;
	  }
	  constexpr size_t allocate(T&& t)
	  {
		access.emplace_back(container->size());
		container->emplace_back(std::move(t));
		return access.size() - 1;
	  }
	  constexpr void free(const size_t& i)
	  {
		printf("Freeing ID %lu\n", access[i]);
		container->erase(container->begin() + access[i]);
		access[i] = INVALID_ID;
		for (size_t it = i + 1; it < access.size(); ++it) {
		  --access[it];
		}
	  }
	  constexpr void clear()
	  {
		access.clear();
	  }
	  constexpr T& get(const size_t i) const
	  {
		if (i >= container->access.size())
		  throw std::runtime_error("ID index (" + std::to_string(i) + ") out of bounds " + std::to_string(container->access.size()));
		if (container->access[i] >= container->size())
		  throw std::runtime_error("ID access (" + std::to_string(container->access[i]) + ") out of bounds " + std::to_string(container->size()));
		return (*container)[container->access[i]];
	  }
	};

  //template<typename T>
  //  struct ReferenceCountedIDPolicy : public ManagedIDPolicy<T>
  //  {
  //    using Preset = T;
  //    using Type = ID_Type<T>;
  //    using ManagedIDPolicy<T, _container>::access;
  //    using ManagedIDPolicy<T, _container>::index;
  //    static std::vector<size_t> refs;
  //    std::vector<T>& container;

  //    ReferenceCountedIDPolicy(const size_t i)
  //  	: ManagedIDPolicy<T, _container>(i)
  //    {}
  //    ReferenceCountedIDPolicy() = default;
  //    ReferenceCountedIDPolicy(const ReferenceCountedIDPolicy& o)
  //  	: ManagedIDPolicy<T, _container>(std::forward<const ManagedIDPolicy<T, _container>&>(o))
  //    {
  //  	++refs[access[index]];
  //    }

  //    ReferenceCountedIDPolicy& operator=(const ReferenceCountedIDPolicy& o)
  //    {
  //      ManagedIDPolicy<T, _container>::operator=(std::forward<const ManagedIDPolicy<T, _container>&>(o));
  //      ++refs[access[index]];
  //      return *this;
  //    }

  //    ReferenceCountedIDPolicy(ReferenceCountedIDPolicy&& o) = default;
  //    ReferenceCountedIDPolicy& operator=(ReferenceCountedIDPolicy&& o) = default;

  //    inline ~ReferenceCountedIDPolicy() noexcept
  //    {
  //      --refs[access[index]];
  //      if (!refs[access[index]]) {
  //        free(index);
  //      }
  //    }

  //    static constexpr void free(const size_t& i)
  //    {
  //  	refs.erase(refs.begin() + access[i]);
  //  	ManagedIDPolicy<T, _container>::free(i);
  //    }
  //    static constexpr size_t allocate(const T& t)
  //    {
  //  	refs.emplace_back(1);
  //  	return ManagedIDPolicy<T, _container>::allocate(std::forward<const T&>(t));
  //    }
  //    static constexpr size_t allocate(T&& t)
  //    {
  //  	refs.emplace_back(1);
  //  	return ManagedIDPolicy<T, _container>::allocate(std::move(t));
  //    }
  //    static void clear()
  //    {
  //  	ManagedIDPolicy<T, container>::clear();
  //  	refs.clear();
  //    }
  //  };

  //template<typename T, std::vector<T>& _container>
  //  std::vector<size_t> ReferenceCountedIDPolicy<T, _container>::refs = std::vector<size_t>();

}
