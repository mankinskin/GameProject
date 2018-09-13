#include <cstdio>
#include "../src/utils/id.h"
#include "tester.h"

using namespace utils;
void test_unmanaged()
{
  puts("Container Unmanaged");
  // test unmanaged Container
  // unmanaged IDs should
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
}
int main()
{
  test_unmanaged();
  test_managed();
  return 0;
}
