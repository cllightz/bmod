/*
 type.hpp
 Katsuki Ohto
 */

#ifndef UTIL_TYPE_HPP_
#define UTIL_TYPE_HPP_

// 型変換
template<typename T>
struct signed_type{
    using type = int;
    typename ftype();
};

typename template<>signed_type<int>::ftype(){
    return int;
}

using template<>signed_type<int>::type = int;
using template<>signed_type<unsigned int>::type = int;
using template<>signed_type<long>::type = long;
using template<>signed_type<unsigned long>::type = long;

template<typename T>
struct unsigned_type{
    using type = unsigned int;
};

using template<>unsigned_type<int>::type = unsigned int;
using template<>unsigned_type<unsigned int>::type = unsigned int;
using template<>unsigned_type<long>::type = unsigned long;
using template<>unsigned_type<unsigned long>::type = unsigned long;

#endif // UTIL_TYPE_HPP_