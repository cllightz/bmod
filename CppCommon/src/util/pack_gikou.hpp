/*
 pack.hpp
 Katsuki Ohto
 */

#ifndef UTIL_PACK_HPP_
#define UTIL_PACK_HPP_

// 将棋ソフト「技巧」を参考に、
// 複数の整数型や不動小数点型をまとめて扱いSIMD演算で高速化するクラス

#include "bitOperation.hpp"

// コンパイラにベクトル型を使わせる(「技巧」より)
#if defined(__clang__) || defined(__GNUC__)
# define USE_VECTOR_EXTENSIONS
#endif

template<typename T, unsigned int kSize>
class Pack{
    static_assert(   std::is_same<T,   int8_t>::value
                  || std::is_same<T,  uint8_t>::value
                  || std::is_same<T,  int16_t>::value
                  || std::is_same<T, uint16_t>::value
                  || std::is_same<T,  int32_t>::value
                  || std::is_same<T, uint32_t>::value
                  || std::is_same<T,  int64_t>::value
                  || std::is_same<T, uint64_t>::value
                  || std::is_same<T,    float>::value
                  || std::is_same<T,   double>::value,
                  "Pack : data type be an integer or a floating point.");
    
    static_assert(kSize > 0, "Pack : kSize must be positive.");
    
    static_assert(any2Bits(kSize), "Pack : kSize must a power of 2.");
    
    static_assert(sizeof(T) * kSize > 8, "Pack : vector smaller than 9 bytes doesn't require Pack.");
    
public:
    using data_t = T;
    
    Pack(){}
    
    explicit Pack(data_t v){
        
    }
};

#endif // UTIL_PACK_HPP_