#pragma once
#include "signal.h"
#include <functional>
#include <algorithm>
#include <tuple>
#include <utility>
#include "utils/id.h"

namespace signals
{
    /*
       Each functor holds a function pointer and a tuple for the arguments.
       when a functor class is first instantited from the template, its static void call( unsigned int index ) function is pushed onto the functorInvokers.
       this function will later be used to call a functor object by its index.
       when a functor object is created, its template_index and its slot index is inserted into the order_queue, which ensures that all functors will always be called in the order in which they were created.
       when checking all functors, the functorOrder queue is used. the invoker_index is used to determine the class of the functor and the slot_index is used to find the functor object to be invoked.
       */

    template<typename R, typename... Args>
        class Functor
        {
            public:
            using ID = utils::ID<Functor<R, Args...>>;
            constexpr static typename ID::Container& all = ID::container;
                Functor(R(&&pF)(Args...), Args&&... pArgs)
                    : func(std::forward<R(Args...)>(pF))
                    , args(std::forward<Args>(pArgs)...)
                {}

                template<size_t... Ns>
                void invoke_ns(std::index_sequence<Ns...>) const
                {
                    func(std::get<Ns>(args)...);
                }
                void invoke() const
                {
                    invoke_ns(std::make_index_sequence<sizeof...(Args)>());
                }

            private:
                R(&func)(Args...);
                const std::tuple<Args...> args;
        };

    template<typename R, typename... Args>
        utils::ID<Functor<R, Args...>> functor(R(&&pF)(Args...), Args&&... pArgs)
        {
            return utils::ID<Functor<R, Args...>>(Functor<R, Args...>(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...));
        }
}

