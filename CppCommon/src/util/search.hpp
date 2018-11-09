/*
 search.hpp
 Katsuki Ohto
 */

#ifndef UTIL_SEARCH_HPP_
#define UTIL_SEARCH_HPP_

#include <cfloat>
#include <array>

#include "../defines.h"

// 探索ソルバー

// 2分探索

template<class function_t>
double biSearch(const int trials, double a, double b, const function_t& f){
    // 純粋な完全2分探索(単調増加の連続関数)
    for(int t = 0; t < trials; ++t){
        double m = (a + b) / 2;
        if(f(m) > 0){
            b = m;
        }else{
            a = m;
        }
    }
    return (a + b) / 2;
}

template<class boolFunction_t>
double boolBiSearch(const int trials, double a, double b, const boolFunction_t& f){
    // 純粋な完全2分探索(ブール値)
    for(int t = 0; t < trials; ++t){
        double m = (a + b) / 2;
        if(f(m)){
            b = m;
        }else{
            a = m;
        }
    }
    return (a + b) / 2;
}


template<class function_t>
double weightedBiSearch(const int trials, double a, double b, const function_t& f){
    // 直線的過程の元での2分探索
    // 端点でも関数fが定義されている必要がある
    double va(f(a)), vb(f(b));
    for(int t = 0; t < trials; ++t){
        double m = (a * vb + b * (-va)) / (vb - va);
        //cerr << a << "," << b << endl;
        double vm = f(m);
        if(vm == 0){ return m; }
        if(vm > 0){
            b = m;
            vb = vm;
        }else{
            a = m;
            va = vm;
        }
    }
    return (a + b) / 2;
}

// 3分探索
// 上に凸であると仮定する

template<class function_t>
std::array<double, 2> triSearch(const int trials, double a, double b, const function_t& f){
    
    // 純粋な3分探索
    for(int t = 0; t < trials; ++t){
        //double m0 = (a * 2 + b) / 3;
        //double m1 = (a + b * 2) / 3;
        double m0 = (a * 1.01 + b) / 2.01;
        double m1 = (a + b * 1.01) / 2.01;
        if(f(m0) < f(m1)){
            a = m0;
        }else{
            b = m1;
        }
    }
    double r = (a + b) / 2;
    double rv = f(r);
    std::array<double, 2> ar;
    ar[0] = r; ar[1] = rv;
    return ar;
}

/*template<class callback_t>
double triSearch(const int trials, double a, double b, const callback_t& callback){
    
    // 純粋な3分探索
    for(int t = 0; t < trials; ++t){
        double m0 = (a * 2 + b) / 3;
        double m1 = (a + b * 2) / 3;
        if(callback(m0) < callback(m1)){
            a = m0;
        }else{
            b = m1;
        }
    }
    return (a + b) / 2;
}*/

class BiSolver{
public:
    double alpha()const noexcept{return alpha_;}
    double beta()const noexcept{return beta_;}
    double mid()const noexcept{ return (alpha_ + beta_) / 2; }
    
    double play()const noexcept{
        return mid();
    }
    
    void feed(bool on)noexcept{
        if (on){
            beta_ = mid();
        }
        else{
            alpha_ = mid();
        }
    }
    
    void feed(double val, bool on)noexcept{
        if (alpha_ <= val && val <= beta_){
            if (on){
                beta_ = val;
            }
            else{
                alpha_ = val;
            }
        }
    }
    
    double answer()const noexcept{
        return mid();
    }
    
    BiSolver(double aa, double ab)
    :originalAlpha_(aa), originalBeta_(ab),
    alpha_(aa), beta_(ab)
    {}
    
private:
    double originalAlpha_, originalBeta_;
    double alpha_, beta_;
};

// 最大値配列
template<class container_t>
std::vector<int> getBestIndexVector(const container_t& dat, const int size){
    std::vector<int> v;
    double best = -DBL_MAX;
    for(int i = 0; i < size; ++i){
        const auto& d = dat[i];
        if(d < best){
        }else if(d > best){
            v.clear();
            v.push_back(i);
            best = d;
        }else{
            v.push_back(i);
        }
    }
    return v;
}

#endif //UTIL_SEARCH_HPP_ 