#include <cstdio>
#include <string>
#include <tuple>
#include <utility>
#include "functor.h"
#include "tester.h"

void func_void_void()
{
    puts("func_void_void");
}

int func_int_void()
{
    puts("func_int_void");
    return 1;
}
void func_void_int(int in)
{
    printf("func_void_int %d\n", in);
}

void func_void_constint(const int in)
{
    printf("func_void_constint %d\n", in);
}

void func_void_conststr(const int in)
{
    printf("func_void_constint %d\n", in);
}

void func_void_constintref(const int& in)
{
    printf("func_void_constintref %d\n", in);
}

void func_void_conststrref(const std::string in)
{
    printf("func_void_conststrref %s\n", in.c_str());
}

int main()
{
    using namespace signals;
    auto f1 = functor(func_int_void);
    f1->invoke();

    auto f2 = functor(func_void_int, 2);
    f2->invoke();

    const int constint = 3;
    auto f3 = functor(func_void_constint, constint);
    f3->invoke();

    const int& constintref = 4;
    auto f4 = functor(func_void_constintref, constintref);
    f4->invoke();

    const std::string& conststrref("constrstrref");
    auto f5 = functor(func_void_conststrref, conststrref);
    f5->invoke();
    Tester::pass();
    return 0;
}
