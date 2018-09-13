#include <stdio.h>
#include <string>
#include <iostream>
#include "../src/utils/id.h"
#include "../src/signal.h"

#include "tester.h"

using namespace signals;

void test_flip()
{
    utils::ID<State> setter = State::all.makeID(State());
    utils::ID<State> resetter = State::all.makeID(State());
    auto flipflop = flip(setter, resetter);
    bool running = true;
    size_t test = 0;
    while (running) {
        switch (test++) {
            case 0:
                assert(flipflop.stat() == false && "flipflop starts false");
                break;
            case 1:
                setter->set(true);
                assert(flipflop.stat() == true && "flipflop is set to true by setter");
                setter->set(false);
                break;
            case 2:
                assert(flipflop.stat() == true && "flipflop remains true");
                assert(flipflop.stat() == true && "flipflop remains true");
                assert(flipflop.stat() == true && "flipflop remains true");
                break;
            case 3:
                resetter->set(true);
                assert(flipflop.stat() == false && "flipflop is set to false by resetter");
                resetter->set(false);
                break;
            case 4:
                assert(flipflop.stat() == false && "flipflop remains false");
                assert(flipflop.stat() == false && "flipflop remains false");
                assert(flipflop.stat() == false && "flipflop remains false");
                break;
            case 5:
                setter->set(true);
                resetter->set(true);
                assert(flipflop.stat() == false && "flipflop does not change if both setter and resetter are true");
                resetter->set(false);
                setter->set(false);
                break;
            case 6:

                break;
            default:
                running = false;
        };
        checkEvents();
        processLinks();
        clearSignals();
        Tester::pass();
    }
}
void test_clicker()
{
    utils::ID<State> setter = State::all.makeID(State());
    utils::ID<State> resetter = State::all.makeID(State());
    auto flipflop = flip(setter, resetter);
    auto click_on = clicker(flipflop, true);
    auto click_off = clicker(flipflop, false);
    bool running = true;
    size_t test = 0;
    while (running) {
        switch (test++) {
            case 0:
                assert(click_on.stat() == false && "clicker(true) starts false");
                assert(click_off.stat() == false && "clicker(false) starts false");
                break;
            case 1:
                setter->set(true);
                assert(click_on.stat() == true && "click(true) is set to true if its flip is flipped on");
                setter->set(false);
                break;
            case 2:
                assert(click_on.stat() == false && "click(true) is set off in the next frame");
                break;
            case 3:
                resetter->set(true);
                assert(click_off.stat() == true && "click(false) is set to true if its flip is flipped off");
                resetter->set(false);
                break;
            case 4:
                assert(click_off.stat() == false && "click(false) is set off in the next frame");
                break;
            case 5:
                setter->set(true);
                resetter->set(true);
                assert(click_on.stat() == false && "click(true) does not change if both setter and resetter are true");
                assert(click_off.stat() == false && "click(false) does not change if both setter and resetter are true");
                resetter->set(false);
                setter->set(false);
                break;
            case 6:
                setter->set(true);
                assert(click_on.stat() == true && "click(true) is set to true if its flip is flipped on");
                setter->set(false);
                break;
            case 7:
                resetter->set(true);
                assert(click_on.stat() == false && "click(true) is set off in the next frame");
                assert(click_off.stat() == true && "click(false) is set to true if its flip is flipped off");
                resetter->set(false);
                break;
            case 8:
                assert(click_off.stat() == false && "click(false) is set off in the next frame");
                break;
            default:
                running = false;
        };
        checkEvents();
        processLinks();
        clearSignals();
        Tester::pass();
    }
}

int main()
{
    try {
    test_flip();
    test_clicker();
    } catch (...) {
        Tester::fail();
    }
    return 0;
}
