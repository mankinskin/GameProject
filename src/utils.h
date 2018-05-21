#pragma once
#include <vector>
#include <functional>


namespace utils {
    
    template<
        typename T, 
        typename Container,
        Container& container>
    struct Itr 
	{
        Itr( const T& element )
            :index( container.size() )
        {
            container.push_back( element );
        }
        Itr( typename Container::size_type i )
            :index( i )
        {}

        typename Container::size_type index;

        operator typename Container::size_type&()
        {
            return index;
        }
        operator T&()
        {
            return container[index];
        }
        T& operator*()
        {
            return container[index];
        }
        T* operator->()
        {
            return &container[index];
        }
    };


}
