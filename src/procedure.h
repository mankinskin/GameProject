#pragma once
#include <tuple>
#include "utils/id.h"
#include "utils/tuple_utils.h"
#include "functor.h"

namespace signals
{
    template<typename... Funcs>
        struct Procedure
        {
            using ID = utils::ID<Procedure<Funcs...>>;
            constexpr static typename ID::Container& all = ID::container;
            Procedure( utils::ID<Funcs>... fs )
                : funcs( fs... )
            {
                (void)init;
            }

            constexpr void invoke_n(utils::_index<0>) const
            {}

            template<size_t N>
            constexpr void invoke_n(utils::_index<N>) const
            {
                std::get<N-1>( funcs )->invoke();
            }

            void invoke() const
            {
                invoke_n(utils::_index<sizeof...(Funcs)>());
            }
            static void invoke( const size_t i )
            {
                all[i].invoke();
            }

            static void clear()
            {
                ID::clear();
            }

            static size_t invoker_index;
            private:
            std::tuple<utils::ID<Funcs>...> funcs;
            struct At_Init
            {
                At_Init()
                {
                    Procedure<Funcs...>::invoker_index = invokers.size();
                    invokers.push_back( Procedure<Funcs...>::invoke );
                    destructors.push_back( Procedure<Funcs...>::clear );
                }
            };
            static At_Init init;
        };
    template<typename... Funcs>
        size_t Procedure<Funcs...>::invoker_index = 0;

    template<typename... Funcs>
        typename Procedure<Funcs...>::At_Init Procedure<Funcs...>::init = Procedure<Funcs...>::At_Init();

    template<typename... Funcs>
        utils::ID<Procedure<Funcs...>> procedure( utils::ID<Funcs>... pFuncs )
        {
            return utils::ID<Procedure<Funcs...>>( Procedure<Funcs...>( pFuncs... ) );
        }

    struct Reaction
    {
        using ID = utils::ID<Reaction>;
        constexpr static typename ID::Container& all = ID::container;

        template<typename... Funcs>
            Reaction( ListenerID pListener, utils::ID<Funcs>... pFuncs )
            : invoker( Procedure<Funcs...>::invoker_index )
            , index( Procedure<Funcs...>::all.size() )
            , listener( pListener )
        {
            Procedure<Funcs...>::all.push_back( Procedure<Funcs...>( pFuncs... ) );
        }
        template<typename... Funcs>
            Reaction( ListenerID pListener, utils::ID<Procedure<Funcs...>> pProc )
            : invoker( Procedure<Funcs...>::invoker_index )
            , index( pProc.index )
            , listener( pListener )
        {}

        void invoke() const
        {
            (*invokers[invoker])(index);
        }

        ListenerID listener;
        const size_t invoker; // invoker in functorInvokers
        const size_t index;   // local index for functor
    };

    template<typename R, typename... Args>
        utils::ID<Reaction> reaction( ListenerID pListener, R( &&pF )( Args... ), Args&&... pArgs )
        {
            return utils::ID<Reaction>( Reaction( pListener, std::forward<R( Args... )>( pF ), std::forward<Args>( pArgs )... ) );
        }

    template<typename... Funcs>
        utils::ID<Reaction> reaction( ListenerID pListener, utils::ID<Funcs>... pFuncs )
        {
            return utils::ID<Reaction>( Reaction( pListener, pFuncs... ) );
        }
    void runReactions();
    void clearProcedures();
}
