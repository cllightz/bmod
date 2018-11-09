#ifndef UTIL_STATICVECTOR_HPP_
#define UTIL_STATICVECTOR_HPP_

#include <cassert>

// std::vector や java の ArrayList の代替としてすぐ使いたい場合用のサイズ固定配列

template<typename T, int kSize>
class StaticVector : public std::array<T, kSize>{
    public:
    
    StaticVector():
    size_(0){}
    
    std::size_t size()const noexcept{ return size_; }
    std::size_t length()const noexcept{ return size_; }
    
    static std::size_t max_size()noexcept{
        return std::array<T, kSize>::size();
    }
    static std::size_t max_length()noexcept{
        return std::array<T, kSize>::size();
    }
    
    StaticVector& push_back(const T& rhs){
        (*this)[size_++] = rhs;
        return *this;
    }
    StaticVector& add(const T& rhs){
        return push_back(rhs);
    }
    StaticVector& pop_back(){
        --size_;
        return *this;
    }
    
    private:
    int size_;
    
};

        
#endif // UTIL_STATICVECTOR_HPP_