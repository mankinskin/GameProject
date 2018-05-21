#pragma once
#include <vector>
#include <functional>


namespace utils {
    
    template<
        typename T, 
        typename Container>
    struct Itr 
	{
        Itr()
            :container( nullptr ), index( 0 )
        {}
        Itr( const Itr<T, Container>& other )
            :container( other.container ), index( other.index )
        {}
        Itr( Container& arr, typename Container::size_type i )
            :container( arr ), index( i )
        {}

        Container& container; 
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
