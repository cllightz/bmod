/*
 longBits.hpp
 Katsuki Ohto
 */

// 64ビットを超える型の定義と演算

#ifndef UTIL_LONG_BITS_HPP_
#define UTIL_LONG_BITS_HPP_

#include <cassert>
#include <iostream>
#include <iomanip>

#include "../defines.h"
#include "bitOperation.hpp"
#include "bitSet.hpp"

/***************************汎用レジスタによる整数型**************************/

template<std::size_t N>
class alignas(std::array<std::uint64_t, N>) array_long_bits_t : public std::array<std::uint64_t, N>{
private:
    using base_t = std::array<std::uint64_t, N>;
    using this_t = array_long_bits_t<N>;
public:
    constexpr base_t array()const noexcept{
        return base_t(*this);
    }
    std::uint64_t operator [](std::size_t i)const{
        return base_t::operator [](i);
    }
    std::uint64_t& operator [](std::size_t i){
        return base_t::operator [](i);
    }
    this_t operator ~()const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = ~(operator [](i));
        }
        return a;
    }
    this_t operator &(const this_t& b)const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = (operator [](i)) & b[i];
        }
        return a;
    }
    this_t operator |(const this_t& b)const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = (operator [](i)) | b[i];
        }
        return a;
    }
    this_t operator ^(const this_t& b)const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = (operator [](i)) ^ b[i];
        }
        return a;
    }
    this_t operator +(const this_t& b)const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = (operator [](i)) + b[i];
        }
        return a;
    }
    this_t operator -(const this_t& b)const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = (operator [](i)) - b[i];
        }
        return a;
    }
    this_t operator <<(const int n)const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = (operator [](i)) << n;
        }
        return a;
    }
    this_t operator >>(const int n)const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = (operator [](i)) >> n;
        }
        return a;
    }
    
    this_t& operator &=(const this_t& b)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) &= b[i];
        }
        return *this;
    }
    this_t& operator |=(const this_t& b)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) |= b[i];
        }
        return *this;
    }
    this_t& operator ^=(const this_t& b)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) ^= b[i];
        }
        return *this;
    }
    this_t& operator +=(const this_t& b)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) += b[i];
        }
        return *this;
    }
    this_t& operator -=(const this_t& b)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) -= b[i];
        }
        return *this;
    }
    this_t& operator <<=(const int n)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) <<= n;
        }
        return *this;
    }
    this_t& operator >>=(const int n)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) >>= n;
        }
        return *this;
    }
    
    // 値設定
    this_t& clear()noexcept{
        base_t::fill(0);
        return *this;
    }
    this_t& fill1(std::uint64_t v)noexcept{
        v = -v;
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) = v;
        }
        return *this;
    }
    this_t& fill2(std::uint64_t v)noexcept{
        v *= 0x5555555555555555ULL;
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) = v;
        }
        return *this;
    }
    this_t& fill4(std::uint64_t v)noexcept{
        v *= 0x1111111111111111ULL;
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) = v;
        }
        return *this;
    }
    this_t& fill8(std::uint64_t v)noexcept{
        v *= 0x0101010101010101ULL;
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) = v;
        }
        return *this;
    }
    this_t& fill16(std::uint64_t v)noexcept{
        v *= 0x0001000100010001ULL;
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) = v;
        }
        return *this;
    }
    this_t& fill32(std::uint64_t v)noexcept{
        v *= 0x0000000100000001ULL;
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) = v;
        }
        return *this;
    }
    this_t& fill64(std::uint64_t v)noexcept{
        for(std::size_t i = 0; i < N; ++i){
            operator [](i) = v;
        }
        return *this;
    }
    template<std::size_t M = 0>
    this_t& pack64(const std::uint64_t v)noexcept{
        operator [](M) = v;
        for(std::size_t i = M + 1; i < N; ++i){
            operator[] (i) = 0;
        }
        return *this;
    }
    template<std::size_t M = 0, typename ... args_t>
    this_t& pack64(const std::uint64_t v, args_t ... others)noexcept{
        operator [](M) = v;
        return pack64<M + 1>(others...);
    }
    
    
    // 値設定して返す
    static this_t zero()noexcept{
        this_t a;
        a.clear();
        return a;
    }
    static this_t filled1(std::uint64_t v)noexcept{
        this_t a;
        a.fill1(v);
        return a;
    }
    static this_t filled2(std::uint64_t v)noexcept{
        this_t a;
        a.fill2(v);
        return a;
    }
    static this_t filled4(std::uint64_t v)noexcept{
        this_t a;
        a.fill4(v);
        return a;
    }
    static this_t filled8(std::uint64_t v)noexcept{
        this_t a;
        a.fill8(v);
        return a;
    }
    static this_t filled16(std::uint64_t v)noexcept{
        this_t a;
        a.fill16(v);
        return a;
    }
    static this_t filled32(std::uint64_t v)noexcept{
        this_t a;
        a.fill32(v);
        return a;
    }
    static this_t filled64(std::uint64_t v)noexcept{
        this_t a;
        a.fill64(v);
        return a;
    }
    
    static this_t packed64(const std::uint64_t v)noexcept{
        this_t a;
        a.pack64(v);
        return a;
    }
    
    template<typename ... args_t>
    static this_t packed64(args_t ... vs){
        this_t a;
        a.pack64(vs...);
        return a;
    }
    
    // 合計
    std::uint64_t sum1()const noexcept{
        std::uint64_t sm = 0;
        for(std::size_t i = 0; i < N; ++i){
            sm += countBits(operator [](i));
        }
        return sm;
    }
    std::uint64_t sum2()const noexcept{
        std::uint64_t sm = 0;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<2> ba(operator [](i));
            sm += ba.sum();
        }
        return sm;
    }
    std::uint64_t sum4()const noexcept{
        std::uint64_t sm = 0;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<4> ba(operator [](i));
            sm += ba.sum();
        }
        return sm;
    }
    std::uint64_t sum8()const noexcept{
        std::uint64_t sm = 0;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<8> ba(operator [](i));
            sm += ba.sum();
        }
        return sm;
    }
    std::uint64_t sum16()const noexcept{
        std::uint64_t sm = 0;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<16> ba(operator [](i));
            sm += ba.sum();
        }
        return sm;
    }
    std::uint64_t sum32()const noexcept{
        std::uint64_t sm = 0;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<32> ba(operator [](i));
            sm += ba.sum();
        }
        return sm;
    }
    std::uint64_t sum64()const noexcept{
        std::uint64_t sm = 0;
        for(std::size_t i = 0; i < N; ++i){
            sm += operator [](i);
        }
        return sm;
    }
    
    // ブロックごとの合計
    this_t sum1_per64()const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            a[i] = countBits(operator [](i));
        }
        return a;
    }
    this_t sum2_per64()const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<2> ba(operator [](i));
            a[i] = ba.sum();
        }
        return a;
    }
    this_t sum4_per64()const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<4> ba(operator [](i));
            a[i] = ba.sum();
        }
        return a;
    }
    this_t sum8_per64()const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<8> ba(operator [](i));
            a[i] = ba.sum();
        }
        return a;
    }
    this_t sum16_per64()const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<16> ba(operator [](i));
            a[i] = ba.sum();
        }
        return a;
    }
    this_t sum32_per64()const noexcept{
        this_t a;
        for(std::size_t i = 0; i < N; ++i){
            BitArray64<32> ba(operator [](i));
            a[i] = ba.sum();
        }
        return a;
    }
    
    bool operator ==(const this_t& b)const noexcept{
        for(std::size_t i = 0; i < N; ++i){
            if(operator [](i) != b[i]){
                return false;
            }
        }
        return true;
    }
    bool operator !=(const this_t& b)const noexcept{
        return !(operator ==(b));
    }
    
    constexpr array_long_bits_t(): base_t(){}
    constexpr array_long_bits_t(const array_long_bits_t& b): base_t(b){}
};

template<std::size_t N>
bool holds(const array_long_bits_t<N>& b0, const array_long_bits_t<N>& b1)noexcept{
    for(std::size_t i = 0; i < N; ++i){
        if(!holdsBits(b0[i], b1[i])){
            return false;
        }
    }
    return true;
}
template<std::size_t N>
bool isExclusive(const array_long_bits_t<N>& b0, const array_long_bits_t<N>& b1)noexcept{
    for(std::size_t i = 0; i < N; ++i){
        if(!isExclusive(b0[i], b1[i])){
            return false;
        }
    }
    return true;
}

/**************************整数型**************************/

#ifdef HAVE_AVX2

#include <immintrin.h>

class alignas(__m128i) bits128_t{
public:
    union{
        __m128i xmm_;
        array_long_bits_t<2> ar_;
    };
private:
    using this_t = bits128_t;

    struct not_type{
        const __m128i not_xmm_;

        // ~b0 & b1
        bits128_t operator &(const bits128_t& b)const{
            return bits128_t(_mm_andnot_si128(not_xmm_, b.xmm_));
        }
        // 通常のnot
        operator bits128_t()const{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
            __m128i temp;
            const __m128i mask = _mm_cmpeq_epi64(temp, temp);
#pragma GCC diagnostic pop
            //const __m128i mask = _mm_set1_epi8(-1);
            return bits128_t(_mm_xor_si128(not_xmm_, mask));
        }
        explicit not_type(const bits128_t& b) : not_xmm_(b.xmm_) {}
    };

public:
    std::uint64_t operator [](std::size_t i)const{
        return ar_[i];
    }
    std::uint64_t& operator [](std::size_t i){
        return ar_[i];
    }
    this_t::not_type operator ~()const noexcept{
        return this_t::not_type(*this);
    }
    this_t operator &(const this_t::not_type& not_b)const noexcept{
        return this_t(_mm_andnot_si128(not_b.not_xmm_, xmm_));
    }
    this_t operator &(const this_t& b)const noexcept{
        return this_t(_mm_and_si128(xmm_, b.xmm_));
    }
    this_t operator |(const this_t& b)const noexcept{
        return this_t(_mm_or_si128(xmm_, b.xmm_));
    }
    this_t operator ^(const this_t& b)const noexcept{
        return this_t(_mm_xor_si128(xmm_, b.xmm_));
    }
    this_t operator +(const this_t& b)const noexcept{
        return this_t(xmm_ + b.xmm_);
    }
    this_t operator -(const this_t& b)const noexcept{
        return this_t(xmm_ - b.xmm_);
    }
    this_t operator <<(const int i)const noexcept{
        return this_t(xmm_ << i);
    }
    this_t operator >>(const int i)const noexcept{
        return this_t(xmm_ >> i);
    }

    this_t& clear()noexcept{
        xmm_ = _mm_setzero_si128();
        return *this;
    }
    this_t& operator |=(const this_t& b)noexcept{
        xmm_ = _mm_or_si128(xmm_, b.xmm_);
        return *this;
    }
    this_t& operator &=(const this_t::not_type& not_b)noexcept{
        xmm_ = _mm_andnot_si128(not_b.not_xmm_, xmm_);
        return *this;
    }
    this_t& operator &=(const this_t& b)noexcept{
        xmm_ = _mm_and_si128(xmm_, b.xmm_);
        return *this;
    }
    this_t& operator ^=(const this_t& b)noexcept{
        xmm_ = _mm_xor_si128(xmm_, b.xmm_);
        return *this;
    }
    this_t& operator +=(const bits128_t& b)noexcept{
        xmm_ = xmm_ + b.xmm_;
        return *this;
    }
    this_t& operator -=(const bits128_t& b)noexcept{
        xmm_ = xmm_ - b.xmm_;
        return *this;
    }
    this_t& operator <<=(const int i)noexcept{
        xmm_ = xmm_ << i;
        return *this;
    }
    this_t& operator >>=(const int i)noexcept{
        xmm_ = xmm_ >> i;
        return *this;
    }

    bool operator ==(const this_t& b)const noexcept{
        __m128i temp = xmm_ - b.xmm_;
        return _mm_testz_si128(temp, temp);
    }
    bool operator !=(const this_t& b)const noexcept{
        return !((*this) == b);
    }
    
    // 合計
    std::uint64_t sum1()const noexcept{ return ar_.sum1(); }
    std::uint64_t sum2()const noexcept{ return ar_.sum2(); }
    std::uint64_t sum4()const noexcept{ return ar_.sum4(); }
    std::uint64_t sum8()const noexcept{ return ar_.sum8(); }
    std::uint64_t sum16()const noexcept{ return ar_.sum16(); }
    std::uint64_t sum32()const noexcept{ return ar_.sum32(); }
    std::uint64_t sum64()const noexcept{ return ar_.sum64(); }
    // ブロックごとの合計 TODO
    this_t sum1_per64()const noexcept{ return this_t(ar_.sum1_per64()); }
    this_t sum2_per64()const noexcept{ return this_t(ar_.sum2_per64()); }
    this_t sum4_per64()const noexcept{ return this_t(ar_.sum4_per64()); }
    this_t sum8_per64()const noexcept{ return this_t(ar_.sum8_per64()); }
    this_t sum16_per64()const noexcept{ return this_t(ar_.sum16_per64()); }
    this_t sum32_per64()const noexcept{ return this_t(ar_.sum32_per64()); }
    
    // 値設定して返す
    static this_t zero()noexcept{
        return this_t(_mm_setzero_si128());
    }
    static this_t filled4(std::uint8_t i)noexcept{
        return this_t(_mm_set1_epi8(int8_t(i | (i << 4))));
    }
    static this_t filled8(std::uint8_t i)noexcept{
        return this_t(_mm_set1_epi8(int8_t(i)));
    }
    static this_t filled16(std::uint16_t i)noexcept{
        return this_t(_mm_set1_epi16(int16_t(i)));
    }
    static this_t filled32(std::uint32_t i)noexcept{
        return this_t(_mm_set1_epi32(int32_t(i)));
    }
    static this_t filled64(std::uint64_t i)noexcept{
        return this_t(_mm_set_epi32(std::int32_t(i >> 32), std::int32_t(i),
                                    std::int32_t(i >> 32), std::int32_t(i)));
    }
    static this_t packed64(std::uint64_t i0, std::uint64_t i1)noexcept{
        return this_t(_mm_set_epi32(std::int32_t(i1 >> 32), std::int32_t(i1),
                                    std::int32_t(i0 >> 32), std::int32_t(i0)));
    }

    constexpr bits128_t(): xmm_(){}
    explicit constexpr bits128_t(const __m128i& i): xmm_(i){}
    explicit constexpr bits128_t(const array_long_bits_t<2>& ar): ar_(ar){}
    constexpr bits128_t(const bits128_t& b): xmm_(b.xmm_){}
};

bool holds(const bits128_t& b0, const bits128_t& b1)noexcept{
    return _mm_testc_si128(b0.xmm_, b1.xmm_);
}
bool isExclusive(const bits128_t& b0, const bits128_t& b1)noexcept{
    return _mm_testz_si128(b0.xmm_, b1.xmm_);
}
    
class alignas(__m256i) bits256_t{
public:
    union{
        __m256i ymm_;
        array_long_bits_t<4> ar_;
    };
private:
    using this_t = bits256_t;

    struct not_type{
        const __m256i not_ymm_;
            
        // ~b0 & b1
        bits256_t operator &(const bits256_t& b)const{
            return bits256_t(_mm256_andnot_si256(not_ymm_, b.ymm_));
        }
        // 通常のnot
        operator bits256_t()const{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
            __m256i temp;
            const __m256i mask = _mm256_cmpeq_epi64(temp, temp);
#pragma GCC diagnostic pop
            //const __m256i mask = _mm256_set1_epi8(-1);
            return bits256_t(_mm256_xor_si256(not_ymm_, mask));
        }
        explicit not_type(const bits256_t& b) : not_ymm_(b.ymm_) {}
    };
        
public:
    std::uint64_t operator [](std::size_t i)const{
        return ar_[i];
    }
    std::uint64_t& operator [](std::size_t i){
        return ar_[i];
    }
    this_t::not_type operator ~()const noexcept{
        return this_t::not_type(*this);
    }
    this_t operator &(const bits256_t::not_type& not_b)const noexcept{
        return this_t(_mm256_andnot_si256(not_b.not_ymm_, ymm_));
    }
    this_t operator &(const bits256_t& b)const noexcept{
        return this_t(_mm256_and_si256(ymm_, b.ymm_));
    }
    this_t operator |(const bits256_t& b)const noexcept{
        return this_t(_mm256_or_si256(ymm_, b.ymm_));
    }
    this_t operator ^(const bits256_t& b)const noexcept{
        return this_t(_mm256_xor_si256(ymm_, b.ymm_));
    }
    this_t operator +(const bits256_t& b)const noexcept{
        return this_t(ymm_ + b.ymm_);
    }
    this_t operator -(const bits256_t& b)const noexcept{
        return this_t(ymm_ - b.ymm_);
    }
    this_t operator <<(const int i)const noexcept{
        return this_t(ymm_ << i);
    }
    this_t operator >>(const int i)const noexcept{
        return this_t(ymm_ >> i);
    }

    this_t& clear()noexcept{
        ymm_ = _mm256_setzero_si256();
        return *this;
    }
    this_t& operator |=(const this_t& b)noexcept{
        ymm_ = _mm256_or_si256(ymm_, b.ymm_);
        return *this;
    }
    this_t& operator &=(const this_t::not_type& not_b)noexcept{
        ymm_ = _mm256_andnot_si256(not_b.not_ymm_, ymm_);
        return *this;
    }
    this_t& operator &=(const this_t& b)noexcept{
        ymm_ = _mm256_and_si256(ymm_, b.ymm_);
        return *this;
    }
    this_t& operator ^=(const this_t& b)noexcept{
        ymm_ = _mm256_xor_si256(ymm_, b.ymm_);
        return *this;
    }
    this_t& operator +=(const this_t& b)noexcept{
        ymm_ = ymm_ + b.ymm_;
        return *this;
    }
    this_t& operator -=(const this_t& b)noexcept{
        ymm_ = ymm_ - b.ymm_;
        return *this;
    }
    this_t& operator <<=(const int i)noexcept{
        ymm_ = ymm_ << i;
        return *this;
    }
    this_t& operator >>=(const int i)noexcept{
        ymm_ = ymm_ >> i;
        return *this;
    }

    bool operator ==(const this_t& b)const noexcept{
        __m256i temp = ymm_ - b.ymm_;
        return _mm256_testz_si256(temp, temp);
    }
    bool operator !=(const this_t& b)const noexcept{
        return !((*this) == b);
    }
    
    // 合計
    std::uint64_t sum1()const noexcept{ return ar_.sum1(); }
    std::uint64_t sum2()const noexcept{ return ar_.sum2(); }
    std::uint64_t sum4()const noexcept{ return ar_.sum4(); }
    std::uint64_t sum8()const noexcept{ return ar_.sum8(); }
    std::uint64_t sum16()const noexcept{ return ar_.sum16(); }
    std::uint64_t sum32()const noexcept{ return ar_.sum32(); }
    std::uint64_t sum64()const noexcept{ return ar_.sum64(); }
    // ブロックごとの合計 TODO
    this_t sum1_per64()const noexcept{ return this_t(ar_.sum1_per64()); }
    this_t sum2_per64()const noexcept{ return this_t(ar_.sum2_per64()); }
    this_t sum4_per64()const noexcept{ return this_t(ar_.sum4_per64()); }
    this_t sum8_per64()const noexcept{ return this_t(ar_.sum8_per64()); }
    this_t sum16_per64()const noexcept{ return this_t(ar_.sum16_per64()); }
    this_t sum32_per64()const noexcept{ return this_t(ar_.sum32_per64()); }
    
    // 値設定して返す
    static this_t zero()noexcept{
        return this_t(_mm256_setzero_si256());
    }
    static this_t filled1(std::uint8_t i)noexcept{
        return this_t(_mm256_set1_epi8(int8_t(-i)));
    }
    static this_t filled2(std::uint8_t i)noexcept{
        return this_t(_mm256_set1_epi8(int8_t(i * 0x55)));
    }
    static this_t filled4(std::uint8_t i)noexcept{
        return this_t(_mm256_set1_epi8(int8_t(i * 0x11)));
    }
    static this_t filled8(std::uint8_t i)noexcept{
        return this_t(_mm256_set1_epi8(int8_t(i)));
    }
    static this_t filled16(std::uint16_t i)noexcept{
        return this_t(_mm256_set1_epi16(int16_t(i)));
    }
    static this_t filled32(std::uint32_t i)noexcept{
        return this_t(_mm256_set1_epi32(int32_t(i)));
    }
    static this_t filled64(std::uint64_t i)noexcept{
        return this_t(_mm256_set_epi32(std::int32_t(i >> 32), std::int32_t(i),
                                       std::int32_t(i >> 32), std::int32_t(i),
                                       std::int32_t(i >> 32), std::int32_t(i),
                                       std::int32_t(i >> 32), std::int32_t(i)));
    }
    
    static this_t packed64(std::uint64_t i0, std::uint64_t i1 = 0,
                           std::uint64_t i2 = 0, std::uint64_t i3 = 0)noexcept{
        return this_t(_mm256_set_epi32(std::int32_t(i3 >> 32), std::int32_t(i3),
                                       std::int32_t(i2 >> 32), std::int32_t(i2),
                                       std::int32_t(i1 >> 32), std::int32_t(i1),
                                       std::int32_t(i0 >> 32), std::int32_t(i0)));
    }

    constexpr bits256_t(): ymm_(){}
    explicit constexpr bits256_t(const __m256i& i): ymm_(i){}
    explicit constexpr bits256_t(const array_long_bits_t<4>& ar): ar_(ar){}
    constexpr bits256_t(const bits256_t& b): ymm_(b.ymm_){}
};
    
bool holds(const bits256_t& b0, const bits256_t& b1)noexcept{
    return _mm256_testc_si256(b0.ymm_, b1.ymm_);
}
bool isExclusive(const bits256_t& b0, const bits256_t& b1)noexcept{
    return _mm256_testz_si256(b0.ymm_, b1.ymm_);
}

#else

using bits128_t = array_long_bits_t<2>;
using bits256_t = array_long_bits_t<4>;

#endif
    
std::ostream& operator <<(std::ostream& ost, const bits128_t& b){
    for(int i = 0; i < 2; ++i){
        ost << BitSet64(b[i]);
    }
    return ost;
}
std::ostream& operator <<(std::ostream& ost, const bits256_t& b){
    for(int i = 0; i < 4; ++i){
        ost << BitSet64(b[i]);
    }
    return ost;
}
/*std::ostream& operator <<(std::ostream& ost, const bits128_t& b){
    ost << std::ios::hex;
    for(int i = 0; i < 2; ++i){
        ost << b[i];
    }
    ost << std::ios::dec;
    return ost;
}
std::ostream& operator <<(std::ostream& ost, const bits256_t& b){
    ost << std::ios::hex;
    for(int i = 0; i < 4; ++i){
        ost << b[i];
    }
    ost << std::ios::dec;
    return ost;
}*/
    
#endif // UTIL_LONG_BITS_HPP_
