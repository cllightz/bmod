/*
regressor.hpp
Katsuki Ohto
*/

#ifndef UTIL_REGRESSOR_HPP_
#define UTIL_REGRESSOR_HPP_

#include <vector>
#include <utility>
#include <fstream>
#include <cmath>
#include <map>

#include "../defines.h"
#include "random.hpp"

template<class regressor_t>
class RegressorLearner{
private:
    constexpr static int N_PARAMS_ = regressor_t::N_PARAMS_; // パラメータの数
    constexpr static int N_PHASES_ = regressor_t::N_PHASES_; // ドメインの数
    constexpr static int N_STAGES_ = regressor_t::N_STAGES_; // 第1分岐の数
    
    constexpr static double PARAM_VALUE_MAX = 256;
    
    static void assert_index(int i)noexcept{ ASSERT(0 <= i && i < N_PARAMS_, cerr << i << endl;); }
    static void assert_phase(int i)noexcept{ ASSERT(0 <= i && i < N_PHASES_, cerr << i << endl;); }
    static void assert_stage(int i)noexcept{ ASSERT(0 <= i && i < N_STAGES_, cerr << i << endl;); }
    
    regressor_t *pregressor_;
    
public:
    constexpr static int params()noexcept{ return N_PARAMS_; }
    constexpr static int phases()noexcept{ return N_PHASES_; }
    constexpr static int stages()noexcept{ return N_STAGES_; }
    
    double temperature()const noexcept{ return T_; }
    
    // temporary variable
    double score_;
    std::vector<std::pair<int, double>> vec_;
    
    // temporary variables for learning
    // double param_sum_;
    // double param2_sum_;
    double gradient_[N_PARAMS_];
    int turns_;
    int tmpBatch_;
    
    // objective function
    int trials_[N_PHASES_];
    double MAESum_[N_PHASES_];
    double MSESum_[N_PHASES_];
    //double entropySum_[N_PHASES_];
    
    // about feature
    double feature_size_[N_PHASES_];
    double feature_sum_[N_PHASES_][N_PARAMS_];
    double feature_sum2_[N_PHASES_][N_PARAMS_];
    std::map<std::string, std::size_t> teacher_[N_PHASES_];
    
    // about record
    int records_[N_PHASES_];
    
    // L1, L2 standardation
    double baseParam_[N_PARAMS_];
    
    // learning param
    double E_;
    double L1_;
    double L2_;
    
    double T_;
    int batch_;
    
    RegressorLearner():
    pregressor_(nullptr){
        initBaseParam();
        initFeatureValue();
        initObjValue();
        initLearnParam();
        initLearning();
    }
    
    ~RegressorLearner(){
        pregressor_ = nullptr;
    }
    
    double param(int i)const{
        if(pregressor_ != nullptr){
            return pregressor_->param(i);
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
    
    void setPolicy(regressor_t *const appol)noexcept{
        pregressor_ = appol;
    }
    
    void resetPolicy()noexcept{
        pregressor_ = nullptr;
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
            feature_size_[ph] = 1;
            for(int i = 0; i < N_PARAMS_; ++i){
                feature_sum_[ph][i] = 0;
                feature_sum2_[ph][i] = feature_size_[ph];
            }
            records_[ph] = 0;
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
        for (int ph = 0; ph < N_PHASES_; ++ph){
            trials_[ph] = 0;
            MAESum_[ph] = 0;
            MSESum_[ph] = 0;
            entropySum_[ph] = 0;
        }
    }
    
    double calcMAE(int ph = 0)const noexcept{
        if (trials_[ph] > 0){
            return bestHitRateSum_[ph] / trials_[ph];
        }
        return 0;
    }
    double calcMSE(int ph = 0)const noexcept{
        if (trials_[ph] > 0){
            return MSESum_[ph] / trials_[ph];
        }
        return 0;
    }
    /*double calcEntropy(int ph = 0)const noexcept{
        if (trials_[ph] > 0){
            return entropySum_[ph] / trials_[ph];
        }
        return 0;
    }*/
    
    void initLearning()noexcept{
        // initialize
        turns_ = 0;
        for(int i = 0; i < N_PARAMS_; ++i){
            gradient_[i] = 0;
        }
        tmpBatch_ = 0;
    }
    
    void initCalculatingScore(){
        vec_.clear();
        score_ = 0;
    }
    void feedFeatureScore(int f, double v){
        //cerr << "i = " << f << " v = " << v << endl;
        vec_.emplace_back(std::pair<int, double>(f, v));
    }
    void feedScore(double s){
        score_ = s;
    }
    
    void learn(){}
    
    void learnByMove(){
        
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
        
        if(e == 0.0 || pregressor_ == nullptr){ return; }
        
        double *const param = pregressor_->param_;
            
        for(int i = 0; i < N_PARAMS_; ++i){
            double omg = e / T * gradient_[i];
            param[i] += omg; // 最急降下法によるパラメータ更新
            double tmp = param[i];
            
            FASSERT(tmp,); FASSERT(baseParam(i),);
            
            // L1
            const double l1 = tmp > baseParam(i) ? (-1) : (1);
            // L2
            const double l2 = -2 * (tmp - baseParam(i));
            
            // 正則化項によるパラメータ更新は、baseを追い越すときはbaseにする
            double nrm = l1 * lam1 + l2 * lam2;
            if((tmp - baseParam(i)) * (tmp + nrm - baseParam(i)) <= 0){
                param[i] = baseParam(i);
            }else{
                param[i] += nrm;
            }
            
            // 絶対値が大きい場合は丸める
            if(param[i] > +PARAM_VALUE_MAX){
                param[i] = PARAM_VALUE_MAX;
            }else if(param[i] < -PARAM_VALUE_MAX){
                param[i] = -PARAM_VALUE_MAX;
            }
            FASSERT(param[i],);
        }
        initLearning();
        
        ASSERT(pregressor_->exam(),);
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
    }
    
    void feedTeacherName(const std::string& name, int ph = 0){
        teacher_[name]++;
    }
    
    void feedAnswer(double ans, int ph = 0){
        for (const auto& element : vec_){
            double dg = element.second / var(ph, element.first);
            
            FASSERT(dg, cerr << "elm = " << element.first << " " << element.second
                   << " / " << var(ph, element.first) << endl;);

            gradient_[element.first] += dg;
        }
        ++turns_;
    }
    
    void feedObjValue(double ans, int ph = 0){
        // feed MAE, MSE
        MAESum_[ph] += fabs(ans - score_);
        MSESum_[ph] += pow(ans - score_, 2);
        ++trials_[ph];
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
            cerr << "SoftmaxPolicyLearner::finFeatureSurvey() : failed to import!" << endl;
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
    
    std::string toObjValueString(int ph = 0)const{
        std::ostringstream oss;
        oss << "MAE = "<< calcMAE(ph);
        oss << ", MSE = "<< calcMSE(ph);
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
class Regressor{
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
    RegressorLearner<Regressor<_N_PARAMS_, _N_PHASES_>> *plearner_;
    
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
    void initCalculatingScore(int candidates)const{
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
    void feedScore(double s)const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            FASSERT(s,);
            plearner_->feedScore(s);
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
    void feedAnswer(double ans, int ph = 0)const{
        if(M){
            ASSERT(plearner_ != nullptr,);
            plearner_->acceptDecidedMoveIndex(ans, ph);
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
            cerr << "Regressor::fin() : failed to import!" << endl;
            return -1;
        }
        for(int i = 0; ifs && i < N_PARAMS_; ++i){
            ifs >> param_[i];
        }
        return 0;
    }
    
    int fout(const std::string& fName)const{
        std::ofstream ofs(fName, std::ios::out);
        if(!ofs){ return -1; }
        for(int i = 0; i < N_PARAMS_ - 1; ++i){
            ofs << param_[i] << " ";
        }
        ofs << param_[N_PARAMS_ - 1];
        return 0;
    }

    void setLearner(RegressorLearner<Regressor<_N_PARAMS_, _N_PHASES_>> *const aplearner)noexcept{
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
        Norm<double> norm(mean, sigma);
        for (int i = 0; i < N_PARAMS_; ++i){
            param_[i] = norm.rand(pdice);
        }
    }
    
    void setTemperature(double at)noexcept{
        T_ = at;
    }
    
    Regressor(const double* ap):
    T_(1.0),
    plearner_(nullptr)
    {
        for (int i = 0; i < N_PARAMS_; ++i){
            param_[i] = ap[i];
        }
    }
    
    Regressor():
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


#endif // UTIL_REGRESSOR_HPP_