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
	struct ID : public Management<T>
  {
	using Preset = T;
	using Management<T>::get;
	using Management<T>::container;
	size_t index;

	constexpr ID(const size_t i, typename Management<T>::ContainerType* _c)
	  : Management<T>(_c)
	  , index(i)
	{}
	constexpr ID(typename Management<T>::ContainerType* _c)
	  : Management<T>(_c)
	  , index(INVALID_ID)
	{}

	T* operator->() const
	{
	  return &get(index);
	}
	T& operator*() const
	{
	  return get(index);
	}
	operator size_t() const
	{
	  return index;
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
	using ID = ID<T>;
	using Management<T>::allocate;
	Container()
	  : Management<T>(this)
	{}
	void clear()
	{
	  Management<T>::clear();
	  std::vector<T>::clear();
	};
	ID makeID(const T&& t)
	{
	  return allocate(std::move(t));
	}
	ID makeID(const T& t)
	{
	  return allocate(std::forward<const T&>(t));
	}
  };
  template<typename T>
	struct UnmanagedPolicy
	{
	  using IDType = ID<T, UnmanagedPolicy>;
	  using ContainerType = Container<T, UnmanagedPolicy>;
	  ContainerType* container;
	  constexpr UnmanagedPolicy(ContainerType* c)
		: container(c)
	  {}
	  constexpr IDType allocate(const T& t) const
	  {
		container->push_back(t);
		return IDType(container->size() - 1, container);
	  }
	  constexpr IDType allocate(T&& t) const
	  {
		container->emplace_back(t);
		return IDType(container->size() - 1, container);
	  }
	  void clear()
	  {
	  }
	  T& get(const size_t i) const
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
	  using ContainerType = Container<T, ManagedPolicy>;
	  ContainerType* container;
	  std::vector<size_t> access;
	  constexpr ManagedPolicy(const ContainerType& c)
		: container(&c)
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
		printf("Freeing ID %lu\n", i);
		container->erase(container->begin() + access[i]);
		access[i] = INVALID_ID;
		for (size_t it = i + 1; it < access.size(); ++it) {
		  --access[it];
		}
	  }
	  void clear()
	  {
		access.clear();
	  }
	  IDType& get(const size_t i) const
	  {
		printf("Accessing ID %lu\n", i);
		if (i >= container->access.size())
		  throw std::runtime_error("ID index (" + std::to_string(i) + ") out of bounds " + std::to_string(container->access.size()));
		if (container->access[i] >= container->size())
		  throw std::runtime_error("ID access (" + std::to_string(container->access[i]) + ") out of bounds " + std::to_string(container->size()));
		return *container[container->access[i]];
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
