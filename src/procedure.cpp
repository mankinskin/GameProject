#include "procedure.h"


void signals::clearProcedures()
{
    for ( void( *&destructor )() : destructors ) {
        destructor();
    }
}

void signals::runReactions()
{
    for( const Reaction& reaction : Reaction::all ) {
        if ( reaction.listener.stat() ) {
            reaction.invoke();
        }
    }
}
