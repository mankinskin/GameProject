#include "reaction.h"

std::vector<std::pair<signals::Listener, signals::hidden::Invoker>> signals::hidden::reactions;

void signals::processReactions()
{
    for (std::pair<Listener, hidden::Invoker>& link : hidden::reactions) {
        if (link.first.stat()) {
            link.second.invoke();
        }
    }
}
