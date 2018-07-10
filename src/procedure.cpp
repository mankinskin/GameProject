#include "procedure.h"


void signals::clearProcedures()
{
}

void signals::runReactions()
{
    for( const Reaction& reaction : Reaction::all ) {
        if ( reaction.listener.stat() ) {
            reaction.invoke();
        }
    }
}
