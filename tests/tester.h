#include <cassert>

namespace Tester
{
    const char* text_color_green = "\e[32m";
    const char* text_color_red = "\e[31m";
    const char* text_color_none = "\e[39m";

    inline void pass()
    {
        printf("%sPASS%s\n", text_color_green, text_color_none);
    }

    inline void fail()
    {
        printf("%sFAIL%s\n", text_color_red, text_color_none);
    }
}
