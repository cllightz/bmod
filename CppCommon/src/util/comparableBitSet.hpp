/*
 comparablebitSet.hpp
 Katsuki Ohto
 */

#ifndef UTIL_COMPARABLEBITSET_HPP_
#define UTIL_COMPARABLEBITSET_HPP_

// 集合間の比較が定義されたビットセット

#include <string>
#include <iostream>
#include <sstream>

#include "../defines.h"
#include "bitSet.hpp"

template<typename data_t>
class ComparableBitSet : public BitSetInRegister<data_t>{
private:
    using base_t = BitSetInRegister<data_t>;
    using this_t = ComparableBitSet<data_t>;
public:
    ComparableBitSet() : base_t(){}
    ComparableBitSet(const data_t& ad) : base_t(ad){}
    
    bool operator <(const ComparableBitSet& rhs)const{
        return base_t::bsr() <= rhs.bsf()
        && base_t::operator !=(rhs);
    }
    bool operator >(const ComparableBitSet& rhs)const{
        return base_t::bsf() >= rhs.bsr()
        && base_t::operator !=(rhs);
    }
    bool operator <=(const ComparableBitSet& rhs)const{
        return !operator >(rhs);
    }
    bool operator >=(const ComparableBitSet& rhs)const{
        return !operator <(rhs);
    }
private:
};

template<class data_t>
int smallerSet(const ComparableBitSet<data_t>& a,
               const ComparableBitSet<data_t>& b){
    // 2つの結果候補のうちどちらが「小さい」か
    // a -> 0
    // b -> 1
    // どちらとも言えない -> -1
    if(a < b){
        return 0;
    }
    if(a > b){
        return 1;
    }
    return -1;
}

using ComparableBitSet8 = ComparableBitSet<uint8_t>;
using ComparableBitSet16 = ComparableBitSet<uint16_t>;
using ComparableBitSet32 = ComparableBitSet<uint32_t>;
using ComparableBitSet64 = ComparableBitSet<uint64_t>;

#endif // UTIL_COMPARABLEBITSET_HPP_