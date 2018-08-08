#pragma once
#include "utils/id.h"
#include "utils/id.h"
#include "functor.h"
#include "event.h"
#include <tuple>

namespace signals
{
        // plug these into signal listeners
        template<typename... Signals>
            class And
            {
                public:
                    And(const Signals... sigs)
                        : signals(sigs...)
                    {}

                    constexpr bool stat_n(utils::_index<0>) const
                    {
                        return std::get<sizeof...(Signals) - 1>(signals)->stat();
                    }
                    template<size_t N>
                        constexpr bool stat_n(utils::_index<N>) const
                        {
                            return std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() && stat_n(utils::_index<N-1>());
                        }
                    constexpr bool stat() const
                    {
                        return stat_n(utils::_index<sizeof...(Signals) - 1>());
                    }

                    constexpr const And* operator->() const
                    {
                        return this;
                    }
                private:
                    const std::tuple<const Signals...> signals;
            };

        template<typename... Signals>
            class Or
            {
                public:
                    Or(const Signals... sigs)
                        : signals(sigs...)
                    {}

                    constexpr bool stat_n(utils::_index<0>) const
                    {
                        return std::get<sizeof...(Signals) - 1>(signals)->stat();
                    }
                    template<size_t N>
                        constexpr bool stat_n(utils::_index<N>) const
                        {
                            return std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() || stat_n(utils::_index<N-1>());
                        }
                    constexpr bool stat() const
                    {
                        return stat_n(utils::_index<sizeof...(Signals) - 1>());
                    }
                    constexpr const Or* operator->() const
                    {
                        return this;
                    }
                private:
                    const std::tuple<const Signals...> signals;
            };

        template<typename... Signals>
            class Xor
            {
                public:
                    Xor(const Signals... sigs)
                        : signals(sigs...)
                    {}

                    constexpr bool stat_n(utils::_index<0>) const
                    {
                        return false;
                    }
                    template<size_t N>
                        constexpr bool stat_n(utils::_index<N>) const
                        {
                            return !((std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() ==
                                        std::get<(sizeof...(Signals) - 1) - (N - 1)>(signals)->stat())
                                    && !stat_n(utils::_index<N-1>()));
                        }
                    constexpr bool stat() const
                    {
                        return stat_n(utils::_index<sizeof...(Signals) - 1>());
                    }
                    constexpr const Xor* operator->() const
                    {
                        return this;
                    }
                private:
                    const std::tuple<const Signals...> signals;
            };

        template<typename... Signals>
            class Nor
            {
                public:
                    Nor(const Signals... sigs)
                        : signals(sigs...)
                    {}

                    constexpr bool stat_n(utils::_index<0>) const
                    {
                        return !std::get<sizeof...(Signals) - 1>(signals)->stat();
                    }
                    template<size_t N>
                        constexpr bool stat_n(utils::_index<N>) const
                        {
                            return !std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() && stat_n(utils::_index<N-1>());
                        }
                    constexpr bool stat() const
                    {
                        return stat_n(utils::_index<sizeof...(Signals) - 1>());
                    }

                    constexpr const Nor* operator->() const
                    {
                        return this;
                    }
                private:
                    const std::tuple<const Signals...> signals;
            };

        template<typename... Signals>
            class Equal
            {
                public:
                    Equal(const Signals... sigs)
                        : signals(sigs...)
                    {}

                    constexpr bool stat_n(utils::_index<0>) const
                    {
                        return std::get<sizeof...(Signals) - 1>(signals)->stat();
                    }
                    template<size_t N>
                        constexpr bool stat_n(utils::_index<N>) const
                        {
                            return std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() == stat_n(utils::_index<N-1>());
                        }
                    constexpr bool stat() const
                    {
                        return stat_n(utils::_index<sizeof...(Signals) - 1>());
                    }
                    constexpr const Equal* operator->() const
                    {
                        return this;
                    }
                private:
                    const std::tuple<const Signals...> signals;
            };

        struct Signal
        {
            Signal(bool s = false)
                : status(s)
            {}

            constexpr void set(bool s)
            {
                status = s;
            }
            constexpr bool stat() const
            {
                return status;
            }
            constexpr const Signal* operator->() const
            {
                return this;
            }
            private:
            bool status;
        };

        template<typename Set, typename Reset>
            struct Flip
            {
                Flip(const Set pA, const Reset pB, bool startAs = false)
                    : set(pA)
                    , reset(pB)
                    , on(utils::makeID(Signal(startAs)))
                    , off(utils::makeID(Signal(!startAs)))
                {}
                constexpr bool stat() const
                {
                    if (set->stat() && reset->stat())
                        return on->stat();
                    off->set(!(on->stat() || set->stat()));
                    on->set(!(off->stat() || reset->stat()));
                    return on->stat();
                }
                constexpr const Flip* operator->() const
                {
                    return this;
                }
                private:
                const utils::ID<Signal> on;
                const utils::ID<Signal> off;
                const Set set;
                const Reset reset;
            };



        template<template<typename...> typename Op, typename... Signals>
            class SignalListener : public Op<Signals...>
        {
            public:
                using ID = utils::ID<SignalListener<Op, Signals...>>;
                static constexpr typename ID::Container& all = ID::container;
                constexpr SignalListener(const Signals... sigs)
                    : Op<Signals...>(sigs...)
                {
                    initialize();
                }
                constexpr SignalListener(const Op<Signals...> op)
                    : Op<Signals...>(op)
                {
                    initialize();
                }

                constexpr Op<Signals...>* operator->() const
                {
                    return (Op<Signals...>*)(this);
                }

                static bool stat(size_t i)
                {
                    return all[i].stat();
                }
                using Op<Signals...>::stat;
                static void clear()
                {
                    all.clear();
                }

            private:
                static void initialize()
                {
                    static At_Init<Op, Signals...> init;
                }
                template<template<typename...> typename O, typename... S>
                    struct At_Init
                    {
                        constexpr At_Init()
                        {
                            signalClearFuncs.push_back(&SignalListener<O, S...>::clear);
                        }
                    };
        };


    template<typename... Signals>
        constexpr const SignalListener<And, Signals...> ifAll(const Signals... sigs)
        {
            return SignalListener<And, Signals...>(sigs...);
        }

    template<typename... Signals>
        constexpr const SignalListener<Or, Signals...> ifAny(const Signals... sigs)
        {
            return SignalListener<Or, Signals...>(sigs...);
        }

    template<typename... Signals>
        constexpr const SignalListener<Xor, Signals...> ifNotEqual(const Signals... sigs)
        {
            return SignalListener<Xor, Signals...>(sigs...);
        }

    template<typename... Signals>
        constexpr const SignalListener<Nor, Signals...> ifNone(const Signals... sigs)
        {
            return SignalListener<Nor, Signals...>(sigs...);
        }

    template<typename... Signals>
        constexpr const SignalListener<Equal, Signals...> ifEqual(const Signals... sigs)
        {
            return SignalListener<Equal, Signals...>(sigs...);
        }

    template<typename Set, typename Reset>
        constexpr const SignalListener<Flip, Set, Reset> flip(const Set set, const Reset reset, bool startAs = false)
        {
            return SignalListener<Flip, Set, Reset>(Flip<Set, Reset>(set, reset, startAs));
        }

    struct Listener
    {
        using ID = utils::ID<Listener>;
        constexpr static typename ID::Container& all = ID::container;

        template<typename E>
            Listener(const utils::ID<EventListener<E>> pListener)
            : stater(EventListener<E>::stat)
            , index(pListener.index)
        {}
        template<template<typename...> typename Op, typename... Signals>
            Listener(const utils::ID<SignalListener<Op, Signals...>> pListener)
            : stater(SignalListener<Op, Signals...>::stat)
            , index(pListener.index)
        {}
        template<template<typename...> typename Op, typename... Signals>
            Listener(const SignalListener<Op, Signals...> pListener)
            : stater(SignalListener<Op, Signals...>::stat)
            , index(utils::makeID(pListener).index)
        {}
            Listener(const utils::ID<Listener> pListener)
            : stater(pListener->stater)
            , index(pListener->index)
        {}

        bool stat() const
        {
            return stater(index);
        }

        bool(&stater)(const size_t);
        const size_t index;
        private:
    };

    template<typename E>
        const utils::ID<Listener> listen(const utils::ID<EventListener<E>> pEvent)
        {
            return utils::makeID(Listener(pEvent));
        }
    template<typename E>
        const utils::ID<Listener> listen(const E pEvent)
        {
            return listen(utils::makeID(EventListener<E>(pEvent)));
        }

    template<template<typename...> typename Op, typename... Signals>
        const utils::ID<Listener> listen(SignalListener<Op, Signals...> pSignal)
        {
            return utils::makeID(Listener(utils::makeID(pSignal)));
        }

    void processLinks();

        extern std::vector<std::pair<Listener, Invoker>> links;
        void link(Listener pListener, Invoker pInvoker);

    template<typename Signal, typename R, typename... Args>
        void link(const Signal pListener, R(&&pF)(Args...), Args&&... pArgs)
        {
            link(Listener(pListener), Invoker(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...));
        }

    template<typename Signal, typename... Funcs>
        void link(const Signal pListener, utils::ID<Funcs>... pFuncs)
        {
            link(Listener(pListener), Invoker(pFuncs...));
        }
    template<typename Signal, typename R, typename... Args>
        void link(const Signal pListener, utils::ID<Functor<R, Args...>> pFunc)
        {
            link(Listener(pListener), Invoker(pFunc));
        }

    template<typename Signal, typename... Funcs>
        void link(const Signal pListener, utils::ID<Procedure<Funcs...>> pProc)
        {
            link(Listener(pListener), Invoker(pProc));
        }
    void checkEvents();
    void clearSignals();

    template<typename ObjectType, typename StateType = bool>
    struct ButtonSignals
    {
        using EventType = Event<ObjectType, StateType>;
        using SignalType = utils::ID<EventListener<EventType>>;
        using HoldSignalType = SignalListener<Flip, SignalType, SignalType>;
        constexpr ButtonSignals()
        {}
        constexpr ButtonSignals(const ObjectType o)
            : onSignal(ifEvent(EventType(o, true)))
            , offSignal(ifEvent(EventType(o, false)))
        {}
        constexpr const SignalType event(const StateType state) const
        {
            return state ? onSignal : offSignal;
        }
        constexpr const SignalType on() const
        {
            return onSignal;
        }
        constexpr const SignalType off() const
        {
            return offSignal;
        }
        constexpr const HoldSignalType hold() const
        {
            return flip(onSignal, offSignal);
        }

        constexpr const SignalType enter() const { return onSignal; }
        constexpr const SignalType leave() const { return offSignal; }
        constexpr const SignalType down() const { return onSignal; }
        constexpr const SignalType up() const { return offSignal; }
        constexpr const SignalType press() const { return onSignal; }
        constexpr const SignalType release() const { return offSignal; }
        constexpr const HoldSignalType hover() const { return hold(); }

        protected:
        SignalType onSignal;
        SignalType offSignal;
    };
}

