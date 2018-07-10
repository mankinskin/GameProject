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
            {}

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

            private:
            std::tuple<utils::ID<Funcs>...> funcs;
        };

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
            : invoker( Procedure<Funcs...>::invoke )
            , index( Procedure<Funcs...>::all.size() )
            , listener( pListener )
        {
            Procedure<Funcs...>::all.push_back( Procedure<Funcs...>( pFuncs... ) );
        }
        template<typename... Funcs>
            Reaction( ListenerID pListener, utils::ID<Procedure<Funcs...>> pProc )
            : invoker( Procedure<Funcs...>::invoke )
            , index( pProc.index )
            , listener( pListener )
        {}

        void invoke() const
        {
            invoker(index);
        }

        ListenerID listener;
        void(&invoker)(const size_t);
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
