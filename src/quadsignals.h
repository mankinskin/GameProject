#pragma once
#include "signal.h"
#include "buttonsignals.h"
#include "mouse.h"

namespace signals
{
    template<typename ObjectType, typename StateType = bool>
    struct QuadSignals : public ButtonSignals<ObjectType, StateType>
    {
        using BaseSignals = ButtonSignals<ObjectType, StateType>;
        using EventType = typename BaseSignals::EventType;
        using SignalType = typename BaseSignals::SignalType;
        using HoverType = typename BaseSignals::HoldType;
        using HoldType = signals::SignalListener<signals::Flip, signals::SignalListener<signals::And, HoverType, signals::ButtonSignals<input::MouseKey>::SignalType>, signals::ButtonSignals<input::MouseKey>::SignalType>;
        using PressType = signals::SignalListener<signals::Clicker, typename HoldType::Set, typename HoldType::Reset>;
        using ReleaseType = signals::SignalListener<signals::Clicker, typename HoldType::Set, typename HoldType::Reset>;

        constexpr QuadSignals(const ObjectType o)
            : BaseSignals(o)
            , enter(BaseSignals::on)
            , leave(BaseSignals::off)
            , hover(BaseSignals::hold)
            , hold(flip(ifAll(hover, input::Mouse::lmb->on), input::Mouse::lmb->off))
            , press(clicker(hold, true))
            , release(clicker(hold, false))
        {}

        const SignalType& enter = signals::ButtonSignals<ObjectType, StateType>::on;
        const SignalType& leave = signals::ButtonSignals<ObjectType, StateType>::off;
        const HoverType& hover = signals::ButtonSignals<ObjectType, StateType>::hold;
        const HoldType hold;
        const PressType press;
        const ReleaseType release;
    };
}
