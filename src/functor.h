#pragma once
#include <tuple>
#include <vector>
#include <utility>
#include "utils/id.h"
#include "utils/tuple_utils.h"

namespace signals
{
    namespace hidden
    {
        extern std::vector<void(*)()> functorClearFuncs;
        template<typename R, typename... Args>
            class Functor
            {
                public:
                    using ID = utils::ID<Functor<R, Args...>>;
                    constexpr static typename ID::Container& all = ID::container;

                    Functor(R(&&pF)(Args...), Args&&... pArgs)
                        : func(std::forward<R(Args...)>(pF))
                          , args(std::forward<Args>(pArgs)...)
                {
                    initialize();
                }

                    template<size_t... Ns>
                        constexpr void invoke_unpacked(std::index_sequence<Ns...>)
                        {
                            func(std::get<Ns>(args)...);
                        }
                    constexpr void invoke()
                    {
                        invoke_unpacked(std::make_index_sequence<sizeof...(Args)>());
                    }

                    static void invoke( const size_t i )
                    {
                        all[i].invoke();
                    }

                    static void clear()
                    {
                        all.clear();
                    }

                private:
                    R(&func)(Args...);
                    const std::tuple<Args...> args;
                    struct At_Init
                    {
                        At_Init()
                        {
                            functorClearFuncs.push_back(&Functor<R, Args...>::clear);
                        }
                    };
                    static void initialize()
                    {
                        static At_Init init;
                    }
            };

        template<typename... Funcs>
            class Procedure
            {
                public:
                    using ID = utils::ID<Procedure<Funcs...>>;
                    constexpr static typename ID::Container& all = ID::container;

                    Procedure(utils::ID<Funcs>... fs)
                        : funcs(fs...)
                    {
                        initialize();
                    }

                    constexpr void invoke_n(utils::_index<0>) const
                    {}

                    template<size_t N>
                        constexpr void invoke_n(utils::_index<N>) const
                        {
                            invoke_n(utils::_index<N-1>());
                            std::get<N-1>(funcs)->invoke();
                        }

                    constexpr void invoke()
                    {
                        invoke_n(utils::_index<sizeof...(Funcs)>());
                    }

                    static void invoke(const size_t i)
                    {
                        all[i].invoke();
                    }

                    static void clear()
                    {
                        all.clear();
                    }

                private:
                    std::tuple<utils::ID<Funcs>...> funcs;
                    struct At_Init
                    {
                        At_Init()
                        {
                            functorClearFuncs.push_back(&Procedure<Funcs...>::clear);
                        }
                    };
                    static void initialize()
                    {
                        static At_Init init;
                    }
            };
    }

    struct Invoker
    {
        template<typename R, typename... Args>
            Invoker(R(&&pF)(Args...), Args&&... pArgs)
            : invoker(hidden::Functor<R, Args...>::invoke)
              , index(functor(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...).index)
        {}

        template<typename... Funcs>
            Invoker(utils::ID<Funcs>... pFuncs)
            : invoker(hidden::Procedure<Funcs...>::invoke)
              , index(procedure(pFuncs...).index)
        {}

        template<typename R, typename... Args>
            Invoker(utils::ID<hidden::Functor<R, Args...>> pFunc)
            : invoker(hidden::Functor<R, Args...>::invoke)
              , index(pFunc.index)
        {}

        template<typename... Funcs>
            Invoker(utils::ID<hidden::Procedure<Funcs...>> pProc)
            : invoker(hidden::Procedure<Funcs...>::invoke)
              , index(pProc.index)
        {}

        void invoke() const
        {
            invoker(index);
        }

        void(&invoker)(const size_t);
        const size_t index;
    };
    // allocates a functor and returns an ID for it
    template<typename R, typename... Args>
        utils::ID<hidden::Functor<R, Args...>> functor(R(&&pF)(Args...), Args&&... pArgs)
        {
            return utils::makeID(hidden::Functor<R, Args...>(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...));
        }

    template<typename... Funcs>
        utils::ID<hidden::Procedure<Funcs...>> procedure(utils::ID<Funcs>... pFuncs)
        {
            return utils::makeID(hidden::Procedure<Funcs...>(pFuncs...));
        }

    void clearFunctors();
}

