/*
 statistics.hpp
 Katsuki Ohto
 */

#ifndef UTIL_STATISTICS_HPP_
#define UTIL_STATISTICS_HPP_

#include "../defines.h"

// 統計関係演算

template<class array_like>
double mean(const array_like& ar){
    double sum = 0;
    for(const auto& v : ar){
        sum += v;
    }
    return sum / ar.size();
}

template<class array_like>
double var(const array_like& ar){
    double sum2 = 0;
    for(const auto& v : ar){
        sum2 += v * v;
    }
    return sum2 / ar.size() - mean(ar);
}

template<class array_like>
double stddev(const array_like& ar){
    return sqrt(var(ar));
}

#endif // UTIL_STATISTICS_HPP_