//実験

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
#include "../util/bitPartition.hpp"
#include "../util/xorShift.hpp"
#include "../util/BitSet.hpp"

using namespace std;

int main(){

    XorShift64 dice((unsigned int)time(NULL));
    Clock cl;

    constexpr int N = 500000;

    uint64_t val = 0;

    uint64_t time[2] = { 0 };

    int chance[2][64] = { 0 };
    int chosen[2][64] = { 0 };


    for (int i = 0; i < N; ++i){
        int numAll = 2 + dice.rand() % 63;
        int numDist = 1 + dice.rand() % (numAll-1);

        uint64_t all=pickNBits64(-1LL, numAll, 64-numAll, &dice);

        uint64_t ans;

        cl.start();

        {
            ans = 0;
            int slot[64], cnt = 0;
            int *pslot = slot;
            iterate(BitSet64(all), [pslot, &cnt](int idx)->void{
                pslot[cnt] = idx;
                ++cnt;
            });
            for (int i = 0; i < numDist; ++i){
                int num = dice.rand() % cnt;
                ans |= (1ULL << slot[num]);
                --cnt;
                slot[num] = slot[cnt];
            }
        }
        time[1] += cl.stop();
        val ^= ans;
        //for (int i = 0; i < numAll; ++i){ chance[1][i]++; }
        //chosen[]

        cl.start();
        {
            ans = pickNBits64(all, numDist, numAll - numDist, &dice);
        }
        time[0] += cl.stop();
        val ^= ans;
        

    }

    cerr << time[0] / N << endl;
    cerr << time[1] / N << endl;
    cerr << val << endl;

    return 0;
}