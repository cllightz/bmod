/*
 index.hpp
 Katsuki Ohto
 */

#ifndef UTIL_INDEX_HPP_
#define UTIL_INDEX_HPP_

#include <array>

// インデックス付け
template<int D>
struct TensorIndex{
    std::array<int, D + 1> size_;
    
    template<typename ... args_t>
    TensorIndex(args_t ... args)
    :size_(){
        fill(args...);
    }
    
    void fill_sub(int d){
        size_[d] = 1;
    }
    void fill_sub(int d, int n){
        fill_sub(d + 1);
        size_[d] = n;
    }
    template<typename ... args_t>
    void fill_sub(int d, int n, args_t ... args){
        fill_sub(d + 1, args...);
        size_[d] = size_[d + 1] * n;
    }
    template<typename ... args_t>
    void fill(args_t ... args){
        fill_sub(0, args...);
    }
    
    constexpr int get_sub(int d){
        return 0;
    }
    constexpr int get_sub(int d, int i){
        return i;
    }
    template<typename ... args_t>
    int get_sub(int d, int i, args_t ... args){
        return get_sub(d + 1, args...) + i * size_[d];
    }
    template<typename ... args_t>
    int get(args_t ... args){
        return get_sub(1, args...);
    }
};

// 配列インスタンスを持たないバージョン
template<int L, int ... shape_t>
struct TensorIndexTypeSub{
    template<typename ... args_t>
    static constexpr int get(int i, args_t ... args){
        return i * size() + TensorIndexTypeSub<shape_t...>::get(args...);
    }
    static constexpr int size(){
        return L * TensorIndexTypeSub<shape_t...>::size();
    }
};

template<int L>
struct TensorIndexTypeSub<L>{
    template<typename ... args_t>
    static constexpr int get(int i, int j){
        return i * size() + j;
    }
    static constexpr int size(){ return L; }
};

template<int L, int ... shape_t>
struct TensorIndexType{
    template<typename ... args_t>
    static constexpr int get(args_t ... args){
        return TensorIndexTypeSub<shape_t...>::get(args...);
    }
    static constexpr int size(){
        return L * TensorIndexType<shape_t...>::size();
    }
};

template<int L>
struct TensorIndexType<L>{
    static constexpr int get(int i){ return i; }
    static constexpr int size(){ return L; }
};

#endif // UTIL_INDEX_HPP_