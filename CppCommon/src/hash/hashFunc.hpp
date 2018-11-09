/*
 hashfunc.hpp
 Katsuki Ohto
 */

#ifndef UTIL_HASHFUNC_HPP_
#define UTIL_HASHFUNC_HPP_

#include "../defines.h"
#include "../util/bitOperation.hpp"

// ハッシュ値系の演算

// 関数名
// gen...情報より0から計算
// genproc...ハッシュ値に対して進行情報を与えて進行させる
// proc...ハッシュ値に対して進行ハッシュ値を与えて進行させる
// merge...同種の情報のハッシュ値を合成、進行
// knit...主に異種の情報のハッシュ値から求めるハッシュ値を計算

// 基本関数

// 線形加算
constexpr uint64_t addHash(uint64_t h0, uint64_t h1)noexcept{ return h0 ^ h1; }

/**************************ビット**************************/

// 1 << 0, 1 << 1, ..., 1 << 63 の値を 0 ~ 63 に転置する完全ハッシュ関数
// 1 << 0, 1 << 1, ..., 1 << 31 の値を 0 ~ 31 に転置する完全ハッシュ関数

inline int genPerfectHash_Bit64(uint64_t b)noexcept{
    //return (int)(((b) * 0x03F566ED27179461ULL) >>5 8); // 乗算
    return bsf64(b);
}

inline int genPerfectHash_Bit32(uint32_t b)noexcept{
    return bsf32(b);
}

/**************************交叉ハッシュ**************************/

// 交叉
constexpr uint64_t crossHash(uint64_t h0, uint64_t h1)noexcept{
    return crossBits64(h0, h1);
}
constexpr uint64_t crossHash(uint64_t h0, uint64_t h1, uint64_t h2)noexcept{
    return crossBits64(h0, h1, h2);
}
constexpr uint64_t crossHash(uint64_t h0, uint64_t h1, uint64_t h2, uint64_t h3)noexcept{
    return crossBits64(h0, h1, h2, h3);
}
constexpr uint64_t crossHash(uint64_t h0, uint64_t h1, uint64_t h2, uint64_t h3, uint64_t h4)noexcept{
    return crossBits64(h0, h1, h2, h3, h4);
}

template<int N>
uint64_t crossHash(const uint64_t h[]){
    return crossBits64<N>(h);
}

// 部分生成
template<int N>
constexpr uint64_t genPartCrossedHash(const int n, const uint64_t hash)noexcept{
    return hash & genCrossNumber<N>(n);
}

template<int N>
constexpr uint64_t genPart2CrossedHash(const int n, const uint64_t hash0, const uint64_t hash1)noexcept{
    return (hash0 ^ hash1) & genCrossNumber<N>(n);
}

// 交叉更新
template<int N>
constexpr uint64_t procCrossedHash(const uint64_t hash, const int n, const uint64_t hash_dist)noexcept{
    return hash ^ (hash_dist & genCrossNumber<N>(n));
}

#endif // UTIL_HASHFUNC_HPP_

