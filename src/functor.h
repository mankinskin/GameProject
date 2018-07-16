#pragma once
#include <tuple>
#include <vector>
#include <utility>
#include "utils/id.h"
#include "utils/tuple_utils.h"
#include <type_traits>

namespace signals
{
    namespace hidden
    {
        extern std::vector<void(*)()> functorClearFuncs;
        template<typename F, typename... Args>
            class Functor
            {
                public:
                    using ID = utils::ID<Functor<F, Args...>>;
                    constexpr static typename ID::Container& all = ID::container;

                    Functor(F&& pF, Args&&... pArgs)
                        : func(std::forward<F>(pF))
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

                    static void invoke(const size_t i)
                    {
                        all[i].invoke();
                    }

                    static void clear()
                    {
                        all.clear();
                    }

                private:
                    F func;
                    const std::tuple<Args...> args;
                    struct At_Init
                    {
                        At_Init()
                        {
                            functorClearFuncs.push_back(&Functor<F, Args...>::clear);
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
        template<typename F, typename... Args>
            Invoker(utils::ID<hidden::Functor<F, Args...>> pFunc)
            : invoker(hidden::Functor<F, Args...>::invoke)
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
        //void operator()() const
        //{
        //    invoker(index);
        //}

        void(&invoker)(const size_t);
        const size_t index;
    };

    // allocates a functor and returns an ID for it
    template<typename F, typename... Args>
        constexpr utils::ID<hidden::Functor<F, Args...>> functor(F&& pF, Args&&... pArgs)
        {
            return utils::makeID(hidden::Functor<F, Args...>(std::forward<F>(pF), std::forward<Args>(pArgs)...));
        }

    template<typename... Funcs>
        constexpr utils::ID<hidden::Procedure<Funcs...>> procedure(utils::ID<Funcs>... pFuncs)
        {
            return utils::makeID(hidden::Procedure<Funcs...>(pFuncs...));
        }

    void clearFunctors();
}

