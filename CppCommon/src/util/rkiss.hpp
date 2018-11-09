/*
 rkiss.hpp
 */


#ifndef UTIL_RKISS_HPP_
#define UTIL_RKISS_HPP_

#include "../defines.h"

// RKISSによる乱数生成
// StockFishとやねうらお氏の解説を参考に

//クラス
class RKISS64{
private:
    uint64_t x, y, z, t;
    
    constexpr uint64_t rot(uint64_t a, uint64_t k)const{
        return (a << k) | (a >> (64 - k));
    }
    uint64_t rand64(){
        const uint64_t tmp = x - rot(y, 7);
        x = y ^ rot(z, 13);
        y = z + rot(t, 37);
        z = t + tmp;
        return t = tmp + x;
    }
    
public:
    void srand(int seed = 73){
        seed = min(seed, 99);
        x = 0xf1ea5eed, y = z = t = 0xd4e12c77;
        for(int i = 0; i < seed; ++i){ rand64(); }
    }
    
    template<typename T>
    T rand(){ return T(rand64()); }
    
    constexpr RKISS64()
    :x(), y(), z(), t(){}
    
    RKISS64(const int s){ srand(s); }
};


#endif // UTIL_RKISS_HPP_