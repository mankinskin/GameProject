#include "action.h"

void events::invokeAllActions()
{
    for( const Action& action : Action::all ) {
        action.invoke();
    }
}
