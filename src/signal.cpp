#include "signal.h"

std::vector<void(*)()> signals::hidden::eventCheckFuncs;
std::vector<void(*)()> signals::hidden::signalClearFuncs;
std::vector<std::pair<signals::Listener, signals::Invoker>> signals::hidden::links;

void signals::processLinks()
{
    for (std::pair<Listener, Invoker>& link : hidden::links) {
        if (link.first.stat()) {
            link.second.invoke();
        }
    }
}

void signals::hidden::link(Listener pListener, Invoker pInvoker)
{
    links.push_back(std::make_pair(pListener, pInvoker));
}

void signals::checkEvents()
{
    for (void(*&check)() : hidden::eventCheckFuncs) {
        check();
    }
}

void signals::clearSignals()
{
    for (void(*&clear)() : hidden::signalClearFuncs) {
        clear();
    }
}
