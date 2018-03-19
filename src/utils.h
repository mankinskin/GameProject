#pragma once
#include <vector>

namespace utils {
    
    template<
        typename T, 
        template<typename...>typename Array = std::vector >
    struct Itr {
        Itr( Array<T>& arr, typename Array<T>::size_type i )
            :array( arr ), index( i ){}

        Array<T>& array; 
        typename Array<T>::size_type index;

        operator unsigned int() const
        {
            return index;
        }
        T& operator*(){
            return array[index];
        }

        Itr<T>& operator=( Itr<T>& other ){
            return other;
        }

    };


}
