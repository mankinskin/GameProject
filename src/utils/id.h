#pragma once
#include <vector>
#include <functional>

namespace utils
{
    template<typename T>
        struct ID
        {
            using Container = typename std::vector<T>;
            using size_type = typename Container::size_type;
            static Container container;
            ID()
                :index( container.size() )
            {
                container.push_back( T() );
            }

            ID( const size_t i )
                :index( i )
            {}

            ID( const T obj )
                :index( container.size() )
            {
                container.push_back( obj );
            }
            size_type index;

            T& get() const
            {
                return container[index];
            }

            T* operator->() const
            {
                return &get();
            }
            T& operator*() const
            {
                return get();
            }
        };

    template<typename T>
        typename ID<T>::Container ID<T>::container = ID<T>::Container();
}
