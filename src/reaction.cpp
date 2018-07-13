#include "reaction.h"

std::vector<std::pair<signals::Listener, signals::hidden::Invoker>> signals::hidden::links;

void signals::processReactions()
{
    for (std::pair<Listener, hidden::Invoker>& link : hidden::links) {
        if (link.first.stat()) {
            link.second.invoke();
        }
    }
}
