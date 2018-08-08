#include <stdio.h>
#include <string>
#include <iostream>
#include "utils/id.h"
#include "signal.h"

#include "tester.h"



const std::string SET_CMD = "set";
const std::string RESET_CMD = "reset";
const std::string EXIT_CMD = "exit";

using namespace signals;

void test_flip()
{
    utils::ID<Signal> setter = utils::makeID(Signal());
    utils::ID<Signal> resetter = utils::makeID(Signal());
    auto flipflop = flip(setter, resetter);
    assert(flipflop.stat() == false && "flipflop starts false");
    setter->set(true);
    assert(flipflop.stat() == true && "flipflop is set to true by setter");
    setter->set(false);
    assert(flipflop.stat() == true && "flipflop remains true");
    assert(flipflop.stat() == true && "flipflop remains true");
    assert(flipflop.stat() == true && "flipflop remains true");
    resetter->set(true);
    assert(flipflop.stat() == false && "flipflop is set to false by resetter");
    resetter->set(false);
    assert(flipflop.stat() == false && "flipflop remains false");
    assert(flipflop.stat() == false && "flipflop remains false");
    assert(flipflop.stat() == false && "flipflop remains false");
    setter->set(true);
    resetter->set(true);
    assert(flipflop.stat() == false && "flipflop does not change if both setter and resetter are true");
    resetter->set(false);
    assert(flipflop.stat() == true && "flipflop is set to true by setter");
    setter->set(false);
    assert(flipflop.stat() == true && "flipflop remains true");
    setter->set(true);
    resetter->set(true);
    assert(flipflop.stat() == true && "flipflop does not change if both setter and resetter are true");
    assert(flipflop.stat() == true && "flipflop does not change if both setter and resetter are true");
    Tester::pass();
}

int main()
{
    test_flip();
    return 0;
}
