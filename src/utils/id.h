#pragma once
#include <vector>
#include <functional>
#include <type_traits>
#include <assert.h>

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

	  constexpr ID(const size_t i)
		: index(i)
	  {}
	  constexpr ID(const T t)
		: index(container.size())
	  {
		puts("Creating ID from object...");
		container.push_back(t);
	  }

	  size_type index;

	  Type& get() const
	  {
		assert(container.size());
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
	inline bool operator==(const utils::ID<T> l, const utils::ID<T> r)
	{
	  return l.index == r.index;
	}

  template<typename T>
	ID<T> makeID(const T& t)
	{
	  size_t id = ID<T>::container.size();
	  ID<T>::container.push_back(t);
	  return ID<T>(id);
	}
}
