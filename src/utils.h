#pragma once
#include <vector>

namespace utils {
    
    template<
        typename T, 
        typename Container = std::vector<T> >
    struct Itr {
        Itr( Container& arr, typename Container::size_type i )
            :container( arr ), index( i ){}

        Container& container; 
        typename Container::size_type index;

        operator typename Container::size_type() const
        {
            return index;
        }
        T& operator*(){
            return container[index];
        }

        Itr<T, Container>& operator=( Itr<T, Container>& other ){
            return other;
        }

    };


}
