//実験

#include<cstring>
#include<unistd.h>
#include<sys/time.h>
#include<ctime>

#include<cmath>
#include<cstdio>
#include<iostream>
#include<fstream>
#include<iomanip>
#include<cassert>
#include<thread>
#include<mutex>
#include<array>
#include<vector>
#include<random>
#include<algorithm>
#include<string>
#include<bitset>

#include"../common_all.h"

using namespace std;

int main(){

    XorShift64 dice((unsigned int)time(NULL));

    Clock cl;
    uint64_t time[16]={0};

    BitArray32<4, 8> ar32;
    BitArray64<4, 16> ar64;
    uint32_t sum[2];
    
    //sum
    {
        ar32.clear();
        sum[0]=0;
        for (int i = 0; i < 100000; ++i){
            ar32 = dice.rand();
            cl.start();
            uint32_t sm=ar32.sum();
            time[4] += cl.stop();
            sum[0]+=sm;
        }
    }
    {
        ar64.clear();
        sum[1]=0;
        for (int i = 0; i < 100000; ++i){
            ar64 = dice.rand();
            cl.start();
            uint64_t sm=ar64.sum();
            time[5] += cl.stop();
            sum[1]+=sm;
        }
    }
    
    //assign
    {
        cl.start();
        for (int i = 0; i < 100000; ++i){
            uint32_t r = dice.rand();
            ar32.replace(r & 7, (r >> 16) & 15);
        }
        time[0] = cl.stop();
    }
    {
        cl.start();
        for (int i = 0; i < 100000; ++i){
            uint32_t r = dice.rand();
            ar64.replace(r & 15, (r >> 16) & 15);
        }
        time[1] = cl.stop();
    }
    
    //swap
    {
        cl.start();
        for (int i = 0; i < 100000; ++i){
            uint32_t r = dice.rand();
            ar32.swap(r & 7, (r >> 16) & 7);
        }
        time[2] = cl.stop();
    }
    {
        cl.start();
        for (int i = 0; i < 100000; ++i){
            uint32_t r = dice.rand();
            ar64.swap(r & 15, (r >> 16) & 15);
        }
        time[3] = cl.stop();
    }
    //rotate
    {
        cl.start();
        for (int i = 0; i < 100000; ++i){
            uint32_t r = dice.rand();
            ar32.rotate(r & 7);
        }
        time[6] = cl.stop();
    }
    {
        cl.start();
        for (int i = 0; i < 100000; ++i){
            uint32_t r = dice.rand();
            ar64.rotate(r & 15);
        }
        time[7] = cl.stop();
    }

    cerr << "assign()" << endl;
    cerr << "BitArray32 : " << time[0] << " clock" << endl;
    cerr << "BitArray64 : " << time[1] << " clock" << endl;
    cerr << "swap()" << endl;
    cerr << "BitArray32 : " << time[2] << " clock" << endl;
    cerr << "BitArray64 : " << time[3] << " clock" << endl;
    cerr << "sum()" << endl;
    cerr << "BitArray32 : " << time[4] << " clock" << endl;
    cerr << "BitArray64 : " << time[5] << " clock" << endl;
    cerr << "rotate()" << endl;
    cerr << "BitArray32 : " << time[6] << " clock" << endl;
    cerr << "BitArray64 : " << time[7] << " clock" << endl;
    cerr << ar32 << endl;
    cerr << ar64 << endl;
    cerr << sum[0]<<" "<<sum[1]<< endl;
    
    //value test
    ar32.set_sequence();
    for(int i=ar32.size()-1;i>0;--i){
        ar32.swap(dice.rand()%(i+1),i);
    }
    cerr << ar32 << invert(ar32) << endl;
    
    ar64.set_sequence();
    for(int i=ar64.size()-1;i>0;--i){
        ar64.swap(dice.rand()%(i+1),i);
    }
    cerr << ar64 << invert(ar64) << endl;
    
    
    return 0;
}