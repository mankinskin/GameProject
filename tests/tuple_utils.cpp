#include <stdio.h>
#include <string>
#include <iostream>
#include "../src/utils/tuple_utils.h"
#include <tuple>

#include "tester.h"

struct T
{
  static size_t constructions;
  static size_t copies;
  static size_t moves;
  static size_t destructions;
  static size_t instances;
  T()
  {
	// Constructed
	++constructions;
	++instances;
  }
  T(const T&) noexcept
  {
	// Copied
	++copies;
	++instances;
  }
  T(T&&) noexcept
  {
	// Moved
	++moves;
	++instances;
  }
  ~T()
  {
	// Destructed
	++destructions;
	--instances;
  }
};
size_t T::constructions = 0;
size_t T::copies = 0;
size_t T::moves = 0;
size_t T::destructions = 0;
size_t T::instances = 0;

struct C : public T
{
  C(int pa, char pb, float pc)
	: a(pa)
	, b(pb)
	, c(pc)
  {}
  int a;
  char b;
  float c;
};
struct M : public T
{
  M(int pa, char pb, float pc)
	: a(pa)
	, b(pb)
	, c(pc)
  {}
  M(const M&) = delete;
  M(M&&) = default;
  int a;
  char b;
  float c;
};

T create()
{
  return T();
}

void test_converter()
{
  std::tuple<int, int, int> ints({0, 1, 2});
  std::tuple<char, char, char> chars({'a', 'b', 'c'});
  std::tuple<float, float, float> floats({0.1f, 0.5f, 1.0f});

  std::tuple<M, M, M> multis(utils::convert_tuple<M, M, M>(ints, chars, floats));

  printf("Ctor:\t%lu\nCpy:\t%lu\nMv:\t%lu\nDtor:\t%lu\nInst:\t%lu\n", M::constructions, M::copies, M::moves, M::destructions, M::instances);
}

int main()
{
  test_converter();
  return 0;
}


