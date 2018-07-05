#pragma once
#include <vector>
#include <functional>
#include <type_traits>
#include <assert.h>

namespace utils
{
    const size_t INVALID_ID = -1;
    template<typename T>
    using ID_Container = typename std::vector<T>;

    template<typename T>
    using ID_Type = typename std::remove_reference<T>::type;

    template<typename T>
    ID_Container<T> id_container;

    template<typename T>
        struct ID
        {
            using Type = ID_Type<T>;
            using Container = ID_Container<Type>;
            using size_type = typename Container::size_type;
            constexpr static Container& container = id_container<Type>;

            ID()
                :index(INVALID_ID)
            {}

            ID( const size_t i )
                :index( i )
            {}

            ID( T obj )
                :index( container.size() )
            {
                container.push_back( obj );
            }

            size_type index;

            Type& get() const
            {
                assert( container.size() );
                return container[index];
            }

            Type* operator->() const
            {
                return &get();
            }
            Type& operator*() const
            {
                return get();
            }
        };
}
