/*
 selection_test.cc
 Katsuki Ohto
 */

#include <iostream>

#include "../defines.h"
#include "../util/io.hpp"
#include "../util/selection.hpp"

using namespace std;

template<class selector_t>
int testProbCalculation(selector_t& selector){
    // 確率計算のテスト
    Clock cl;
    uint64_t calcTime;
    cl.start();
    selector.calc_prob();
    calcTime = cl.stop();
    cerr << toString(selector.score, selector.moves) << endl;
    cerr << "calculation time = " << calcTime << endl;
    return 0;
}

int main(int argc, char* argv[]){

    // 得点セット
    std::vector<double> test0 = {0, -1, -2, -3, -4, -5, -6, -7};
    std::vector<double> test1 = {0, 0, -2, -2, -4, -4, -6, -6};
    std::vector<double> test2 = {0, -2, -6, -6, -6, -6, -6, -6};
    std::vector<double> test3 = {0, -sqrt(1), -sqrt(2), -sqrt(3), -sqrt(4), -sqrt(5), -sqrt(6), -sqrt(7)};
    
    std::vector<std::vector<double>> tests = {test0, test1, test2, test3};
    
    for(const auto& test : tests){
        
        // softmax
        {
            auto tmp_test = test;
            cerr << "Softmax : " << endl;
            SoftmaxSelector selector(tmp_test.data(), tmp_test.size(), 1.0);
            testProbCalculation(selector);
        }
        
        // biased softmax
        {
            auto tmp_test = test;
            cerr << "Biased Softmax : " << endl;
            BiasedSoftmaxSelector selector(tmp_test.data(), tmp_test.size(), 1.1, 0.22, 2);
            testProbCalculation(selector);
        }
        
        // exp-biased softmax
        {
            auto tmp_test = test;
            cerr << "Exp-Biased Softmax : " << endl;
            ExpBiasedSoftmaxSelector selector(tmp_test.data(), tmp_test.size(), 1.1, 0.22, 1 / log(2));
            testProbCalculation(selector);
        }
        
        // sparsemax
        {
            auto tmp_test = test;
            cerr << "Sparsemax : " << endl;
            SparsemaxSelector selector(tmp_test.data(), tmp_test.size());
            testProbCalculation(selector);
        }
    }
    
    
    return 0;
}