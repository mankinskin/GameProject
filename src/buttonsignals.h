#pragma once
#include "signal.h"

namespace signals
{
    template<typename ObjectType, typename StateType = bool>
    struct ButtonSignals
    {
        using EventType = Event<ObjectType, StateType>;
        using SignalType = utils::ID<EventListener<EventType>>;
        using HoldType = SignalListener<Flip, SignalType, SignalType>;

        constexpr ButtonSignals(const ObjectType o)
            : on(ifEvent(EventType(o, true)))
            , off(ifEvent(EventType(o, false)))
            , hold(flip(on, off))
        {}

        constexpr const SignalType event(utils::_bool<true>) const
        {
            return on;
        }

        constexpr const SignalType event(utils::_bool<false>) const
        {
            return off;
        }

        const SignalType on;
        const SignalType off;
        const HoldType hold;
    };

}
