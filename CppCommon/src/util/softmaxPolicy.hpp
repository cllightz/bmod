/*
 softmaxPolicy.hpp
 Katsuki Ohto
 */

#ifndef UTIL_SOFTMAX_POLICY_HPP_
#define UTIL_SOFTMAX_POLICY_HPP_

#include <vector>
#include <utility>
#include <fstream>
#include <cmath>
#include <map>

#include "../defines.h"
#include "random.hpp"

struct MomentQuadruple{
    // one state
    std::vector<std::vector<std::pair<int, double>>> vec_;
    std::vector<double> score_;
    double score_sum_;
    int chosenIndex_; // which action was chosen?
};

template<class softmaxPolicy_t>
class SoftmaxPolicyLearner{
private:
    constexpr static int N_PARAMS_ = softmaxPolicy_t::N_PARAMS_; // パラメータの数
    constexpr static int N_PHASES_ = softmaxPolicy_t::N_PHASES_; // ドメインの数(複数のドメインに同じパラメータを使い回す場合)
    constexpr static int N_STAGES_ = softmaxPolicy_t::N_STAGES_; // 第1分岐の数(パラメータを分ける)
    
    constexpr static double PARAM_SUM_VALUE_MAX = 256; // 大きい値になりすぎるのを防ぐ
    constexpr static double VAR_ALPHA_MIN = 0.00001; // 分散が小さい要素に対応するパラメータの学習率が上がり過ぎないようにする
    
    static void assert_index(int i)noexcept{ ASSERT(0 <= i && i < N_PARAMS_, cerr << i << endl;); }
    static void assert_phase(int i)noexcept{ ASSERT(0 <= i && i < N_PHASES_, cerr << i << endl;); }
    static void assert_stage(int i)noexcept{ ASSERT(0 <= i && i < N_STAGES_, cerr << i << endl;); }
    
    softmaxPolicy_t *ppolicy_;
    
public:
    constexpr static int params()noexcept{ return N_PARAMS_; }
    constexpr static int phases()noexcept{ return N_PHASES_; }
    constexpr static int stages()noexcept{ return N_STAGES_; }
    
    double temperature()const noexcept{ return T_; }
    
    // temporary variable
    std::vector<double> score_;
    double score_sum_;
    std::vector<std::vector<std::pair<int, double>>> vec_;
    
    // temporary variables for learning
    // double param_sum_;
    // double param2_sum_;
    double gradient_[N_PARAMS_];
    int turns_;
    int tmpBatch_;
    
    // objective function
    int trials_[N_PHASES_];
    int unfoundTrials_[N_PHASES_];
    double meanHitRateSum_[N_PHASES_];
    double bestHitRateSum_[N_PHASES_];
    double KLDivergenceSum_[N_PHASES_];
    double entropySum_[N_PHASES_];
    
    // about feature
    double feature_size_[N_PHASES_];
    double feature_sum_[N_PHASES_][N_PARAMS_];
    double feature_sum2_[N_PHASES_][N_PARAMS_];
    std::map<std::string, std::size_t> teacher_[N_PHASES_];
    //double limitParam_[N_PARAMS_];
    
    // about record
    int records_[N_PHASES_];
    int unfoundRecords_[N_PHASES_];
    double branchSum_[N_PHASES_];
    double invBranchSum_[N_PHASES_];
    
    // L1, L2 standardation
    double baseParam_[N_PARAMS_];
    
    // learning param
    double E_;
    double L1_;
    double L2_;
    
    double T_;
    int batch_;
    
    // value stock for after learning
    std::vector<MomentQuadruple> stock_;
    
    struct ImaginaryTragectory{
        std::vector<MomentQuadruple> trg;
        double reward;
    };
    
    // imaginary value stock for after learning (1 turn)
    std::vector<ImaginaryTragectory> imaginaryTragectories_;
    
    struct ImaginaryTragectories{
        std::vector<ImaginaryTragectory> tragectory_;
        int realChosenIndex_;
    };
    
    // stock of imaginary tragectories
    std::vector<ImaginaryTragectories> imaginaryStock_;
    
    SoftmaxPolicyLearner():
    ppolicy_(nullptr){
        initBaseParam();
        initFeatureValue();
        initObjValue();
        initLearnParam();
        initLearning();
    }
    
    ~SoftmaxPolicyLearner(){
        ppolicy_ = nullptr;
    }
    
    double param(int i)const{
        if(ppolicy_ != nullptr){
            return ppolicy_->param(i);
        }else{
            return 0.0;
        }
    }
    double baseParam(int i)const{
        assert_index(i);
        return baseParam_[i];
    }
    double mean(int ph, int i)const{
        assert_phase(ph); assert_index(i); assert(feature_size_[ph] > 0);
        return feature_sum_[ph][i] / feature_size_[ph];
    }
    double var(int ph, int i)const{
        assert_phase(ph); assert_index(i); assert(feature_size_[ph] > 0);
        double me = mean(ph, i);
        return feature_sum2_[ph][i] / feature_size_[ph] - me * me;
    }
    double all_phase_size()const{
        double sum_size = 0;
        for(int ph = 0; ph < N_PHASES_; ++ph){
            sum_size += feature_size_[ph];
        }
        return sum_size;
    }
    double all_phase_mean(int i)const{
        assert_index(i);
        double sum_sum = 0;
        for(int ph = 0; ph < N_PHASES_; ++ph){
            sum_sum += feature_sum_[ph][i];
        }
        double sum_size = all_phase_size();
        assert(sum_size > 0);
        return sum_sum / sum_size;
    }
    double all_phase_var(int i)const{
        assert_index(i);
        double sum_sum2 = 0;
        for(int ph = 0; ph < N_PHASES_; ++ph){
            sum_sum2 += feature_sum2_[ph][i];
        }
        double sum_size = all_phase_size();
        assert(sum_size > 0);
        double me = all_phase_mean(i);
        return sum_sum2 / sum_size - me * me;
    }
    double limit(int i)const{
        assert_index(i);
        // パラメータ数が多いとき
        // 特徴量の値の分散が大きいとき
        // にはパラメータの値を小さめに制限
        return PARAM_SUM_VALUE_MAX / sqrt(sqrt(N_PARAMS_)) / sqrt(max(all_phase_var(i), 1.0));
    }
    
    void setPolicy(softmaxPolicy_t *const appol)noexcept{
        ppolicy_ = appol;
    }
    
    void resetPolicy()noexcept{
        ppolicy_ = nullptr;
    }

    void initLearnParam()noexcept{
        T_ = 1.0;
        E_ = 0.0001;
        L1_ = 0.0;
        L2_ = 0.0;
        batch_ = 1;
    }

    void setLearnParam(double at, double ae, double al1, double al2, int ab)noexcept{
        T_ = at;
        E_ = ae;
        L1_ = al1;
        L2_ = al2;
        batch_ = ab;
    }
    
    void initFeatureValue()noexcept{
        for(int ph = 0; ph < N_PHASES_; ++ph){
            teacher_[ph].clear();
            //feature_size_[ph] = 0.00000000000000001;
            feature_size_[ph] = 4;
            for(int i = 0; i < N_PARAMS_; ++i){
                feature_sum_[ph][i] = 0;
                feature_sum2_[ph][i] = feature_size_[ph];
            }
            records_[ph] = 0;
            unfoundRecords_[ph] = 0;
            branchSum_[ph] = 0;
            invBranchSum_[ph] = 0;
        }
    }
    
    void closeFeatureValue()noexcept{/*
        for(int ph = 0; ph < N_PHASES_; ++ph){
            if(trials_[ph] > 0){
                for(int i = 0; i < N_PARAMS_; ++i){
                    mean_[ph][i] = mean_[ph][i] / trials_[ph];
                    var_[ph][i] = var_[ph][i] / trials_[ph] - mean_[ph][i] * mean_[ph][i];
                }
            }
        }*/
    }
    
    void initBaseParam()noexcept{
        for(int i = 0; i < N_PARAMS_; ++i){
            baseParam_[i] = 0;
        }
    }
    void setBaseParam(const double *const ap){
        for(int i = 0; i < N_PARAMS_; ++i){
            baseParam_[i] = ap[i];
        }
    }
    
    void initObjValue()noexcept{
        for(int ph = 0; ph < N_PHASES_; ++ph){
            trials_[ph] = 0;
            unfoundTrials_[ph] = 0;
            meanHitRateSum_[ph] = 0;
            bestHitRateSum_[ph] = 0;
            KLDivergenceSum_[ph] = 0;
            entropySum_[ph] = 0;
        }
    }
    //void closeObjValue()noexcept{}
    
    double calcMeanHitRate(int ph = 0)const noexcept{
        if((trials_[ph] + unfoundTrials_[ph]) > 0){
            return meanHitRateSum_[ph] / (trials_[ph] + unfoundTrials_[ph]);
        }
        return 0;
    }
    double calcBestHitRate(int ph = 0)const noexcept{
        if((trials_[ph] + unfoundTrials_[ph]) > 0){
            return bestHitRateSum_[ph] / (trials_[ph] + unfoundTrials_[ph]);
        }
        return 0;
    }
    double calcKLDivergence(int ph = 0)const noexcept{
        if(trials_[ph] > 0){
            return KLDivergenceSum_[ph] / trials_[ph];
        }
        return 0;
    }
    double calcEntropy(int ph = 0)const noexcept{
        if(trials_[ph] > 0){
            return entropySum_[ph] / trials_[ph];
        }
        return 0;
    }
    
    double calcMeanBranches(int ph = 0)const noexcept{
        if(records_[ph] > 0){
            return branchSum_[ph] / records_[ph];
        }
        return 0;
    }
    double calcMeanInvBranches(int ph = 0)const noexcept{
        if(records_[ph] > 0){
            return invBranchSum_[ph] / records_[ph];
        }
        return 0;
    }
    
    void initLearning()noexcept{
        // initialize
        turns_ = 0;
        for(int i = 0; i < N_PARAMS_; ++i){
            gradient_[i] = 0;
        }
        tmpBatch_ = 0;
    }
    
    void initCalculatingScore(int candidates = -1){
        vec_.clear();
        score_.clear();
        if(candidates >= 0){
            vec_.reserve(candidates);
            score_.reserve(candidates);
        }
        score_sum_ = 0;
    }
    void initCalculatingCandidateScore(){
        vec_.emplace_back(std::vector<std::pair<int, double>>());
    }
    void feedFeatureScore(int f, double v){
        //cerr << "i = " << f << " v = " << v << endl;
        vec_.back().emplace_back(std::pair<int, double>(f, v));
    }
    
    void feedCandidateScore(double s){
        score_.emplace_back(s);
        score_sum_ += s;
    }
    
    void finishCalculatingScore(bool stock = false){
        // if after-learning mode, value should be stocked
        if(stock){
            stock_.emplace_back(MomentQuadruple{
                vec_,
                score_,
                score_sum_,
                -1
            });
        }
    }
    
    void feedChosenActionIndexToLatestStock(int idx, int ph = 0){
        if(stock_.size() > 0){
            stock_.back().chosenIndex_ = idx;
        }
    }
    
    void clearStocks(){
        stock_.clear();
    }
    
    void clearImaginaryStocks(){
        imaginaryStock_.clear();
    }
    
    void feedReward(double reward, int ph = 0){ // reinforcement learning
        for(auto& q : stock_){
            if(q.vec_.size() <= 1){ return; }
            
            for (const auto& element : q.vec_[q.chosenIndex_]){ // chosen action
                double dg = element.second / (var(ph, element.first) + VAR_ALPHA_MIN) * reward;
                
                FASSERT(dg, cerr << "elm = " << element.first << " " << element.second
                        << " / " << var(ph, element.first) << endl;);
                
                gradient_[element.first] += dg;
            }
            
            for (int m = 0, n = q.vec_.size(); m < n; ++m){ // all actions
                double possibility = (q.score_sum_ > 0) ? (q.score_[m] / q.score_sum_) : (1 / (double)n);
                
                FASSERT(possibility, cerr << q.score_[m] << " / " << q.score_sum_ << endl;);
                for (const auto& element : q.vec_[m]){ // all features
                    double dg = -element.second / (var(ph, element.first) + VAR_ALPHA_MIN) * possibility * reward;
                    
                    FASSERT(dg, cerr << "elm = " << element.first << " grd = " << dg
                            << " val = " << element.second << " p = " << possibility << " var = " << var(ph, element.first) << endl;);
                    
                    gradient_[element.first] += dg;
                }
            }
        }
        clearStocks();
    }
    
    void feedImaginaryReward(double reward, int ph = 0){
        //imaginaryStock.back().emplace_back(make_tuple(stock_, reward));
    }
    
    /*void feedChosenActionIndexToLatestImagnaryStock(int idx, int ph = 0){
        if(stock_.size() > 0){
            stock_.back().chosenIndex_ = idx;
        }
    }*/
    
    void feedRealChosenIndex(int idx){
        imaginaryStock_.emplace_back(ImaginaryTragectories{
            std::move(imaginaryTragectories_),
            idx,
        });
    }
    
    void feedRealReward(double reward, int ph = 0){
        /*for(auto& ts : imaginaryStock_){
            for(auto& t : ts.tragectories){
                
            }
        }
        for(auto& q : stock_){
            if(q.vec_.size() <= 1){ return; }
            
            for (const auto& element : q.vec_[q.chosenIndex_]){ // chosen action
                double dg = element.second / var(ph, element.first) * reward;
                
                FASSERT(dg, cerr << "elm = " << element.first << " " << element.second
                        << " / " << var(ph, element.first) << endl;);
                
                gradient_[element.first] += dg;
            }
            
            for (int m = 0, n = q.vec_.size(); m < n; ++m){ // all actions
                double possibility = (q.score_sum_ > 0) ? (q.score_[m] / q.score_sum_) : (1 / (double)n);
                
                FASSERT(possibility, cerr << q.score_[m] << " / " << q.score_sum_ << endl;);
                for (const auto& element : q.vec_[m]){ // all features
                    double dg = -element.second / var(ph, element.first) * possibility * reward;
                    
                    FASSERT(dg, cerr << "elm = " << element.first << " grd = " << dg
                            << " val = " << element.second << " p = " << possibility << " var = " << var(ph, element.first) << endl;);
                    
                    gradient_[element.first] += dg;
                }
            }
        }*/
        clearImaginaryStocks();
    }
    
    void updateParams(){
        
        ++tmpBatch_;
        //cerr << "batch = " << tmpBatch_ << endl;
        if(tmpBatch_ >= batch_){
            tmpBatch_ = 0;
        }else{
            return;
        }
        
        const double T = temperature();
        const double e = E_;
        const double lam1 = L1_ * sqrt(batch_);
        const double lam2 = L2_ * sqrt(batch_);
        
        if(e == 0.0 || ppolicy_ == nullptr){ return; }
        
        double *const param = ppolicy_->param_;
            
        for(int i = 0; i < N_PARAMS_; ++i){
            double omg = e / T * gradient_[i];
            param[i] += omg; // 最急降下法によるパラメータ更新
            double tmp = param[i];
            
            FASSERT(tmp,); FASSERT(baseParam(i),);
            
            // L1
            const double l1 = tmp > baseParam(i) ? (-1) : (1);
            // L2
            const double l2 = -2 * (tmp - baseParam(i));
            
            // 正則化項によるパラメータ更新は、baseを追い越すときはbaseにする(FOBOS)
            double nrm = l1 * lam1 + l2 * lam2;
            if((tmp - baseParam(i)) * (tmp + nrm - baseParam(i)) <= 0){
                param[i] = baseParam(i);
            }else{
                param[i] += nrm;
            }
            
            //if(nrm > 100){ cerr << nrm << endl; }
            
            // 絶対値が大きい場合は丸める
            double paramLimit = limit(i);
            param[i] = max(-paramLimit, min(+paramLimit, param[i]));
            FASSERT(param[i],);
        }
        initLearning();
        
        ASSERT(ppolicy_->exam(),);
    }
    void feedUnfoundFeatureValue(int ph = 0){
        ++unfoundRecords_[ph];
    }
    void feedFeatureValue(int ph = 0){
        if(vec_.size() <= 1){ return; }
        
        //double sum[POL_NUM_ALL] = {0};
        //double sum2[POL_NUM_ALL] = {0};
        
        const double weight = 1.0 / vec_.size();
        
        for (int m = 0, n = vec_.size(); m < n; ++m){
            for (auto element : vec_[m]){
                feature_sum_[ph][element.first] += element.second * weight;
                feature_sum2_[ph][element.first] += element.second * element.second * weight;
            }
        }
        
        ++feature_size_[ph];
        ++records_[ph];
        branchSum_[ph] += vec_.size();
        invBranchSum_[ph] += 1.0 / vec_.size();
    }
    
    void feedTeacherName(const std::string& name, int ph = 0){
        teacher_[name]++;
    }
    
    void feedSupervisedActionIndex(int idx, int ph = 0){ // in supervised learning mode
        if(vec_.size() <= 1){ return; }
        for (const auto& element : vec_[idx]){ // correct answer
            double dg = element.second / (var(ph, element.first) + VAR_ALPHA_MIN);
            
            FASSERT(dg, cerr << "elm = " << element.first << " " << element.second
                   << " / " << var(ph, element.first) << endl;);
            
            gradient_[element.first] += dg;
        }
        for (int m = 0, n = vec_.size(); m < n; ++m){ // all answers
            double possibility = (score_sum_ > 0) ? (score_[m] / score_sum_) : (1 / (double)n);
            
            FASSERT(possibility, cerr << score_[m] << " / " << score_sum_ << endl;);
            
            for (const auto& element : vec_[m]){
                double dg = -element.second / (var(ph, element.first) + VAR_ALPHA_MIN) * possibility;
                
                FASSERT(dg, cerr << "elm = " << element.first << " grd = " << dg
                       << " val = " << element.second << " p = " << possibility << " var = " << var(ph, element.first) << endl;);
                
                gradient_[element.first] += dg;
            }
        }
        ++turns_;
    }
    
    void feedObjValue(int idx, int ph = 0){
        if(vec_.size() <= 1){ return; }
        if(idx >= 0){
            for(int m = 0, n = vec_.size(); m < n; ++m){
                double possibility = (score_sum_ > 0) ? (score_[m] / score_sum_) : (1 / (double)n);
                
                FASSERT(possibility, cerr << score_[m] << " / " << score_sum_ << endl;);
                
                if(possibility > 0){
                    entropySum_[ph] += - possibility * log(possibility) / log(2.0);
                }
            }
            {
                int bestIdx[vec_.size()];
                bestIdx[0] = 0;
                int numBestIndex = 1;
                double bestScore = score_[0];
                for (int m = 1, n = vec_.size(); m < n; ++m){
                    if (score_[m] > bestScore){
                        bestIdx[0] = m;
                        bestScore = score_[m];
                        numBestIndex = 1;
                    }else if(score_[m] == bestScore){
                        bestIdx[numBestIndex] = m;
                        ++numBestIndex;
                    }
                }
                bool best = false;
                for(int m = 0; m < numBestIndex; ++m){
                    if(idx == bestIdx[m]){
                        best = true;
                        break;
                    }
                }
                if (score_sum_ > 0){
                    meanHitRateSum_[ph] += score_[idx] / score_sum_;
                    bestHitRateSum_[ph] += best ? (1.0 / numBestIndex) : 0;
                    
                    const double dkl = -(log(score_[idx] / score_sum_) / log(2.0));
                    FASSERT(dkl, cerr << " dkl" << endl;);
                    
                    KLDivergenceSum_[ph] += dkl;
                }else{
                    meanHitRateSum_[ph] += 1.0 / vec_.size();
                    bestHitRateSum_[ph] += 1.0 / vec_.size();
                    KLDivergenceSum_[ph] += -(log(1.0 / vec_.size()) / log(2.0));
                    //cerr << " n- " << -(log(1.0 / vec_.size()) / log(2.0)) << endl;
                }
                //cerr << KLDivergenceSum_[ph] << endl;
            }
            ++trials_[ph];
        }else{
            ++unfoundTrials_[ph];
        }
    }
    
    double calcBaseDistance(int ph = 0)const{
        double dsum = 0;
        for(int i = 0; i < params(); ++i){
            const double d = (param(i) - baseParam(i)) * var(ph , i);
            dsum += fabs(d);
            FASSERT(dsum,);
        }
        return dsum;
    }
    
    double calcBaseDistance2(int ph = 0)const{
        double dsum = 0;
        for(int i = 0; i < params(); ++i){
            const double d = (param(i) - baseParam(i)) * var(ph, i);
            dsum += d * d;
            FASSERT(dsum,);
        }
        return dsum;
    }
    
    int finFeatureSurvey(const std::string& fName, double af_size = 10000){

        std::ifstream ifs(fName, std::ios::in);
        if(!ifs){
            cerr << "SoftmaxPolicyLearner::finFeatureSurvey() : failed to import! " << fName << endl;
            return -1;
        }
        
        for(int ph = 0; ph < N_PHASES_; ++ph){
            feature_size_[ph] = af_size;
            for(int i = 0; i < N_PARAMS_; ++i){
                feature_sum_[ph][i] = 0;
                feature_sum2_[ph][i] = feature_size_[ph];
            }
        }
        
        for(int ph = 0; ph < N_PHASES_; ++ph){
            for(int i = 0; ifs && i < N_PARAMS_; ++i){
                double tmean, tvar;
                ifs >> tmean >> tvar;
                
                feature_sum_[ph][i] = tmean * feature_size_[ph];
                if(tvar > 0){
                    feature_sum2_[ph][i] = (tvar + tmean * tmean) * feature_size_[ph];
                }
                
                ASSERT(var(ph, i) > 0,);
            }
        }
        return 0;
    }
    
    
    int foutFeatureSurvey(const std::string& fName)const{
        std::ofstream ofs(fName, std::ios::out);
        if(!ofs){ return -1; }
        for(int ph = 0; ph < N_PHASES_; ++ph){
            for(int i = 0; ofs && i < N_PARAMS_; ++i){
                ofs << mean(ph, i) << " " << var(ph, i) << endl;
            }
        }
        return 0;
    }
    
    std::string toOverviewString()const{
        std::ostringstream oss;
        oss << params() << " params, " << phases() << " phases, " << stages() << " stages";
        return oss.str();
    }
    
    std::string toFeatureString()const{
        std::ostringstream oss;
        for(int i = 0, n = vec_.size(); i < n; ++i){
            oss << "action " << i << " : ";
            for(int j = 0, l = vec_[i].size(); j < l; ++j){
                oss << vec_[i][j].first;
                if(vec_[i][j].second != 1.0){
                    oss << "(" << vec_[i][j].second << ")";
                }
                oss << " ";
            }
            oss << endl;
        }
        return oss.str();
    }
    
    std::string toRecordString(int ph = 0)const{
        std::ostringstream oss;
        oss << "MB = " << calcMeanBranches(ph);
        oss << ", MIB = " << calcMeanInvBranches(ph) << " in " << records_[ph];
        oss << " records. (and " << unfoundRecords_[ph] << " unfound)";
        return oss.str();
    }
    
    std::string toObjValueString(int ph = 0)const{
        std::ostringstream oss;
        oss << "MHR(BHR) = " << calcMeanHitRate(ph);
        oss << "(" << calcBestHitRate(ph) << ")";
        oss << ", KLD = "<< calcKLDivergence(ph);
        oss << ", ENT = "<< calcEntropy(ph);
        return oss.str();
    }
    
    std::string toTeacherString(int ph = 0)const{
        std::ostringstream oss;
        for(auto& t : teacher_){
            oss << t.first << " : " << t.second << endl;
        }
        return oss.str();
    }
};

template<int _N_PARAMS_, int _N_PHASES_, int _N_STAGES_ = 1>
class SoftmaxPolicy{
private:
    static void assert_index(int i)noexcept{
        ASSERT(0 <= i && i < N_PARAMS_, cerr << i << endl;);
    }
    
public:
    constexpr static int N_PARAMS_ = _N_PARAMS_;
    constexpr static int N_PHASES_ = _N_PHASES_;
    constexpr static int N_STAGES_ = _N_STAGES_; // 第1分岐の数
    
    double T_; // temperature
    
    double param_[N_PARAMS_];
    SoftmaxPolicyLearner<SoftmaxPolicy<_N_PARAMS_, _N_PHASES_>> *plearner_;
    
    constexpr static int params()noexcept{ return N_PARAMS_; }
    constexpr static int phases()noexcept{ return N_PHASES_; }
    constexpr static int stages()noexcept{ return N_STAGES_; }
    
    double temperature()const noexcept{ return T_; }
    
    double param(int i)const{
        assert_index(i);
        return param_[i];
    }
    
    template<int M = 0>
    void setLearningMode()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->setLearningMode();
        }
    }
    
    template<int M = 0>
    bool isLearning()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            return plearner_->isLearning();
        }
        return false;
    }
    
    template<int M = 0>
    void initFeedingFeatureValue()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->initFeedingFeatureValue();
        }
    }
    
    template<int M = 0>
    void initObjValue()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->initObjValue();
        }
    }
    template<int M = 0>
    void initLearning()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->initLearning();
        }
    }
    template<int M = 0>
    void initCalculatingScore(int candidates = -1)const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->initCalculatingScore(candidates);
        }
    }
    template<int M = 0>
    void initCalculatingCandidateScore()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->initCalculatingCandidateScore();
        }
    }
    template<int M = 0>
    void feedFeatureScore(int f, double v)const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            FASSERT(v,);
            plearner_->feedFeatureScore(f, v);
        }
    }
    template<int M = 0>
    void feedCandidateScore(double s)const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            FASSERT(s,);
            plearner_->feedCandidateScore(s);
        }
    }
    template<int M = 0>
    void finishCalculatingScore()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->finishCalculatingScore(bool(M > 1));
        }
    }
    
    void learn();
    
    template<int M = 0>
    void learnByMove()const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->learnByMove();
        }
    }
    
    template<int M = 0>
    void feedFeatureValue(int ph = 0)const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->feedFeatureValue(ph);
        }
    }
    
    template<int M = 0>
    void acceptDecidedMoveIndex(int idx, int ph = 0)const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->acceptDecidedMoveIndex(idx, ph);
        }
    }
    
    std::string toString(int start, int end)const{
        std::ostringstream oss;
        for(int i = start; i < end; ++i){
            oss << param_[i] << " ";
        }
        return oss.str();
    }
    
    int fin(const std::string& fName){
        memset(param_, 0, sizeof(param_));
        std::ifstream ifs(fName, std::ios::in);
        if(!ifs){
            cerr << "SoftmaxPolicy::fin() : failed to import! " << fName << endl;
            return -1;
        }
        for(int i = 0; ifs && i < N_PARAMS_; ++i){
            ifs >> param_[i];
        }
        return 0;
    }
    
    int bin(const std::string& fName){
        memset(param_, 0, sizeof(param_));
        FILE *const pf = fopen(fName.c_str(), "rb");
        if(pf == nullptr){
            cerr << "SoftmaxPolicy::bin() : failed to import! " << fName << endl;
            return -1;
        }
        fread(param_, sizeof(param_), 1, pf);
        fclose(pf);
        return 0;
    }
    
    int fout(const std::string& fName)const{ // 標準出入力型
        std::ofstream ofs(fName, std::ios::out);
        if(!ofs){ return -1; }
        for(int i = 0; i < N_PARAMS_ - 1; ++i){
            ofs << param_[i] << " ";
        }
        ofs << param_[N_PARAMS_ - 1];
        return 0;
    }
    
    int bout(const std::string& fName)const{ // バイナリ型
        FILE *const pf = fopen(fName.c_str(), "wb");
        if(pf == nullptr){ return -1; }
        fwrite(param_, sizeof(param_), 1, pf);
        fclose(pf);
        return 0;
    }

    int hout(const std::string& fName, const std::string& tableName)const{ // ヘッダファイル型
        std::ofstream ofs(fName, std::ios::out);
        if(!ofs){ return -1; }
        ofs << "double " << tableName << "[] = {" << endl;
        for(int i = 0; i < N_PARAMS_; ++i){
            ofs << param_[i] << "," << endl;
        }
        ofs << "};";
        return 0;
    }
    
    void setLearner(SoftmaxPolicyLearner<SoftmaxPolicy<_N_PARAMS_, _N_PHASES_>> *const aplearner)noexcept{
        ASSERT(aplearner != nullptr,);
        plearner_ = aplearner;
    }
    void resetLearner()noexcept{
        plearner_ = nullptr;
    }
    
    void setParam(const std::vector<double>& apv){
        for(int i = 0, n = apv.size(); i < n && i < N_PARAMS_; ++i){
            param_[i] = apv[i];
        }
    }
    void setParam(const double* ap){
        memmove(param_, ap, sizeof(param_));
    }
    template<class dice_t>
    void setRandomParam(double mean, double sigma, dice_t *const pdice){
        NormalDistribution<double> norm(mean, sigma);
        for (int i = 0; i < N_PARAMS_; ++i){
            param_[i] = norm.rand(pdice);
        }
    }
    
    void setTemperature(double at)noexcept{
        T_ = at;
    }
    
    SoftmaxPolicy(const double* ap):
    T_(1.0),
    plearner_(nullptr)
    {
        for (int i = 0; i < N_PARAMS_; ++i){
            param_[i] = ap[i];
        }
    }
    
    SoftmaxPolicy():
    T_(1.0),
    plearner_(nullptr)
    {
        for (int i = 0; i < N_PARAMS_; ++i){
            param_[i] = 0;
        }
    }
    
    bool exam()const noexcept{
        // nan, inf
        auto valid = [](double d)->bool{ return !std::isnan(d) && !std::isinf(d); };
        if(!valid(T_)){
            return false;
        }
        for (int i = 0; i < N_PARAMS_; ++i){
            if(!valid(param_[i])){
                return false;
            }
        }
        return true;
    }
    
    std::string toOverviewString()const{
        std::ostringstream oss;
        oss << params() << " params, " << phases() << " phases, " << stages() << " stages";
        return oss.str();
    }
};

template<class param_t>
double calcParamDistance(const param_t& p0, const param_t& p1){
    double dsum = 0;
    for(int i = 0; i < param_t::params(); ++i){
        const double d = fabs(p0.param(i) - p1.param(i));
        dsum += d;
    }
    return dsum;
}

template<class param_t>
double calcParamDistance2(const param_t& p0, const param_t& p1){
    double dsum = 0;
    for(int i = 0; i < param_t::params(); ++i){
        const double d = p0.param(i) - p1.param(i);
        dsum += d * d;
    }
    return dsum;
}


#endif // UTIL_SOFTMAX_POLICY_HPP_