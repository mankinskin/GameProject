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
            ID( size_type I )
                :index( I )
            {}
            ID( const T& data )
                :index( container.size() )
            {
                container.push_back( data );
            }
            size_type index;

            operator size_type() const
            {
                return index;
            }
            T* operator->() const
            {
                return &container[index];
            }
            T& operator*() const
            {
                return container[index];
            }
            operator T() const
            {
                return container[index];
            }
        };

    template<typename T>
        typename ID<T>::Container ID<T>::container;
}
