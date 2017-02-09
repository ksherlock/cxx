//
//  algorithm_17.h
//  2Term
//
//  Created by Kelvin Sherlock on 7/12/2016.
//
//

#ifndef cxx_algorithm_h
#define cxx_algorithm_h

#include <algorithm>


// c++17

template<class T, class Compare>
constexpr const T& clamp( const T& v, const T& lo, const T& hi, Compare comp ) {
    return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi ) {
    return clamp( v, lo, hi, std::less<T>() );
}


#endif /* algorithm_17_h */
