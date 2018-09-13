#include <cassert>
#include <string>
namespace log
{
  const char* text_color_green = "\e[32m";
  const char* text_color_red = "\e[31m";
  const char* text_color_none = "\e[39m";

  inline void msg(const std::string s)
  {
	printf("%s\n", s.c_str());
  }
  inline void pass()
  {
	printf("%sPASS%s\n", text_color_green, text_color_none);
  }
  inline void fail()
  {
	printf("%sFAIL%s\n", text_color_red, text_color_none);
  }
  inline void pass(const std::string s)
  {
	printf("%s: ", s.c_str());
	pass();
  }
  inline void fail(const std::string s)
  {
	printf("%s: ", s.c_str());
	fail();
  }
  inline void test(bool condition, const std::string name)
  {
	if (condition)
	  pass(name);
	else
	  fail(name);
  }
}
