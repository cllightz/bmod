/*
linearlyInerpolatedArray.hpp
Katsuki Ohto
*/

#ifndef UTIL_LINEZONE_HPP_
#define UTIL_LINEZONE_HPP_

#include <iostream>
#include <cstdio>
#include <array>
#include <tuple>
#include <initializer_list>
#include <list>

#include "../defines.h"

template<typename T>
class LineZone{

public:
    typedef T type;

    std::size_t size()const noexcept{ return zone_.size(); }
    const T& minBound()const noexcept{ return min_; }
    const T& maxBound()const noexcept{ return max_; }
    bool any()const noexcept{ return (size() != 0); }

        /*
        std::string toDebugString()const noexcept{
        std::ostringstream oss;
        oss<<"size = "<<size()<<std::endl;
        oss<<"array size = "<<arraySize()<<std::endl;
        oss<<"step = "<<step()<<std::endl;
        oss<<"min key = "<<minKey()<<std::endl;
        oss<<"max key = "<<maxKey()<<std::endl;
        return oss.str();
        }
        */

        std::string toString()const noexcept{
        std::ostringstream oss;
        if (any()){
            for (auto itr = zone_.begin(); itr != zone_.end(); ++itr){
                oss << "[ " << std::get<0>(*itr) << ", " << std::get<1>(*itr) << " ]";
            }
        }
        else{
            oss << "none";
        }
        return oss.str();
    }

        void limit_over(T a)noexcept{
        for (auto itr = zone_.begin(); itr != zone_.end();){
            if (a > std::get<1>(*itr)){
                itr = zone_.erase(itr);
                continue;
            }
            if (a > std::get<0>(*itr)){
                std::get<0>(*itr) = a;
            }
            ++itr;
        }
    }

        void limit_under(T b)noexcept{
        for (auto itr = zone_.begin(); itr != zone_.end();){
            if (b < std::get<0>(*itr)){
                itr = zone_.erase(itr);
                continue;
            }
            if (b < std::get<1>(*itr)){
                std::get<1>(*itr) = b;
            }
            ++itr;
        }
    }

        void limit_in(T a, T b)noexcept{
        assert(a <= b);
        for (auto itr = zone_.begin(); itr != zone_.end();){
            if (b < std::get<0>(itr)){
                itr = zone_.erase(*itr);
                continue;
            }
            if (a > std::get<1>(*itr)){
                itr = zone_.erase(itr);
                continue;
            }
            if (a > std::get<0>(*itr)){
                std::get<0>(*itr) = a;
            }
            if (b < std::get<1>(*itr)){
                std::get<1>(*itr) = b;
            }
            ++itr;
        }
    }

        void limit_out(T a, T b)noexcept{
        assert(a <= b);
        for (auto itr = zone_.begin(); itr != zone_.end();){
            if (b < std::get<0>(*itr)){
                ++itr; continue;
            }
            if (a > std::get<1>(*itr)){
                ++itr; continue;
            }
            if (a >= std::get<0>(*itr) && b <= std::get<1>(*itr)){
                std::get<1>(*itr) = a;
                auto oitr = itr; ++itr;
                zone_.insert(itr, { b, std::get<1>(*oitr) });
                continue;
            }
            if (a > std::get<0>(*itr)){
                std::get<1>(*itr) = a;
            }
            if (b < std::get<1>(*itr)){
                std::get<1>(*itr) = b;
            }
            if (a < std::get<0>(*itr) && b > std::get<1>(*itr)){
                itr = zone_.erase(itr);
                continue;
            }
            ++itr;
        }
    }

        void compress(double rate)noexcept{
        for (auto itr = zone_.begin(); itr != zone_.end();){
            T length = std::get<1>(*itr) - std::get<0>(*itr);
            T move = length*(1 - rate) / 2;
            std::get<0>(*itr) += move;
            std::get<1>(*itr) -= move;
            ++itr;
        }
    }

        const std::array<T, 2>& searchMaxZone()const noexcept{
        auto maxIterator = zone_.begin();
        auto itr = maxIterator;
        T maxLength = std::get<1>(*itr) - std::get<0>(*itr);
        ++itr;
        for (; itr != zone_.end();){
            T length = std::get<1>(*itr) - std::get<0>(*itr);
            if (length > maxLength){
                maxIterator = itr;
                maxLength = length;
            }
            ++itr;
        }
        return *maxIterator;
    }

        LineZone(T amin, T amax)
        :min_(amin), max_(amax)
    {
        zone_.push_back({ min_, max_ });
    }

    ~LineZone(){}

private:
    T min_, max_;
    std::list<std::array<T, 2>> zone_;
};


#endif // UTIL_LINEZONE_HPP_