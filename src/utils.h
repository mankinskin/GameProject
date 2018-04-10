#pragma once
#include <vector>

namespace utils {
    
    template<
        typename T, 
        template<typename...>typename Container = std::vector >
    struct Itr {
        Itr( Container<T>& arr, typename Container<T>::size_type i )
            :container( arr ), index( i ){}

        Container<T>& container; 
        typename Container<T>::size_type index;

        operator typename Container<T>::size_type() const
        {
            return index;
        }
        T& operator*(){
            return container[index];
        }

        Itr<T>& operator=( Itr<T>& other ){
            return other;
        }

    };


}
