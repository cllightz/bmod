/*
 random_test.cc
 Katsuki Ohto
 */

// 確率分布のテスト

#include <cstring>
#include <unistd.h>
#include <sys/time.h>
#include <ctime>

#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <random>
#include <algorithm>
#include <string>
#include <array>

#include "../defines.h"
#include "../util/random.hpp"
#include "../util/xorShift.hpp"

using namespace std;

XorShift64 dice;
Clock cl;

template<class distribution_t>
int testDistribution(distribution_t& dist){
    constexpr int N = 1500000;
    double sum = 0, sum2 = 0;
    constexpr int sigma = 3; // この範囲まで分布調べる
    constexpr double breadth = 0.4; // 度数分布を標準偏差のどの程度まで見るか
    constexpr int arraySize = sigma / breadth * 2 + 1;
    std::array<uint64_t, arraySize> count = {0};
    
    uint64_t time = 0;
    
    // 統計量
    cerr << dist.toString() << " : " << endl;
    
    cerr << "mean = " << dist.mean() << endl;
    cerr << "variance = " << dist.var() << endl;
    cerr << "median = " << dist.med() << endl;
    cerr << "mode = " << dist.mod() << endl;
    
    const double mean = dist.mean();
    const double std = sqrt(dist.var());
    
    const double lb = mean - std * sigma;
    const double ub = mean + std * sigma;
    
    // 乱数生成
    for(int i = 0; i < N; ++i){
        double r;
        cl.start();
        r = dist.rand(&dice);
        time += cl.stop();
        sum += r;
        sum2 += r * r;
        if(lb < r && r < ub){
            count[int((r - mean) / std / breadth + (arraySize / 2))] += 1;
        }
    }
    cerr << "generation time = " << time / (double)N << endl;
    cerr << "generation mean = " << sum / N << endl;
    cerr << "generation variance = " << ((sum2 / N) - pow(sum / N, 2)) << endl;

    cerr << "distribution = " << endl;
    for(int i = 0; i < arraySize; ++i){
        cerr << count[i] / std / breadth / N << " ";
        //cerr << std::string(20 * count[i] / std / breadth / N, '*') << endl;
    }cerr << endl;
    return 0;
}

int main(){

    dice.srand((unsigned int)time(NULL));
    
    ExponentialDistribution exp0(2);
    testDistribution(exp0);
    
    GammaDistribution gamma0(2);
    testDistribution(gamma0);
    
    InverseGammaDistribution igamma0(2);
    //testDistribution(igamma0);
    
    BetaDistribution beta0(2, 2);
    testDistribution(beta0);
    
    BetaDistribution beta1(0.5, 0.5);
    testDistribution(beta1);
    
    NormalDistribution<double> normal0(0, 2);
    testDistribution(normal0);
    
    LogNormalDistribution<double> lnormal0(0, 1.5);
    testDistribution(lnormal0);
    
    WeibullDistribution<double> weibull0(1, 1);
    testDistribution(weibull0);
    
    ChiSquaredDistribution<double> csq0(4);
    testDistribution(csq0);
    
    TDistribution<double> t0(3);
    testDistribution(t0);
    
    return 0;
}