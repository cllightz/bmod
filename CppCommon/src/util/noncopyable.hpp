/*
 noncopyable.hpp
 */

#ifndef UTIL_NONCOPYABLE_HPP_
#define UTIL_NONCOPYABLE_HPP_

//noncopyable class
//inherit this class to define noncopyable class
class Noncopyable{
    Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

#endif // UTIL_NONCOPYABLE_HPP_
