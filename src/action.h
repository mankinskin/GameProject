#pragma once
#include "functor.h"
#include "event.h"
#include "utils/id.h"

namespace signals
{
    struct Action
    {
        using ID = utils::ID<Action>;
        static constexpr typename ID::Container& all = ID::container;

        Action( signals::FunctorID pFunctor, ListenerID pListener )
            : functor( pFunctor )
            , listener( pListener )
        {}

        void invoke() const
        {
            if ( listener.stat() ) {
                functor.invoke();
            }
        }
        signals::FunctorID functor;
        ListenerID listener;
    };

    void invokeAllActions();

}
