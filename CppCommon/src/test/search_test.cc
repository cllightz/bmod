/*
 search_test.cc
 Katsuki Ohto
 */

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

#include "../defines.h"
#include "../util/search.hpp"

using namespace std;

int main(){

    const int T = 10000;
    
    auto f0 = [](double x)->double{ return 2 * x + 1; };
    auto f1 = [](double x)->double{ return -(x - 2)*(x - 2) + 1; };
    auto f2 = [](double x)->double{ return exp(x) - 4; };
    
    // 二分探索
    cerr << "*** search f(x) = 0 ***" << endl;
    
    cerr << "y = 2x + 1 [-10, 10]" << endl;
    
    cerr << "pure     : " << biSearch(T, -10, 10, f0) << endl;
    cerr << "weighted : " << weightedBiSearch(T, -10, 10, f0) << endl;
    
    cerr << "y = e^x - 4 [-10, 10]" << endl;
    
    cerr << "pure     : " << biSearch(T, -10, 10, f2) << endl;
    cerr << "weighted : " << weightedBiSearch(T, -10, 10, f2) << endl;
    
    // 三分探索
    cerr << endl << "*** search argmax, max f(x) ***" << endl;
    
    cerr << "y = -(x - 2)^2 + 1 [-10, 10]" << endl;
    
    auto ap = triSearch(T, -10, 10, f1);
    cerr << "pure     : " << ap[1] << " in " << ap[0] << endl;

    return 0;
}