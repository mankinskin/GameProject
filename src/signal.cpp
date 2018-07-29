#include "signal.h"

std::vector<void(*)()> signals::eventCheckFuncs;
std::vector<void(*)()> signals::signalClearFuncs;
std::vector<std::pair<signals::Listener, signals::Invoker>> signals::links;

void signals::processLinks()
{
    for (std::pair<Listener, Invoker>& link : links) {
        if (link.first.stat()) {
            link.second.invoke();
        }
    }
}

void signals::link(Listener pListener, Invoker pInvoker)
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
