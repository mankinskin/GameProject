#pragma once
#include "utils/id.h"
#include "utils/id.h"
#include "functor.h"
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

                    constexpr bool stat_n(utils::_index<0>)
                    {
                        return std::get<sizeof...(Signals) - 1>(signals)->stat();
                    }
                    template<size_t N>
                        constexpr bool stat_n(utils::_index<N>)
                        {
                            return std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() == stat_n(utils::_index<N-1>());
                        }
                    constexpr bool stat()
                    {
                        return stat_n(utils::_index<sizeof...(Signals) - 1>());
                    }
                private:
                    const std::tuple<const Signals...> signals;
            };

        extern std::vector<void(*)()> signalClearFuncs;
        extern std::vector<void(*)()> eventCheckFuncs;
        extern std::vector<void(*)()> eventInstanceClearFuncs;

        template<typename Event>
            class EventListener
            {
                public:
                using ID = utils::ID<EventListener<Event>>;
                static constexpr typename ID::Container& all = ID::container;
                    EventListener(const Event pSignature)
                        : signature(pSignature)
                          , occurred(false)
                {
                    initialize();
                }

                    static void pushEvent(const Event& pEvent)
                    {
                        eventBuffer.emplace_back(pEvent);
                    }

                    bool stat() const
                    {
                        return occurred;
                    }

                    static bool stat(size_t i)
                    {
                        return all[i].stat();
                    }
                    static void clear()
                    {
                        all.clear();
                    }

                    const EventListener* operator->() const
                    {
                        this;
                    }

                    const Event signature;
                    bool occurred = false;

                private:
                    static std::vector<Event> eventBuffer;
                    static void check()
                    {
                        for (EventListener<Event>& instance : all) {
                            instance.occurred = false;
                            for (const Event& event : EventListener<Event>::eventBuffer) {
                                if(instance.signature == event) {
                                    instance.occurred = true;
                                    break;
                                }
                            }
                        }
                        eventBuffer.clear();
                    }
                    static void initialize()
                    {
                        static At_Init init;
                    }

                    struct At_Init
                    {
                        At_Init()
                        {
                            eventCheckFuncs.push_back(&check);
                            signalClearFuncs.push_back(&clear);
                        }
                    };
            };

        template<typename Event>
            std::vector<Event> EventListener<Event>::eventBuffer = std::vector<Event>();

        template<template<typename...> typename Op, typename... Signals>
            class SignalListener : public Op<Signals...>
        {
            public:
                using ID = utils::ID<SignalListener<Op, Signals...>>;
                static constexpr typename ID::Container& all = ID::container;
                SignalListener(const Signals... sigs)
                    : Op<Signals...>(sigs...)
                {
                    initialize();
                }

                constexpr const Op<Signals...>* operator->() const
                {
                    this;
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
                        At_Init()
                        {
                            signalClearFuncs.push_back(&SignalListener<O, S...>::clear);
                        }
                    };
        };

    template<typename Event>
        utils::ID<EventListener<Event>> ifEvent(const Event pEvent)
        {
            return utils::makeID(EventListener<Event>(pEvent));
        }

    template<typename... Signals>
        SignalListener<And, Signals...> ifAll(const Signals... sigs)
        {
            return SignalListener<And, Signals...>(sigs...);
        }

    template<typename... Signals>
        SignalListener<Or, Signals...> ifAny(const Signals... sigs)
        {
            return SignalListener<Or, Signals...>(sigs...);
        }

    template<typename... Signals>
        SignalListener<Xor, Signals...> ifNotEqual(const Signals... sigs)
        {
            return SignalListener<Xor, Signals...>(sigs...);
        }

    template<typename... Signals>
        SignalListener<Nor, Signals...> ifNone(const Signals... sigs)
        {
            return SignalListener<Nor, Signals...>(sigs...);
        }

    template<typename... Signals>
        SignalListener<Equal, Signals...> ifEqual(const Signals... sigs)
        {
            return SignalListener<Equal, Signals...>(sigs...);
        }

    struct Listener
    {
        using ID = utils::ID<Listener>;
        constexpr static typename ID::Container& all = ID::container;

        template<typename Event>
            Listener(const utils::ID<EventListener<Event>> pListener)
            : stater(EventListener<Event>::stat)
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
            stater(index);
        }

        bool(&stater)(const size_t);
        const size_t index;
        private:
    };

    template<typename Event>
        void pushEvent(const Event&& pEvent)
        {
            EventListener<Event>::pushEvent(pEvent);
        }

    template<typename Event>
        utils::ID<Listener> listen(const utils::ID<EventListener<Event>> pEvent)
        {
            return utils::makeID(Listener(pEvent));
        }
    template<typename Event>
        utils::ID<Listener> listen(const Event pEvent)
        {
            return listen(utils::makeID(EventListener<Event>(pEvent)));
        }

    template<template<typename...> typename Op, typename... Signals>
        utils::ID<Listener> listen(const SignalListener<Op, Signals...> pSignal)
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
}

