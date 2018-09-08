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
	using ID_Container = std::vector<T>;

  template<typename T>
	using ID_Type = typename std::remove_reference<T>::type;

  template<typename T>
	struct ID
	{
	  using Preset = T;
	  using Type = ID_Type<T>;
	  using Container = ID_Container<Type>;
	  using size_type = typename Container::size_type;
	  static ID_Container<T> container;

	  constexpr ID()
		: index(INVALID_ID)
	  {}

	  static constexpr size_t allocate(const T& t)
	  {
		container.push_back(t);
		return container.size() - 1;
	  }
	  static constexpr size_t allocate(const T&& t)
	  {
		container.emplace_back(std::move(t));
		return container.size() - 1;
	  }
	  constexpr ID(const size_t i)
		: index(i)
	  {}
	  constexpr ID(const T& t)
		: index(allocate(t))
	  {}
	  constexpr ID(const T&& t)
		: index(allocate(t))
	  {}

	  size_type index;

	  Type& get() const
	  {
		assert(index < container.size());
		return container[index];
	  }

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
		return index;
	  }
	  static void clear()
	  {
		container.clear();
	  }
	};

  template<typename T>
	ID_Container<T> ID<T>::container = ID_Container<T>();

  template<typename T>
	struct ManagedID : public ID<T>
	{
	  using Type = typename ID<T>::Type;
	  using ID<T>::index;
	  using ID<T>::container;
	  constexpr ManagedID()
	  {}

	  static std::vector<ID<T>> access;
	  static constexpr size_t allocate(const T& t)
	  {
		size_t i = container.size();
		access.push_back(i);
		container.push_back(t);
		return i;
	  }
	  static constexpr size_t allocate(const T&& t)
	  {
		size_t i = container.size();
		access.push_back(i);
		container.emplace_back(std::move(t));
		return i;
	  }
	  constexpr ManagedID(const size_t i)
		: ID<T>(i)
	  {}
	  constexpr ManagedID(const ID<T> i)
		: ID<T>(i)
	  {}
	  constexpr ManagedID(const T t)
		: ID<T>(allocate(t))
	  {}

	  constexpr ManagedID(const ManagedID&) = delete;
	  constexpr ManagedID(ManagedID&& o) noexcept
	    : ID<T>(std::move(o.index))
	  {
	    o.index = INVALID_ID;
	  }

	  Type& get() const
	  {
		assert(index < access.size());
		assert(access[index] < container.size());
		return container[access[index]];
	  }
	  void free() const
	  {
		ID<T>& id = access[index];
		container.erase(container.begin() + id.index);
		for (size_t i = id.index + 1; i < access.size(); ++i) {
		  --access[i].index;
		}
		id = INVALID_ID;
	  }
	  static void clear()
	  {
		container.clear();
	  }
	};

  template<typename T>
	std::vector<ID<T>> ManagedID<T>::access = std::vector<ID<T>>();

  template<typename T>
	inline bool operator==(const utils::ID<T> l, const utils::ID<T> r)
	{
	  return l.index == r.index;
	}

  template<typename T>
	ID<T> makeID(const T&& t)
	{
	  return ID<T>::allocate(std::move(t));
	}
  template<typename T>
	ID<T> makeID(const T& t)
	{
	  return ID<T>::allocate(t);
	}
}
