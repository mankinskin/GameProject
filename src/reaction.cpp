#include "reaction.h"

std::vector<std::pair<utils::ID<signals::Listener>, signals::hidden::Invoker>> signals::hidden::reactions;

void signals::processReactions()
{
    for (std::pair<utils::ID<Listener>, hidden::Invoker>& link : hidden::reactions) {
        if (link.first->stat()) {
            link.second.invoke();
        }
    }
}
