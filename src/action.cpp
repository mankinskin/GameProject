#include "action.h"

void signals::invokeAllActions()
{
    for( const Action& action : Action::all ) {
        action.invoke();
    }
}
