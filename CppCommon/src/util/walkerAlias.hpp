/*
 walkerAlias.hpp
 Katsuki Ohto
 */

#ifndef UTIL_WALKERALIAS_HPP_
#define UTIL_WALKERALIAS_HPP_

#include <cfloat>
#include <vector>
#include <cmath>

// Walker's alias method による非復元抽出

// 候補数が有限の場合
template<int kMaxCnandidates>
struct BoundedWalkerAliasSelector{
    
    double *const prob;
    const int moves;
    std::array<double, kMaxCnandidates> threshold;
    std::array<int, kMaxCnandidates> indexUnderThreshold;
    std::array<int, kMaxCnandidates> indexOverThreshold;
    
    BoundedWalkerAliasSelector(double *const aprob, const int amoves):
    prob(aprob), moves(amoves){
        set();
    }
    
    void set(){
        // WA mathod 用の配列に入れる
        std::queue<int> small, large;
        
        for(int i = 0; i < moves; ++i){
            indexUnderThreshold[i] = i;
            if(prob[i] < 1){
                small.push(i);
            }else{
                large.push(i);
            }
        }
        while(small.size() > 0 && large.size() > 0){
            int l = small.front();
            int g = large.front();
            small.pop();
            large.pop();
            
            threshold[l] = prob[l];
            indexOverThreshold[l] = indexUnderThreshold[g];
            prob[g] += -1.0 + prob[l];
            if(prob[g] < 1){
                small.push(g);
            }else{
                large.push(g);
            }
        }
        while(large.size() > 0){
            int g = large.front();
            large.pop();
            threshold[g] = 1;
        }
        while(small.size() > 0){
            int l = small.front();
            small.pop();
            threshold[l] = 1;
        }
    }
    
    int select(double urand)const{
        double v = urand * moves;
        int k = (int)v;
        double u = 1 + k - v;
        if(u < thresholdInWA[k]){
            return indexUnderThreshold[k];
        }else{
            return indexOverThreshold[k];
        }
    }
    template<class dice_t>
    int select(dice_t *const pdice){
        return select(pdice->drand());
    }
};

#endif // UTIL_SELECTION_HPP_