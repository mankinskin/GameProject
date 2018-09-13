#pragma once
#include <vector>
#include <functional>
#include <type_traits>
#include <assert.h>
#include <algorithm>

namespace utils
{
  const size_t INVALID_ID = -1;

  template<typename T>
	using ID_Type = typename std::remove_reference<T>::type;


  //template<typename T, std::vector<T>& _container>
  //  struct ManagedIDPolicy
  //  {
  //    static constexpr std::vector<T>& container = _container;
  //    static std::vector<size_t> access;
  //    size_t index;

  //    constexpr ManagedIDPolicy(const size_t i)
  //  	: index(i)
  //    {}
  //    ManagedIDPolicy()
  //  	: index(INVALID_ID)
  //    {}
  //    ManagedIDPolicy(const ManagedIDPolicy&) = default;
  //    ManagedIDPolicy(ManagedIDPolicy&&) = default;
  //    ManagedIDPolicy& operator=(const ManagedIDPolicy&) = default;
  //    ManagedIDPolicy& operator=(ManagedIDPolicy&&) = default;
  //    ~ManagedIDPolicy() = default;

  //    static constexpr size_t allocate(const T& t)
  //    {
  //  	access.emplace_back(container.size());
  //  	container.push_back(std::forward<const T&>(t));
  //  	return access.size() - 1;
  //    }
  //    static constexpr size_t allocate(T&& t)
  //    {
  //  	access.emplace_back(container.size());
  //  	container.emplace_back(std::move(t));
  //  	return access.size() - 1;
  //    }
  //    static constexpr void free(const size_t& i)
  //    {
  //  	printf("Freeing ID %lu\n", i);
  //  	container.erase(container.begin() + access[i]);
  //  	access[i] = INVALID_ID;
  //  	for (size_t it = i + 1; it < access.size(); ++it) {
  //  	  --access[it];
  //  	}
  //    }
  //    static void clear()
  //    {
  //  	access.clear();
  //    }
  //    ID_Type<T>& get() const
  //    {
  //  	printf("Accessing ID %lu\n", index);
  //  	assert(index < access.size());
  //  	assert(access[index] < container.size());
  //  	return container[access[index]];
  //    }
  //  };

  //template<typename T, std::vector<T>& container>
  //  std::vector<size_t> ManagedIDPolicy<T, container>::access = std::vector<size_t>();

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


  template<typename T>
	class UnmanagedPolicy;

  template<typename T, template<class> class Management = UnmanagedPolicy>
	  struct ID : public Management<T>::ID
	  {
		using Preset = T;
		using Type = ID_Type<T>;
		using Management<T>::ID::get;
		using Management<T>::ID::container;

		constexpr ID(const size_t i, typename Management<T>::ContainerType& _c)
		  : Management<T>::ID(i, _c)
		{}
		constexpr ID(typename Management<T>::ContainerType& _c)
		  : Management<T>::ID(_c)
		{}

		Type* operator->() const
		{
		  return &get();
		}
		Type& operator*() const
		{
		  return get();
		}
		operator size_t() const
		{
		  return Management<T>::ID::index;
		}
		constexpr inline bool operator==(const ID<T, Management>& o) const
		{
		  return Management<T>::ID::index == o.index && container == o.container;
		}
		constexpr inline bool operator!=(const ID<T, Management>& o) const
		{
		  return Management<T>::ID::index != o.index || container != o.container;
		}
	  };

  template<typename T, template<class> class Management = UnmanagedPolicy>
	struct Container : public std::vector<T>, Management<T>::Container
	{
	  using ID = ID<T>;
	  using Management<T>::Container::allocate;
	  Container()
		: Management<T>::Container(*this)
	  {}
	  void clear()
	  {
		Management<T>::Container::clear();
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
	  struct Container
	  {
		ContainerType* container;
		constexpr Container(ContainerType& _c)
		  : container(&_c)
		{}
		constexpr IDType allocate(const T& t) const
		{
		  container->push_back(t);
		  return IDType(container->size() - 1, *container);
		}
		constexpr IDType allocate(T&& t) const
		{
		  container->emplace_back(t);
		  return IDType(container->size() - 1, *container);
		}
		void clear()
		{
		}
		Container() {}
	  };
	  struct ID
	  {
		ID() {}
		constexpr ID(ContainerType& _c)
		  : container(&_c)
			, index(INVALID_ID)
		{}
		constexpr ID(const size_t i, ContainerType& _c)
		  : container(&_c)
			, index(i)
		{}
		ID_Type<T>& get() const
		{
		  assert(index < container->size());
		  return (*container)[index];
		}
		ContainerType* container;
		size_t index;
	  };
	};

}
