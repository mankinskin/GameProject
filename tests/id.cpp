#include <cstdio>
#include "../src/utils/id.h"
#include "tester.h"

using namespace utils;
void test_unmanaged()
{
  puts("Container Unmanaged");
  using Container = Container<float, UnmanagedPolicy>;
  using ID = typename Container::ID;
  Container container;
  log::test(container.size() == 0, "container is empty if initialized empty");
  ID idinv(&container);
  try {
	float inv = *idinv;
	log::fail("dereferencing uninitialized id yields exception");
  } catch (...) {
	log::pass("dereferencing uninitialized id yields exception");
  }
  const float sample1 = 1.9f;
  ID id1(container.makeID(sample1));
  log::test(container.size() == 1, "makeID creates object in container");
  try {
	log::test(*id1 == sample1, "dereferencing id yields stored data");
  } catch (std::exception e) {
	log::fail(e.what());
  } catch (...) {
	log::fail();
  }
  const float sample2 = 2.9f;
  ID id2(container.makeID(sample2));
  log::test(container.size() == 2, "makeID creates object in container");
  try {
	log::test(*id2 == sample2, "dereferencing id yields stored data");
	log::test(*id1 == sample1, "dereferencing previous id yields stored data");
  } catch (std::exception e) {
	log::fail(e.what());
  } catch (...) {
	log::fail();
  }
  ID id1p(0, &container);
  try {
	log::test(*id1p == sample1, "dereferencing index initialized id yields stored data");
  } catch (std::exception e) {
	log::fail(e.what());
  } catch (...) {
	log::fail();
  }
}
void test_managed()
{
  puts("Container Managed");
  using Container = Container<float, ManagedPolicy>;
  using ID = typename Container::ID;
  Container container;
  log::test(container.size() == 0, "container is empty if initialized empty");
  ID idinv(&container);
  try {
	float inv = *idinv;
	log::fail("dereferencing uninitialized id yields exception");
  } catch (...) {
	log::pass("dereferencing uninitialized id yields exception");
  }
  const float sample1 = 1.9f;
  ID id1(container.makeID(sample1));
  log::test(container.size() == 1, "makeID creates object in container");
  try {
	log::test(*id1 == sample1, "dereferencing id yields stored data");
  } catch (std::runtime_error e) {
	log::fail(e.what());
  }
  const float sample2 = 2.9f;
  ID id2(container.makeID(sample2));
  log::test(container.size() == 2, "makeID creates object in container");
  try {
	log::test(*id2 == sample2, "dereferencing id yields stored data");
  } catch (std::runtime_error e) {
	log::fail(e.what());
  }
  const float sample3 = 3.5f;
  ID id3(container.makeID(sample3));
  log::test(container.size() == 3, "makeID creates object in container");
  try {
	log::test(*id3 == sample3, "dereferencing id yields stored data");
	log::test(*id2 == sample2, "dereferencing previous id yields stored data");
	log::test(*id1 == sample1, "dereferencing previous id yields stored data");
  } catch (std::runtime_error e) {
	log::fail(e.what());
  }
  ID id1p(0, &container);
  try {
	log::test(*id1p == sample1, "dereferencing index initialized id yields stored data");
  } catch (std::runtime_error e) {
	log::fail(e.what());
  }

  id1.free();;
  log::test(container.size() == 2, "free() deletes object from container");
  try {
	float inv = *id1;
	log::fail("dereferencing free id yields exception");
  } catch (...) {
	log::pass("dereferencing free id yields exception");
  }
  try {
	log::test(*id2 == sample2, "dereferencing previous id yields stored data");
	log::test(*id3 == sample3, "dereferencing previous id yields stored data");
  } catch (std::runtime_error e) {
	log::fail(e.what());
  }
}
int main()
{
  test_unmanaged();
  test_managed();
  return 0;
}
