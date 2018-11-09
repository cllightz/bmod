/*
 bitPartition.hpp
 Katsuki Ohto
 */

#ifndef UTIL_BIT_PARTITION_HPP_
#define UTIL_BIT_PARTITION_HPP_

#include <cstdio>
#include <cassert>

#include "../defines.h"

#include "container.hpp"
#include "bitArray.hpp"

//ビット列に対するランダム操作

/**************************少数ビット取り出し**************************/

template<class dice64_t>
static uint64_t pop1Bit64NoPdep(uint64_t *const arg, dice64_t *const dice){
    uint64_t tmp = *arg, tmp2;
    while (1){
        tmp2 = tmp & dice->rand();
        if (tmp2 != 0ULL){
            if (tmp2 & (tmp2 - 1ULL)){ // 2つ以上
                tmp = tmp2;
            }
            else{
                break;
            }
        }
    }
    (*arg) -= tmp2;
    return tmp2;
}

template<class dice64_t>
static uint64_t pick1Bit64NoPdep(uint64_t arg, dice64_t *const dice){
    uint64_t tmp;
    while (1){
        tmp = arg & dice->rand();
        if (tmp != 0ULL){
            if (tmp & (tmp - 1ULL)){ // 2つ以上
                arg = tmp;
            }
            else{
                break;
            }
        }
    }
    return tmp;
}

#if defined (HAVE_BMI2)

template<class dice64_t>
static uint64_t pick1Bit64(uint64_t arg, int N, dice64_t *const pdice){
    return pdep(1ULL << (pdice->rand() % N), arg);
}
template<class dice64_t>
static uint64_t pop1Bit64(uint64_t *const parg, int N, dice64_t *const pdice){
    uint64_t ret = pdep(1ULL << (pdice->rand() % N), *parg);
    *parg -= ret;
    return ret;
}

template<class dice64_t>
static uint64_t pick1Bit64(uint64_t arg, dice64_t *const pdice){
    return pick1Bit64(arg, countBits(arg), pdice);
}
template<class dice64_t>
static uint64_t pop1Bit64(uint64_t *const parg, dice64_t *const pdice){
    return pop1Bit64(parg, countBits(*parg), pdice);
}

#else

template<class dice64_t>
static uint64_t pick1Bit64(uint64_t arg, int N, dice64_t *const pdice){
    return pick1Bit64NoPdep(arg, pdice);
}
template<class dice64_t>
static uint64_t pop1Bit64(uint64_t *const parg, int N, dice64_t *const pdice){
    return pop1Bit64NoPdep(parg, pdice);
}

template<class dice64_t>
static uint64_t pick1Bit64(uint64_t arg, dice64_t *const pdice){
    return pick1Bit64NoPdep(arg, pdice);
}
template<class dice64_t>
static uint64_t pop1Bit64(uint64_t *const parg, dice64_t *const pdice){
    return pop1Bit64NoPdep(parg, pdice);
}

#endif // HAVE_BMI2



/**************************2分割**************************/

template<class dice64_t>
static uint64_t pickNBits64NoPdep(uint64_t arg, const int argN0, const int argN1, dice64_t *const pdice){
    // argからランダムにN0ビット抽出する
    // 最初はN0 + N1ビットある必要あり
    assert((int)countBits(arg) == argN0 + argN1);
    
    uint64_t res = 0ULL;
    int N0 = argN0;
    int N1 = argN1;
    
    if (N0 < N1){
        if (N0 == 0){ return res; }
        if (N0 == 1){ return pick1Bit64NoPdep(arg, pdice); }
    }
    else{
        if (N1 == 0){ return arg; }
        if (N1 == 1){ return arg - pick1Bit64NoPdep(arg, pdice); }
    }
    
    int NDist;
    uint64_t dist;
    
    while (1){
        
        dist = arg & pdice->rand();
        NDist = countBits64(dist);
        
        // まずは一致チェック
        if (NDist == N0){
            res |= dist;
            assert((int)countBits(res) == argN0);
            return res;
        }
        if (NDist == N1){
            res |= (arg - dist);
            assert((int)countBits(res) == argN0);
            return res;
        }
        
        if (NDist < N0){
            if (NDist < N1){
                // NDistが小さく、鋭い
                if (N0 > N1){
                    res |= dist;
                    N0 -= NDist;
                }else{
                    N1 -= NDist;
                }
                arg -= dist;
            }else{
                // 鈍い
                int NRDist = N0 + N1 - NDist;
                if (NDist > NRDist){
                    // NDistが大きい
                    N0 -= NDist;
                    res |= dist;
                    arg -= dist;
                }else{
                    // NRDistが大きい
                    N0 -= NRDist;
                    res |= (arg - dist);
                    arg = dist;
                }
            }
        }else{
            if (NDist < N1){
                // 鈍い
                int NRDist = N0 + N1 - NDist;
                if (NDist > NRDist){
                    // NDistが大きい
                    N1 -= NDist;
                    arg -= dist;
                }else{
                    // NRDistが大きい
                    N1 -= NRDist;
                    arg = dist;
                }
            }else{
                // NDistが大きく、鋭い
                if (N0 > N1){
                    res |= (arg - dist);
                    N0 = NDist - N1;
                }else{
                    N1 = NDist - N0;
                }
                arg = dist;
            }
        }
        assert((int)countBits(arg) == N0 + N1);
    }
    UNREACHABLE;
}

template<class dice64_t>
static uint64_t pickNBits64(uint64_t arg, const int argN0, const int argN1, dice64_t *const pdice){
    // argからランダムにN0ビット抽出する
    // 最初はN0+N1ビットある必要あり
    assert((int)countBits(arg) == argN0 + argN1);
    
    if (argN0 < argN1){
        if (argN0 == 0){ return 0ULL; }
        if (argN0 == 1){ return pick1Bit64(arg, 1 + argN1, pdice); }
    }
    else{
        if (argN1 == 0){ return arg; }
        if (argN1 == 1){ return arg - pick1Bit64(arg, argN0 + 1, pdice); }
    }
    
    uint64_t res = 0ULL;
    int N0 = argN0;
    int N1 = argN1;
    
    int NDist;
    uint64_t dist;
    
    while (1){
        dist = arg & pdice->rand();
        NDist = countBits64(dist);
        
        // まずは一致チェック
        if (NDist == N0){
            res |= dist;
            assert((int)countBits(res) == argN0);
            return res;
        }
        if (NDist == N1){
            res |= (arg - dist);
            assert((int)countBits(res) == argN0);
            return res;
        }
        
        if (NDist < N0){
            if (NDist < N1){
                // NDistが小さく、鋭い
                if (N0 > N1){
                    res |= dist;
                    N0 -= NDist;
                }else{
                    N1 -= NDist;
                }
                arg -= dist;
            }else{
                //鈍い
                int NRDist = N0 + N1 - NDist;
                if (NDist > NRDist){
                    //NDistが大きい
                    N0 -= NDist;
                    res |= dist;
                    arg -= dist;
                }else{
                    //NRDistが大きい
                    N0 -= NRDist;
                    res |= (arg - dist);
                    arg = dist;
                }
            }
        }else{
            if (NDist < N1){
                // 鈍い
                int NRDist = N0 + N1 - NDist;
                if (NDist > NRDist){
                    // NDistが大きい
                    N1 -= NDist;
                    arg -= dist;
                }else{
                    // NRDistが大きい
                    N1 -= NRDist;
                    arg = dist;
                }
            }else{
                // NDistが大きく、鋭い
                if (N0 > N1){
                    res |= (arg - dist);
                    N0 = NDist - N1;
                }else{
                    N1 = NDist - N0;
                }
                arg = dist;
            }
        }
        assert((int)countBits(arg) == N0 + N1);
        
        if (N0 == 1){
            res |= pick1Bit64(arg, 1 + N1, pdice);
            assert((int)countBits(res) == argN0);
            return res;
        }
        if (N1 == 1){
            res |= (arg - pick1Bit64(arg, N0 + 1, pdice));
            assert((int)countBits(res) == argN0);
            return res;
        }
    }
    UNREACHABLE;
}

template<class dice64_t>
static void dist2_64(uint64_t *goal0, uint64_t *goal1, uint64_t arg, int N0, int N1, dice64_t *const dice){
    assert((int)countBits64(arg) == N0 + N1);
    
    uint64_t tmp = pickNBits64(arg, N0, N1, dice);
    *goal0 |= tmp;
    *goal1 |= (arg - tmp);
}

/**************************n分割**************************/

template<class dice64_t>
static void dist3_64(uint64_t *goal0, uint64_t *goal1, uint64_t *goal2, uint64_t arg, int N0, int N1, int N2, dice64_t *const dice){
    uint64_t goal1_2 = 0ULL;
    dist2_64(&goal1_2, goal0, arg, N1 + N2, N0, dice);
    dist2_64(goal1, goal2, goal1_2, N1, N2, dice);
}

template<class dice64_t>
static void dist4_64(uint64_t *goal0, uint64_t *goal1, uint64_t *goal2, uint64_t *goal3, uint64_t arg, int N0, int N1, int N2, int N3, dice64_t *const dice){
    //std::cout<<arg;
    uint64_t goal0_1 = 0ULL, goal2_3 = 0ULL;
    dist2_64(&goal0_1, &goal2_3, arg, N0 + N1, N2 + N3, dice);
    dist2_64(goal0, goal1, goal0_1, N0, N1, dice);
    dist2_64(goal2, goal3, goal2_3, N2, N3, dice);
}

template<int N = 2, class size_t, class dice64_t>
static void dist64(uint64_t *const dst, const uint64_t arg, const size_t *const argNum, dice64_t *const dice){
    //ビット集合をN分割
    //N<=0には未対応
    //DOUT<<"pointer mode. "<<N<<endl;
    if (N > 0){
        switch (N){
            case 1:
                dst[0] |= arg;
                break;
            case 2:
                dist2_64(&dst[0], &dst[1], arg, argNum[0], argNum[1], dice);
                break;
            case 3:
                dist3_64(&dst[0], &dst[1], &dst[2], arg, argNum[0], argNum[1], argNum[2], dice);
                break;
            default:
                const int NH = N / 2;
                
                int num[2] = { 0 };
                int i = 0;
                for (; i < NH; ++i){
                    num[0] += argNum[i];
                }
                for (; i < N; ++i){
                    num[1] += argNum[i];
                }
                
                assert(num[0] + num[1] == (int)countBits(arg));
                
                uint64_t half[2] = { 0ULL };
                dist64<2>(half, arg, num, dice);
                dist64<NH>(dst, half[0], argNum, dice);
                dist64<N - NH>(dst + NH, half[1], splitHorizon(argNum, NH), dice);
                break;
        }
    }
}

template<int N = 2, int SN, int SSIZE, class dice64_t>
static void dist64(uint64_t *const dst, const uint64_t arg, const BitArray32<SN, SSIZE>& argNum, dice64_t *const dice){
    //ビット集合をN分割
    //N<=0には未対応
    //DOUT<<"reference mode. "<<N<<endl;
    if (N>0){
        switch (N){
            case 1:
                dst[0] |= arg;
                break;
            case 2:
                dist2_64(&dst[0], &dst[1], arg, argNum[0], argNum[1], dice);
                break;
            case 3:
                dist3_64(&dst[0], &dst[1], &dst[2], arg, argNum[0], argNum[1], argNum[2], dice);
                break;
            default:
                const int NH = N / 2;
                
                int num[2] = { 0 };
                int i = 0;
                for (; i < NH; ++i){
                    num[0] += argNum[i];
                }
                for (; i < N; ++i){
                    num[1] += argNum[i];
                }
                
                assert(num[0] + num[1] == (int)countBits(arg));
                
                uint64_t half[2] = { 0ULL };
                dist64<2>(half, arg, num, dice);
                dist64<NH>(dst, half[0], argNum, dice);
                dist64<N - NH>(dst + NH, half[1], splitHorizon(argNum, NH), dice);//BitArray32<SN,SSIZE>(argNum>>(NH*SN)));
                break;
        }
    }
}

#endif  // UTIL_BIT_PARTITION_HPP_
