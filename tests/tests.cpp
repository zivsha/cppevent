#include "catch.hpp"
#include <cppevent\cppevent.hpp>
#include <functional>

using namespace cppevent;

#define CONCAT1(x, y) x ## y
#define CONCAT2(x, y) CONCAT1(x,y)
#define RAND_VAR_NAME CONCAT2(signal_, __LINE__)

float foo(float) { return 0.f; }
void bar(int, int) { }
struct fooclass
{
    static int foo(double) { return 0; }
    char bar() { return 'a'; }
};

TEST_CASE("Compilation Check")
{
    SECTION("Declarations")
    {
        signal<void()>                  RAND_VAR_NAME; RAND_VAR_NAME();
        signal<void(void)>              RAND_VAR_NAME; RAND_VAR_NAME();
        signal<void(int)>               RAND_VAR_NAME; RAND_VAR_NAME(1);
        signal<void(float)>             RAND_VAR_NAME; RAND_VAR_NAME(1.0f);
        signal<void(double)>            RAND_VAR_NAME; RAND_VAR_NAME(1.0);
        signal<void(char)>              RAND_VAR_NAME; RAND_VAR_NAME('c');
        signal<void(void*)>             RAND_VAR_NAME; RAND_VAR_NAME((void*)0x0);
        signal<void(char*)>             RAND_VAR_NAME; RAND_VAR_NAME("c");
        signal<void(const char*)>       RAND_VAR_NAME; RAND_VAR_NAME("123");
        signal<void(const char* const)> RAND_VAR_NAME; RAND_VAR_NAME("123");
        signal<void(char* const)>       RAND_VAR_NAME; RAND_VAR_NAME("123");
        signal<void(int**)>             RAND_VAR_NAME; RAND_VAR_NAME((int**)0x0);

    }
    SECTION("Lambda")
    {
        signal<void()> s;
        s += []() {};
    }

    SECTION("Functor")
    {
        struct functor
        {
            void operator()() const {}
        };
        signal<void()> s;
        s += functor();
        functor f;
        s += f;
        const functor cf;
        s += cf;
        s();
    }

    SECTION("Function Pointer")
    {
        signal<float(float)> s;
        s += foo;
        std::function<float(float)> func = foo;
        s(0.f);
    }

    SECTION("Static Functions")
    {
        signal<int(double)> s;
        s += fooclass::foo;
        s(0.1);
    }

    SECTION("Free Functions")
    {

    }

    SECTION("Member Functions")
    {
        signal<char(fooclass*)> s;
        s += &fooclass::bar;
        fooclass f;
        s(&f);
    }

    SECTION("std::bind")
    {
        signal<void(int)> s;
        s += std::bind(bar, 0, std::placeholders::_1);
        s(5);
    }
}

