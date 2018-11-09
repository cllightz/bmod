/*
 multiArmedBandit.hpp
 Katsuki Ohto
 */


#ifndef UTIL_MULTIARMED_BANDIT_HPP_
#define UTIL_MULTIARMED_BANDIT_HPP_

#include "../defines.h"

//多腕バンディット問題を扱う時の基本的な演算

double calc_ucb1(uint32_t eval_sum, uint32_t size, uint32_t all_size, double K){
    //UCB1
    ASSERT(size>0 && all_size>0, cerr << size << " " << all_size << endl;);
    return eval_sum / (double)size + K * sqrt(log((double)all_size) / size);
}

double calc_ucb1(double eval_sum, uint32_t size, uint32_t all_size, double K){
    //UCB1
    ASSERT(size>0 && all_size>0, cerr << size << " " << all_size << endl;);
    return eval_sum / (double)size + K * sqrt(log((double)all_size) / size);
}

double calc_ucb1(double eval_sum, double size, double all_size, double K){
    //UCB1
    ASSERT(size>0 && all_size>0, cerr << size << " " << all_size << endl;);
    return eval_sum / size + K * sqrt(log(all_size) / size);
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
    using gridIndex_t = unsigned long long;
    using gridVariable_t = std::array<double, _DIMENSION>;
    using gridPoint_t = std::array<gridIndex_t, _DIMENSION>;
    using rewardZone_t = std::array<double, 2>;
    using gridVariableZone_t = std::array<std::array<double, 2>, _DIMENSION>;
    
private:
    constexpr static double EXTRACT_LINE_ = 8.0;
    constexpr static double K_UCB_ = 4.0;
    constexpr static double K_LCB_ = 0.03;
    constexpr static double ATTENUATE_RATE_ = 1;
    
    constexpr static int dimension()noexcept{ return _DIMENSION; }
    
    std::random_device seed_gen_;
    std::mt19937 dice_;
    std::uniform_real_distribution<> uni_;
    
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
        
        int answerRecursively(gridPoint_t& point, MCTSSolverInfo *const pInfo){
            //return depth
            if (pNext_ == nullptr){
                return 0;
            }
            else{
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
                if (bestIdx<0){
                    return 0;
                }
                else{
                    //cerr<<"best idx : "<<bestIdx<<endl;
                    for (int d = 0; d<_DIMENSION; ++d){
                        point[d] <<= 1;
                        point[d] |= (bestIdx >> d) & 1;
                    }
                    int dep = pNext_[bestIdx].answerRecursively(point, pInfo);
                    return dep + 1;
                }
            }
        }
        
        void feedRecursively(gridVariable_t& var, double rew, double rew2, MCTSSolverInfo *const pInfo){
            //atenuating
            size_ *= ATTENUATE_RATE_;
            sum_ *= ATTENUATE_RATE_;
            sum2_ *= ATTENUATE_RATE_;
            size_ += 1;
            sum_ += rew;
            sum2_ += rew2;
            if (size_ >= EXTRACT_LINE_){
                int idx = 0;
                for (int d = 0; d<_DIMENSION; ++d){
                    if (var[d] >= 0.5){
                        idx |= (1 << d);
                        var[d] -= 0.5;
                        var[d] *= 2;
                    }
                    else{
                        var[d] *= 2;
                    }
                }
                if (pNext_ != nullptr){
                    pNext_[idx].feedRecursively(var, rew, rew2, pInfo);
                }
            }
        }
        
        template<class dice_t>
        int play(gridPoint_t& point, dice_t& dice, MCTSSolverInfo *const pInfo){
            if (size_ >= EXTRACT_LINE_){
                int idx = dice() & ((1 << _DIMENSION) - 1);
                if (pNext_ == nullptr){
                    pNext_ = new MCTSSolverChild[1 << _DIMENSION];
                    if (pNext_ != nullptr){
                        pInfo->childs += (1 << _DIMENSION);
                    }
                    else{
                        return 0;
                    }
                }
                assert(pNext_ != nullptr);
                //calc ucb1
                int bestIdx = -1;
                double bestUCB = -1.0;
                for (int i = 0; i<(1 << _DIMENSION); ++i){
                    double ucb = (pNext_[i].size()>4) ? calc_ucb1(pNext_[i].sum_, pNext_[i].size(), size(), K_UCB_) : (10000 + (dice() % 1024));
                    if (ucb>bestUCB){
                        bestIdx = i;
                        bestUCB = ucb;
                    }
                }
                for (int d = 0; d<_DIMENSION; ++d){
                    point[d] <<= 1;
                    point[d] |= (bestIdx >> d) & 1;
                }
                int dep = pNext_[idx].play(point, dice, pInfo);
                return dep + 1;
            }
            else{
                return 0;
            }
        }
        
        std::string toString()const{
            std::ostringstream oss;
            oss << "mean: " << mean() << " var:" << var() << " size:" << size() << endl;
            return oss.str();
        }
        
        MCTSSolverChild(){
            pNext_ = nullptr;
            sum_ = 1;
            sum2_ = 2 / 12.0;
            size_ = 2;
        }
        
        ~MCTSSolverChild(){
            if (pNext_ != nullptr){
                delete[] pNext_;
                pNext_ = nullptr;
            }
        }
    };
    
    MCTSSolverChild child[_TYPES];
    MCTSSolverChild* last[64];
    
    double ItoFNormal(int dep, unsigned long long index)const{
        return index / double(1ULL << dep);
    }
    
public:
    gridVariable_t play(){
        //return value to try
        gridPoint_t point;
        gridVariable_t sample;
        point.fill(0);
        int dep = child[0].play(point, dice_, &info_);
        for (int d = 0; d<_DIMENSION; ++d){
            //CERR<<val[d]<<endl;
            double distVar = max(0.0, varDist(0, d));
            sample[d] = lb(varZone(0, d)) + ItoFNormal(dep, point[d]) * distVar + ItoFNormal(dep, 1) * distVar * uni_(dice_);
        }
        return sample;
    }
    void feed(const gridVariable_t& var, double rew){
        double distRew = rewDist();
        if (distRew <= 0){
            return;
        }
        else{
            double rew01 = min(1.0, max(0.0, (rew - lb(reward_)) / distRew));//0~1
            //CERR<<rew01<<endl;
            gridVariable_t var01;
            for (int d = 0; d<_DIMENSION; ++d){
                double distVar = varDist(0, d);
                if (distVar <= 0){
                    var01[d] = 0;
                }
                else{
                    var01[d] = (var[d] - lb(varZone(0, d))) / distVar;
                }
            }
            child[0].feedRecursively(var01, rew01, rew01 * rew01, &info_);
            return;
        }
    }
    void feed(double rew){
        //feed by last-play
    }
    
    gridVariableZone_t answerZone(){
        gridVariableZone_t ans;
        gridPoint_t point;
        point.fill(0);
        int dep = child[0].answerRecursively(point, &info_);
        for (int d = 0; d<_DIMENSION; ++d){
            //CERR<<val[d]<<endl;
            double distVar = max(0.0, varDist(0, d));
            lb(ans[d]) = lb(varZone(0, d)) + ItoFNormal(dep, point[d]) * distVar;
            hb(ans[d]) = lb(varZone(0, d)) + ItoFNormal(dep, point[d] + 1) * distVar;
            //CERR<<distVal<<" "<<ans[d].first<<"-"<<ans[d].second<<endl;
        }
        return ans;
    }
    
    gridVariable_t answer(){
        gridVariable_t ans;
        gridVariableZone_t ans2 = answerZone();
        for (int d = 0; d<_DIMENSION; ++d){
            //CERR<<ans2[d].first<<"-"<<ans2[d].second<<endl;
            ans[d] = (lb(ans2[d]) + hb(ans2[d])) / 2.0;
        }
        return ans;
    }
    
    void info()const{
        cerr << "*** MCTS-solver information ***" << endl;
        cerr << "childs : " << info_.childs << endl;
        cerr << child[0].toString() << endl;
        for (int i = 0; i < (1 << _DIMENSION); ++i){
            cerr << child[0].pNext_[i].toString();
        }
    }
    
    double mean()const{
        double distRew = max(0.0, rewDist());
        return std::get<0>(reward_) + distRew * child[0].mean();
    }
    
    template<class zone_t>
    void setVariableZone(const std::array<zone_t, _DIMENSION>& azone){
        for (int t = 0; t < _TYPES; ++t){
            for (int d = 0; d < _DIMENSION; ++d){
                lb(varZone(t, d)) = min(std::get<0>(azone[d]), std::get<1>(azone[d]));
                hb(varZone(t, d)) = max(std::get<0>(azone[d]), std::get<1>(azone[d]));
            }
        }
    }
    template<class zone_t>
    void setVariableZone(const std::array<std::array<zone_t, _DIMENSION>, _TYPES>& azone){
        for (int t = 0; t < _TYPES; ++t){
            for (int d = 0; d < _DIMENSION; ++d){
                lb(varZone(t, d)) = min(std::get<0>(azone[t][d]), std::get<1>(azone[t][d]));
                hb(varZone(t, d)) = max(std::get<0>(azone[t][d]), std::get<1>(azone[t][d]));
            }
        }
    }
    
    template<class zone_t>
    void setRewardZone(const zone_t& aRew){
        lb(reward_) = min(std::get<0>(aRew), std::get<1>(aRew));
        hb(reward_) = max(std::get<0>(aRew), std::get<1>(aRew));
    }
    
    MCTSSolver(const gridVariableZone_t& azone, const rewardZone_t& aRew) :
    dice_(seed_gen()),
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