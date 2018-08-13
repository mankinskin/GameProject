#include "signal.h"

std::vector<void(*)()> signals::eventCheckFuncs;
std::vector<void(*)()> signals::signalClearFuncs;
std::vector<std::pair<const signals::Listener, const signals::Invoker>> signals::links;

void signals::processLinks()
{
    for (const std::pair<const Listener, const Invoker>& link : links) {
        if (link.first.stat()) {
            link.second.invoke();
        }
    }
    Signal::reset();
}

void signals::link(const Listener pListener, const Invoker pInvoker)
{
    links.push_back(std::make_pair(pListener, pInvoker));
}

void signals::checkEvents()
{
    for (void(*&check)() : eventCheckFuncs) {
        check();
    }
}

void signals::clearSignals()
{
    for (void(*&clear)() : signalClearFuncs) {
        clear();
    }
}
