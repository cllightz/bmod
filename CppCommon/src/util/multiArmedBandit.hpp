/*
 multiArmedBandit.hpp
 Katsuki Ohto
 */


#ifndef UTIL_MULTIARMED_BANDIT_HPP_
#define UTIL_MULTIARMED_BANDIT_HPP_

#include "../defines.h"

//多腕バンディット問題を扱う時の基本的な演算

double calc_ucb1(uint32_t mean, uint32_t size, uint32_t all_size, double K){
    //UCB1
    ASSERT(size > 0 && all_size > 0, cerr << size << " " << all_size << endl;);
    return mean + K * sqrt(log((double)all_size) / size);
}

double calc_ucb1(double mean, uint32_t size, uint32_t all_size, double K){
    //UCB1
    ASSERT(size > 0 && all_size > 0, cerr << size << " " << all_size << endl;);
    return mean + K * sqrt(log((double)all_size) / size);
}

double calc_ucb1(double mean, double size, double all_size, double K){
    //UCB1
    ASSERT(size > 0 && all_size > 0, cerr << size << " " << all_size << endl;);
    return mean + K * sqrt(log(all_size) / size);
}
/*
 float calc_ucb1(uint32_t eval_sum,uint32_t size,uint32_t all_size,float K){
 //UCB1
 ASSERT( size>0 && all_size>0, cerr<<size<<" "<<all_size<<endl; );
 return eval_sum/(float)size + K * sqrt(log((float)all_size)/size);
 }
 
 float calc_ucb1(float eval_sum,uint32_t size,uint32_t all_size,float K){
 //UCB1
 ASSERT( size>0 && all_size>0, cerr<<size<<" "<<all_size<<endl; );
 return eval_sum/(float)size + K * sqrt(log((float)all_size)/size);
 }
 
 float calc_ucb1(float eval_sum,float size,float all_size,float K){
 //UCB1
 ASSERT( size>0 && all_size>0, cerr<<size<<" "<<all_size<<endl; );
 return eval_sum/size + K * sqrt(log(all_size)/size);
 }
 */

//MCTSによって最適点を探すソルバー

template<int _DIMENSION=1, int _TYPES=1>
class MCTSSolver{
public:
    using gridIndex_t = long long int;
    using gridVariable_t = std::array<double, _DIMENSION>;
    using rewardZone_t = std::array<double, 2>;
    using gridVariableZone_t = std::array<std::array<double, 2>, _DIMENSION>;
    
private:
    constexpr static double EXTRACT_LINE_ = 8.0;
    constexpr static double K_UCB_ = 2.0;
    constexpr static double K_LCB_ = 0.03;
    constexpr static double ATTENUATE_RATE_ = 1.0 - 1.0 / 1024.0;
    
    constexpr static int dimension()noexcept{ return _DIMENSION; }
    
    std::random_device seed_gen_;
    std::mt19937 dice_;
    std::uniform_real_distribution<double> uni_;
    
    std::array<gridVariableZone_t, _TYPES> zone_;
    rewardZone_t reward_;
    
    std::array<double, 2>& varZone(int t, int d){ return zone_[t][d]; }
    const std::array<double, 2>& varZone(int t, int d)const{ return zone_[t][d]; }
    
    double varDist(int t, int d)const{ return std::get<1>(varZone(t, d)) - std::get<0>(varZone(t, d)); }
    double rewDist()const noexcept{ return std::get<1>(reward_) - std::get<0>(reward_); }
    
    // lower or higher bound
    template<typename T>static T& lb(std::array<T, 2>& azone)noexcept{ return std::get<0>(azone); }
    template<typename T>static T& lb(std::tuple<T, T>& azone)noexcept{ return std::get<0>(azone); }
    template<typename T>static T& hb(std::array<T, 2>& azone)noexcept{ return std::get<1>(azone); }
    template<typename T>static T& hb(std::tuple<T, T>& azone)noexcept{ return std::get<1>(azone); }
    
    double genRealRand()noexcept{
        return uni_(dice_);
    }
    
    struct MCTSSolverInfo{
        int childs;
        int plays;
        double sum;
        double size;
        MCTSSolverInfo() :
        childs(1), plays(0){}
    };
    
    MCTSSolverInfo info_;
    
    struct MCTSSolverChild{
        MCTSSolverChild *pNext_;
        double sum_;
        double sum2_;
        double size_;
        
        double size()const noexcept{ return size_; }
        double mean()const{ return sum_ / size_; }
        double var()const{ return sum2_ / size_ - mean()*mean(); }
        double lcb(int i)const{ return pNext_[i].mean() - K_LCB_ * sqrt(log(size()) / pNext_[i].size()); }
        
        int answerRecursively(gridVariable_t& var, MCTSSolverInfo *const pInfo){
            //return depth
            if (pNext_ != nullptr){
                int bestIdx = -1;
                double bestRew = -1.0;
                for (int i = 0; i < (1 << _DIMENSION); ++i){
                    if (pNext_[i].size() > 0){
                        double rew = lcb(i);
                        if (rew > bestRew){
                            bestIdx = i;
                            bestRew = rew;
                        }
                    }
                }
                if (bestIdx >= 0){
                    //cerr<<"best idx : "<<bestIdx<<endl;
                    int dep = pNext_[bestIdx].answerRecursively(var, pInfo);
                    for (int d = 0; d<_DIMENSION; ++d){
                        var[d] = (((bestIdx >> d) & 1) + var[d]) / 2;
                    }
                    return dep + 1;
                }
            }
            for (int d = 0; d<_DIMENSION; ++d){
                var[d] = 0.5;
            }
            return 0;
        }
        
        int answerZoneRecursively(gridVariableZone_t& zone, MCTSSolverInfo *const pInfo){
            //return depth
            if (pNext_ != nullptr){
                int bestIdx = -1;
                double bestRew = -1.0;
                for (int i = 0; i < (1 << _DIMENSION); ++i){
                    if (pNext_[i].size() > 0){
                        double rew = lcb(i);
                        if (rew > bestRew){
                            bestIdx = i;
                            bestRew = rew;
                        }
                    }
                }
                if (bestIdx >= 0){
                    //cerr<<"best idx : "<<bestIdx<<endl;
                    int dep = pNext_[bestIdx].answerZoneRecursively(var, pInfo);
                    for (int d = 0; d<_DIMENSION; ++d){
                        double base = (bestIdx >> d) & 1;
                        lb(zone[d]) = (base + lb(zone[d])) / 2;
                        hb(zone[d]) = (base + hb(zone[d])) / 2;
                    }
                    return dep + 1;
                }
            }
            for (int d = 0; d<_DIMENSION; ++d){
                lb(zone[d]) = 0;
                hb(zone[d]) = 1;
            }
            return 0;
        }
        
        void feedRecursively(gridVariable_t& var, double rew, double rew2, MCTSSolverInfo *const pInfo){
            //atenuating
            size_ *= ATTENUATE_RATE_;
            sum_ *= ATTENUATE_RATE_;
            sum2_ *= ATTENUATE_RATE_;
            size_ += 1;
            sum_ += rew;
            sum2_ += rew2;
            if (pNext_ == nullptr){
                if(size_ >= EXTRACT_LINE_){
                    pNext_ = new MCTSSolverChild[1 << _DIMENSION];
                    if (pNext_ != nullptr){
                        pInfo->childs += (1 << _DIMENSION);
                    }else{
                        return;
                    }
                }else{
                    return;
                }
            }
            assert(pNext_ != nullptr);
            int idx = 0;
            for (int d = 0; d<_DIMENSION; ++d){
                if (var[d] >= 0.5){
                    idx |= (1 << d);
                    var[d] -= 0.5;
                }
                var[d] *= 2;
            }
            pNext_[idx].feedRecursively(var, rew, rew2, pInfo);
        }
        
        template<class dice01_t>
        int play(gridVariable_t& var, const dice01_t& dice, MCTSSolverInfo *const pInfo){
            if (pNext_ != nullptr){
                //calc ucb1
                int bestIdx = -1;
                double bestUCB = -1.0;
                for (int i = 0; i<(1 << _DIMENSION); ++i){
                    double ucb = (pNext_[i].size()>4) ? calc_ucb1(pNext_[i].sum_, pNext_[i].size(), size(), K_UCB_) : (10000 + dice());
                    if (ucb>bestUCB){
                        bestIdx = i;
                        bestUCB = ucb;
                    }
                }
                int dep = pNext_[bestIdx].play(var, dice, pInfo);
                for (int d = 0; d<_DIMENSION; ++d){
                    var[d] = ((bestIdx >> d) & 1) * 0.5 + var[d] / 2;
                }
                return dep + 1;
            }
            else{
                for (int d = 0; d<_DIMENSION; ++d){
                    var[d] = dice();
                }
                return 0;
            }
        }
        
        std::string toString()const{
            std::ostringstream oss;
            oss << "mean: " << mean() << " var:" << var() << " size:" << size() << endl;
            return oss.str();
        }
        
        MCTSSolverChild():
        pNext_(nullptr),
        sum_(1), sum2_(2 / 12.0), size_(2){}
        
        ~MCTSSolverChild(){
            if (pNext_ != nullptr){
                delete[] pNext_;
                pNext_ = nullptr;
            }
        }
    };
    
    MCTSSolverChild child[_TYPES];
    MCTSSolverChild* last[64];
    
public:
    gridVariable_t play(){
        //return value to try
        gridVariable_t var;
        int dep = child[0].play(var, [this]()->double{ return genRealRand(); }, &info_);
        for (int d = 0; d<_DIMENSION; ++d){
            var[d] = lb(varZone(0, d)) + var[d] * varDist(0, d);
        }
        return var;
    }
    void feed(const gridVariable_t& var, double rew){
        double distRew = rewDist();
        if (distRew == 0.0){
            return;
        }
        else{
            double rew01 = min(1.0, max(0.0, (rew - std::get<0>(reward_)) / distRew));//0~1
            //CERR<<rew01<<endl;
            gridVariable_t var01;
            for (int d = 0; d<_DIMENSION; ++d){
                double distVar = varDist(0, d);
                if (distVar == 0.0){
                    var01[d] = 0.5;
                }
                else{
                    var01[d] = (var[d] - lb(varZone(0, d))) / distVar;
                }
            }
            child[0].feedRecursively(var01, rew01, rew01 * rew01, &info_);
            return;
        }
    }
    //void feed(double rew){
    //feed by last-play
    //}
    
    gridVariableZone_t answerZone(){
        gridVariable_t var;
        gridVariableZone_t ans;
        child[0].answerZoneRecursively(ans, &info_);
        for (int d = 0; d < _DIMENSION; ++d){
            lb(ans[d]) = lb(varZone(0, d)) + var[d] * varDist(0, d);
            hb(ans[d]) = lb(varZone(0, d)) + var[d] * varDist(0, d);
        }
        return ans;
    }
    
    gridVariable_t answer(){
        gridVariable_t ans;
        child[0].answerRecursively(ans, &info_);
        for (int d = 0; d < _DIMENSION; ++d){
            ans[d] = lb(varZone(0, d)) + ans[d] * varDist(0, d);
        }
        return ans;
    }
    
    std::string toInfoString()const{
        std::ostringstream oss;
        oss << "*** MCTS-solver information ***" << endl;
        oss << "childs : " << info_.childs << endl;
        oss << child[0].toString() << endl;
        /*for (int i = 0; i < (1 << _DIMENSION); ++i){
         if(child[0].pNext_ != nullptr){
         oss << child[0].pNext_[i].toString();
         }
         }*/
        return oss.str();
    }
    
    double mean()const{
        return std::get<0>(reward_) + child[0].mean() * rewDist();
    }
    
    template<class zone_t>
    void setVariableZone(const std::array<zone_t, _DIMENSION>& azone){
        for (int t = 0; t < _TYPES; ++t){
            for (int d = 0; d < _DIMENSION; ++d){
                lb(varZone(t, d)) = std::get<0>(azone[d]);
                hb(varZone(t, d)) = std::get<1>(azone[d]);
            }
        }
    }
    template<class zone_t>
    void setVariableZone(const std::array<std::array<zone_t, _DIMENSION>, _TYPES>& azone){
        for (int t = 0; t < _TYPES; ++t){
            for (int d = 0; d < _DIMENSION; ++d){
                lb(varZone(t, d)) = std::get<0>(azone[t][d]);
                hb(varZone(t, d)) = std::get<1>(azone[t][d]);
            }
        }
    }
    
    template<class zone_t>
    void setRewardZone(const zone_t& aRew){
        lb(reward_) = std::get<0>(aRew);
        hb(reward_) = std::get<1>(aRew);
    }
    
    MCTSSolver(const gridVariableZone_t& azone, const rewardZone_t& aRew) :
    dice_(seed_gen_()),
    uni_(0, 1),
    info_()
    {
        setVariableZone(azone);
        setRewardZone(aRew);
    }
    
    MCTSSolver(const std::array<gridVariableZone_t, _TYPES>& azone, const rewardZone_t& aRew) :
    dice_(seed_gen_()),
    uni_(0, 1),
    info_()
    {
        setVariableZone(azone);
        setRewardZone(aRew);
    }
    
};

#endif // UTIL_MULTIARMED_BANDIT_HPP_