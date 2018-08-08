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

    template<typename... Ts>
    struct ButtonSignals
    {
        using SignalType = SignalListener<And, SignalListener<Or, typename Ts::SignalType...>,
              SignalListener<Nor, typename Ts::SignalType...>>;
        using HoldSignalType = SignalListener<Flip, SignalType, SignalType>;
        template<size_t... Ns>
            constexpr SignalType gen_event(const bool state, const std::tuple<Ts...> elems, const std::index_sequence<Ns...>) const
            {
                return ifAll(
                        ifAny(std::get<Ns>(elems).event(state)...),
                        ifNone(std::get<Ns>(elems).event(!state)...));
            }
        constexpr ButtonSignals()
        {}
        constexpr ButtonSignals(const std::tuple<Ts...> os)
            : onSignal(gen_event(true, os, std::make_index_sequence<sizeof...(Ts)>()))
            , offSignal(gen_event(false, os, std::make_index_sequence<sizeof...(Ts)>()))
        {}
        constexpr const SignalType event(const bool state) const
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
            return signals::flip(onSignal, offSignal);
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
    template<typename T>
    struct ButtonSignals<T>
    {
        using SignalType = utils::ID<EventListener<Event<T, bool>>>;
        using HoldSignalType = SignalListener<Flip, SignalType, SignalType>;
        constexpr ButtonSignals()
        {}
        constexpr ButtonSignals(const T e)
            : onSignal(ifEvent(Event<T, bool>(e, true)))
            , offSignal(ifEvent(Event<T, bool>(e, false)))
        {}
        constexpr ButtonSignals(const std::tuple<T> e)
            : onSignal(ifEvent(Event<T, bool>(std::get<0>(e), true)))
            , offSignal(ifEvent(Event<T, bool>(std::get<0>(e), false)))
        {}
        constexpr const SignalType event(const bool state) const
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
            return signals::flip(onSignal, offSignal);
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

