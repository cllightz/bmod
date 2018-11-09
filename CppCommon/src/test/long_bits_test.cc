/*
 long_bits_test.cc
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
#include "../util/bitOperation.hpp"
#include "../util/bitSet.hpp"
#include "../util/xorShift.hpp"
#include "../util/pack.hpp"

using namespace std;

XorShift64 dice;
Clock cl;

template<class long_bits_t>
std::string toBitsString(const long_bits_t& b){
    std::ostringstream oss;
    for(int i = 0; i < sizeof(long_bits_t) / 8; ++i){
        oss << BitSet64(b[i]);
    }
    return oss.str();
}

template<class long_bits_t>
int testOperators(){
    // 演算子が正しく設定されているかテスト
    long_bits_t a, b;
    int k;
    for(int i = 0; i < sizeof(long_bits_t) / 8; ++i){
        a[i] = dice.rand();
        b[i] = dice.rand();
    }
    
    std::cerr << "a" << endl << toBitsString(a) << std::endl;
    std::cerr << "b" << endl << toBitsString(b) << std::endl;
    
    // 1項演算子
    long_bits_t c = ~a;
    std::cerr << "~a" << endl << toBitsString(c) << std::endl;
    
    // 2項演算子
    std::cerr << "a & b" << endl << toBitsString(a & b) << std::endl;
    c = a; c &= b;
    std::cerr << "a &= b" << endl << toBitsString(c) << std::endl;
    
    std::cerr << "a | b" << endl << toBitsString(a | b) << std::endl;
    c = a; c |= b;
    std::cerr << "a |= b" << endl << toBitsString(c) << std::endl;
    
    std::cerr << "a ^ b" << endl << toBitsString(a ^ b) << std::endl;
    c = a; c ^= b;
    std::cerr << "a ^= b" << endl << toBitsString(c) << std::endl;
    
    std::cerr << "a & ~b" << endl << toBitsString(a & ~b) << std::endl;
    std::cerr << "~b & a" << endl << toBitsString(~b & a) << std::endl;
    
    std::cerr << "a + b" << endl << toBitsString(a + b) << std::endl;
    c = a; c += b;
    std::cerr << "a += b" << endl << toBitsString(c) << std::endl;
    
    std::cerr << "a - b" << endl << toBitsString(a - b) << std::endl;
    c = a; c -= b;
    std::cerr << "a -= b" << endl << toBitsString(c) << std::endl;
    
    std::cerr << "a << 1" << endl << toBitsString(a << 1) << std::endl;
    c = a; c <<= 1;
    std::cerr << "a <<= 1" << endl << toBitsString(c) << std::endl;
    
    std::cerr << "a >> 1" << endl << toBitsString(a >> 1) << std::endl;
    c = a; c >>= 1;
    std::cerr << "a >>= 1" << endl << toBitsString(c) << std::endl;
    
    long_bits_t d = a;
    std::cerr << "a.sum64()" << endl << BitSet64(d.sum64()) << std::endl;
    
    return 0;
}

/*int testPext(){
    // pextの独自実装とCPU命令の比較
    // BMIが無い場合には比較の意味無し
    constexpr int N = 500000;
    uint64_t val[2] = { 0 };
    uint64_t time[2] = { 0 };
    
    // pext
    for (int i = 0; i < N; ++i){
        int numMask = 1 + dice.rand() % 64;
        
        uint64_t bits = dice.rand();
        uint64_t mask = pickNBits64(-1LL, numMask, 64 - numMask, &dice);
        
        uint64_t ans;
        
        cl.start();
        ans = pextNoBMI2(bits, mask);
        time[1] += cl.stop();
        val[1] ^= ans;
        
        cl.start();
        ans = pext(bits, mask);
        time[0] += cl.stop();
        val[0] ^= ans;
    }
    cerr << "pext" << endl;
    cerr << "bmi2    : " << time[0] / N << endl;
    cerr << "no mbi2 : " << time[1] / N << endl;
    cerr << val[0] << " <-> " << val[1] << endl;
    
    if(val[0] != val[1]){
        cerr << "failed pext test." << endl;
        return -1;
    }
    cerr << "passed pext test." << endl;
    return 0;
}

int testPdep(){
    // pextの独自実装とCPU命令の比較
    // BMIが無い場合には比較の意味無し
    constexpr int N = 500000;
    uint64_t val[2] = { 0 };
    uint64_t time[2] = { 0 };
    // pdep
    for (int i = 0; i < N; ++i){
        int numMask = 1 + dice.rand() % 64;
        
        uint64_t bits = dice.rand();
        uint64_t mask = pickNBits64(-1LL, numMask, 64 - numMask, &dice);
        
        uint64_t ans;
        
        cl.start();
        ans = pdepNoBMI2(bits, mask);
        time[1] += cl.stop();
        val[1] ^= ans;
        
        cl.start();
        ans = pdep(bits, mask);
        time[0] += cl.stop();
        val[0] ^= ans;
    }
    
    cerr << "pdep" << endl;
    cerr << "bmi2    : " << time[0] / N << endl;
    cerr << "no mbi2 : " << time[1] / N << endl;
    cerr << val[0] << " <-> " << val[1] << endl;
    
    if(val[0] != val[1]){
        cerr << "failed pdep test." << endl;
        return -1;
    }
    cerr << "passed pdep test." << endl;
    return 0;
}

int testLowestNBits(){
    constexpr int N = 500000;
    uint64_t val[2] = {0};
    uint64_t time[2] = {0};
    
    for (int i = 0; i < N; ++i){
        uint64_t bits;
        do{
            bits = dice.rand();
        }while(!bits);
        int num = 1 + dice.rand() % countBits(bits);
        
        uint64_t ans;
        
        cl.start();
        ans = lowestNBitsNoBMI2(bits, num);
        time[1] += cl.stop();
        val[1] ^= ans;
        
        cl.start();
        ans = lowestNBitsBMI2(bits, num);
        time[0] += cl.stop();
        val[0] ^= ans;
    }
    cerr << "lowest N bits" << endl;
    cerr << "bmi2    : " << time[0] / N << endl;
    cerr << "no mbi2 : " << time[1] / N << endl;
    cerr << val[0] << " <-> " << val[1] << endl;
    
    if(val[0] != val[1]){
        cerr << "failed lowest N bits test." << endl;
        return -1;
    }
    cerr << "passed lowest N bits test." << endl;
    return 0;
}

int testHighestNBits(){
    constexpr int N = 500000;
    uint64_t val[2] = {0};
    uint64_t time[2] = {0};
    
    for (int i = 0; i < N; ++i){
        uint64_t bits;
        do{
            bits = dice.rand();
        }while(!bits);
        int num = 1 + dice.rand() % countBits(bits);
        
        uint64_t ans;
        
        cl.start();
        ans = highestNBitsNoBMI2(bits, num);
        time[1] += cl.stop();
        val[1] ^= ans;
        
        cl.start();
        ans = highestNBitsBMI2(bits, num);
        time[0] += cl.stop();
        val[0] ^= ans;
    }
    cerr << "highest N bits" << endl;
    cerr << "bmi2    : " << time[0] / N << endl;
    cerr << "no mbi2 : " << time[1] / N << endl;
    cerr << val[0] << " <-> " << val[1] << endl;
    
    if(val[0] != val[1]){
        cerr << "failed highest N bits test." << endl;
        return -1;
    }
    cerr << "passed highest N bits test." << endl;
    return 0;
}

int testNthLowestBit(){
    constexpr int N = 500000;
    uint64_t val[2] = {0};
    uint64_t time[2] = {0};
    
    for (int i = 0; i < N; ++i){
        uint64_t bits;
        do{
            bits = dice.rand();
        }while(!bits);
        int num = 1 + dice.rand() % countBits(bits);
        
        uint64_t ans;
        
        cl.start();
        ans = NthLowestBitNoBMI2(bits, num);
        time[1] += cl.stop();
        val[1] ^= ans;
        
        cl.start();
        ans = NthLowestBitBMI2(bits, num);
        time[0] += cl.stop();
        val[0] ^= ans;
    }
    cerr << "N-th lowest bit" << endl;
    cerr << "bmi2    : " << time[0] / N << endl;
    cerr << "no mbi2 : " << time[1] / N << endl;
    cerr << val[0] << " <-> " << val[1] << endl;
    
    if(val[0] != val[1]){
        cerr << "failed N-th lowest bit test." << endl;
        return -1;
    }
    cerr << "passed N-th lowest bit test." << endl;
    return 0;
}

int testNthHighestBit(){
    constexpr int N = 500000;
    uint64_t val[2] = {0};
    uint64_t time[2] = {0};
    
    for (int i = 0; i < N; ++i){
        uint64_t bits;
        do{
            bits = dice.rand();
        }while(!bits);
        int num = 1 + dice.rand() % countBits(bits);
        
        uint64_t ans;
        
        cl.start();
        ans = NthHighestBitNoBMI2(bits, num);
        time[1] += cl.stop();
        val[1] ^= ans;
        
        cl.start();
        ans = NthHighestBitBMI2(bits, num);
        time[0] += cl.stop();
        val[0] ^= ans;
    }
    cerr << "N-th highest bit" << endl;
    cerr << "bmi2    : " << time[0] / N << endl;
    cerr << "no mbi2 : " << time[1] / N << endl;
    cerr << val[0] << " <-> " << val[1] << endl;
    
    if(val[0] != val[1]){
        cerr << "failed N-th highest bit test." << endl;
        return -1;
    }
    cerr << "passed N-th highest bit test." << endl;
    return 0;
}*/

int main(){

    dice.srand((unsigned int)time(NULL));
    
    if(testOperators<bits128_t>()){
        cerr << "bits128_t : failed operators test." << endl;
        return -1;
    }
    if(testOperators<bits256_t>()){
        cerr << "bits256_t : failed operators test." << endl;
        return -1;
    }
    
    return 0;
}