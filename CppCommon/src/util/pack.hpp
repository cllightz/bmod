/*
 pack.hpp
 Katsuki Ohto
 */

#ifndef UTIL_PACK_HPP_
#define UTIL_PACK_HPP_

#include "io.hpp"
#include "longBits.hpp"

template<class long_data_t, class data_t, std::size_t N = sizeof(long_data_t) / sizeof(data_t)>
struct alignas(long_data_t) PackInLongRegister{
    static constexpr std::size_t N_REAL = sizeof(long_data_t) / sizeof(data_t);
    using size_t = std::size_t;
    using this_type_t = PackInLongRegister<long_data_t, data_t, N>;
    using array_t = std::array<data_t, N_REAL>;
    
    static_assert(N <= N_REAL, "N should be equal or smaller than real data size.");
    static_assert(sizeof(long_data_t) == sizeof(array_t),
                  "vector and array should be same size.");
    
public:
    /*operator long_data_t()const noexcept{
        return b_;
    }
    operator std::array<data_t, N_REAL>()const noexcept{
        return ar_;
    }*/
    
    long_data_t long_data()const noexcept{
        return b_;
    }
    array_t array()const noexcept{
        return ar_;
    }
    
    data_t& operator [](size_t i){
        return ar_[i];
    }
    const data_t& operator [](size_t i)const{
        return ar_[i];
    }
    
    this_type_t& clear()noexcept{
        b_ = long_data_t::zero();
        return *this;
    }
    this_type_t& fill(const data_t& aval)noexcept{
        ar_.fill(aval);
        return *this;
    }
    
    this_type_t operator+(const this_type_t& p)const noexcept{
        return this_type_t(b_ + p.b_);
    }
    this_type_t operator-(const this_type_t& p)const noexcept{
        return this_type_t(b_ - p.b_);
    }
    this_type_t& operator +=(const this_type_t& p)noexcept{
        b_ += p.b_;
        return *this;
    }
    this_type_t& operator -=(const this_type_t& p)noexcept{
        b_ -= p.b_;
        return *this;
    }

    data_t sum()const noexcept;
    
    template<class data1_t, std::size_t N1>
    constexpr bool operator ==(const PackInLongRegister<long_data_t, data1_t, N1>& p)const noexcept{
        return b_ == p.b_;
    }
    template<class data1_t, std::size_t N1>
    constexpr bool operator !=(const PackInLongRegister<long_data_t, data1_t, N1>& p)const noexcept{
        //cerr << b_ << " " << p.b_ << endl;
        return b_ != p.b_;
    }
    
    constexpr bool holds(const this_type_t& p)const noexcept{
        return holds(b_, p.b);
    }
    
    std::string toString()const{
        return ::toString(ar_);
    }
    
    union{
        array_t ar_;
        long_data_t b_;
    };
    
    constexpr PackInLongRegister(): b_(){}
    constexpr PackInLongRegister(const long_data_t& ab): b_(ab){}
    constexpr PackInLongRegister(const array_t& aar): ar_(aar){}
    constexpr PackInLongRegister(const this_type_t& a): b_(a.b_){}
};

template<class long_data_t, class data_t, std::size_t N, std::size_t M>
data_t sumSubOfPackInLongRegister(const PackInLongRegister<long_data_t, data_t, N>& p)noexcept{
    if(M <= 0){ return data_t(0); }
    return p[M - 1] + sumSubOfPackInLongRegister<long_data_t, data_t, N, (M > 0) ? (M - 1) : 0>(p);
}

template<class long_data_t, class data_t, std::size_t N>
data_t PackInLongRegister<long_data_t, data_t, N>::sum()const noexcept{
    return sumSubOfPackInLongRegister<long_data_t, data_t, N, N>(*this);
}

template<class long_data_t, class data_t, std::size_t N>
std::ostream& operator <<(std::ostream& ost, const PackInLongRegister<long_data_t, data_t, N>& p){
    ost << p.toString();
    return ost;
}

template<class data_t, std::size_t N = sizeof(bits128_t) / sizeof(data_t)>
using Pack128 = PackInLongRegister<bits128_t, data_t, N>;

template<class data_t, std::size_t N = sizeof(bits256_t) / sizeof(data_t)>
using Pack256 = PackInLongRegister<bits256_t, data_t, N>;

#endif // UTIL_PACK_HPP_