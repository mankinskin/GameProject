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
            ID( size_type I )
                :index( I )
            {}
            ID( const T data )
                :index( container.size() )
            {
                container.push_back( data );
            }
            size_type index;

            operator size_type() const
            {
                return index;
            }
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
            operator T() const
            {
                return get();
            }
        };

    template<typename T>
        typename ID<T>::Container ID<T>::container;
}
