/*
 selection.hpp
 Katsuki Ohto
 */

#ifndef UTIL_SELECTION_HPP_
#define UTIL_SELECTION_HPP_

#include <cfloat>
#include <vector>
#include <cmath>

// 何らかの候補ごとの評価関数が与えられた場合に
// そこから選択するための方法を実装

template<class dice_t>
int selectBySoftmax(double *const score, const int moves, const double temp,
                    dice_t *const pdice){
    // 単純softmaxによって選択
    
    // 確率に変換、最大値を検索
    double sum = 0;
    for(int i = 0; i < moves; ++i){
        double es = exp(score[i] / temp);
        score[i] = es;
        sum += es;
    }
    // 確率的に決定
    double newSum = 0;
    for(int i = 0; i < moves; ++i){
        newSum += score[i];
    }
    double r = pdice->drand() * newSum;
    int i = 0;
    for(; i < moves - 1; ++i){
        r -= score[i];
        if(r <= 0){ break; }
    }
    return i;
}

template<class dice_t>
int selectByThresholdSoftmax(double *const score, const int moves,
                             const double temp, const double threshold,
                             dice_t *const pdice){
    // softmax関数による確率に閾値を設ける
    // 確率に変換、最大値を検索
    double sum = 0;
    for(int i = 0; i < moves; ++i){
        double es = exp(score[i] / temp);
        score[i] = es;
        sum += es;
    }
    for(int i = 0; i < moves; ++i){
        score[i] /= sum;
    }
    // 確率を閾値によって調整
    for(int i = 0; i < moves; ++i){
        score[i] = max(score[i] - threshold, 0.0);
    }
    
    // 確率的に決定
    double newSum = 0;
    for(int i = 0; i < moves; ++i){
        newSum += score[i];
    }
    double r = pdice->drand() * newSum;
    int i = 0;
    for(; i < moves - 1; ++i){
        r -= score[i];
        if(r <= 0){ break; }
    }
    return i;
}

template<class dice_t>
int selectByProbBiasedSoftmax(double *const score, const int moves, const double temp,
                              dice_t *const pdice){
    // softmaxより、評価が低い選択肢が選ばれる確率を低く調整したもの
    // 確率に変換、最大値を検索
    double sum = 0;
    for(int i = 0; i < moves; ++i){
        double es = exp(score[i] / temp);
        score[i] = es;
        sum += es;
    }
    double maxProb = 0;
    for(int i = 0; i < moves; ++i){
        score[i] /= sum;
        maxProb = max(maxProb, score[i]);
    }
    // 確率を新たに調整
    // トップからの差によって調整
    for(int i = 0; i < moves; ++i){
        double diffProb = maxProb - score[i];
        score[i] *= exp(diffProb);
    }
    // 確率的に決定
    double newSum = 0;
    for(int i = 0; i < moves; ++i){
        newSum += score[i];
    }
    double r = pdice->drand() * newSum;
    int i = 0;
    for(; i < moves - 1; ++i){
        r -= score[i];
        if(r <= 0){ break; }
    }
    return i;
}

template<class dice_t>
int selectByBiasedSoftmax(double *const score, const int moves,
                          const double temp, const double coef, const double rate,
                          dice_t *const pdice){
    // softmaxより、評価が低い選択肢が選ばれる確率を低く調整したもの
    // 最大値を検索
    double maxScore = -DBL_MAX;
    for(int i = 0; i < moves; ++i){
        maxScore = max(maxScore, score[i]);
    }
    // トップからの差によって調整
    for(int i = 0; i < moves; ++i){
        score[i] = score[i] - coef * pow(maxScore - score[i], rate);
    }
    double sum = 0;
    for(int i = 0; i < moves; ++i){
        double es = exp(score[i] / temp);
        score[i] = es;
        sum += es;
    }
    // 確率的に決定
    double r = pdice->drand() * sum;
    int i = 0;
    for(; i < moves - 1; ++i){
        r -= score[i];
        if(r <= 0){ break; }
    }
    return i;
}

struct MaxSelector{
    // スコア最大を選択
    double *const score;
    const int moves;
    
    MaxSelector(double *const ascore, const int amoves):
    score(ascore), moves(amoves){}
    
    int select()const{ // 同点の場合は先にあるもの
        int bestIndex = 0;
        double bestScore = -DBL_MAX;
        for(int i = 0; i < moves; ++i){
            if(score[i] > bestScore){
                bestScore = score[i];
                bestIndex = i;
            }
        }
        return bestIndex;
    }
    
    template<class dice_t>
    int select(dice_t *const pdice)const{ // 同点の場合はランダム
        int bestIndex = 0;
        int bestCount = 0;
        double bestScore = -DBL_MAX;
        for(int i = 0; i < moves; ++i){
            if(score[i] > bestScore){
                bestScore = score[i];
                bestIndex = i;
                bestCount = 1;
            }else if(score[i] == bestScore){
                bestCount += 1;
                if(pdice->rand() % bestCount == 0){
                    bestIndex = i;
                }
            }
        }
        return bestIndex;
    }
    int run_all(){
        return select();
    }
    template<class dice_t>
    int run_all(dice_t *const pdice){
        return select(pdice);
    }
    double prob(size_t index)const{
        int bestCount = 0;
        double cmpScore = score[index];
        for(int i = 0; i < moves; ++i){
            if(score[i] > cmpScore){
                return 0;
            }else if(score[i] == cmpScore){
                bestCount += 1;
            }
        }
        return 1.0 / bestCount;
    }
    double entropy()const{
        int bestCount = 0;
        double bestScore = -DBL_MAX;
        for(int i = 0; i < moves; ++i){
            if(score[i] > bestScore){
                bestScore = score[i];
                bestCount = 1;
            }else if(score[i] == bestScore){
                bestCount += 1;
            }
        }
        return log((double)bestCount) / log((double)2);
    }
};

struct SoftmaxSelector{
    // 単純softmaxによって選択
    
    double *const score;
    const int moves;
    const double temp;
    
    double sum;
    
    SoftmaxSelector(double *const ascore, const int amoves,
                    const double atemp):
    score(ascore), moves(amoves), temp(atemp){}
    
    SoftmaxSelector(double *const ascore, const int amoves,
                    const std::vector<double>& params):
    score(ascore), moves(amoves), temp(params[0]){}
    
    void to_prob(){
        // 和を求める
        sum = 0;
        for(int i = 0; i < moves; ++i){
            double es = exp(score[i] / temp);
            score[i] = es;
            sum += es;
        }
    }
    
    double prob(size_t i)const{
        return score[i] / sum;
    }
    
    int select(double urand)const{
        double r = urand * sum;
        int i = 0;
        for(; i < moves - 1; ++i){
            r -= score[i];
            if(r <= 0){ break; }
        }
        return i;
    }
    
    template<class dice_t>
    int select(dice_t *const pdice)const{
        return select(pdice->drand());
    }
    
    int run_all(double urand){
        to_prob();
        return select(urand);
    }
    
    template<class dice_t>
    int run_all(dice_t *const pdice){
        return run_all(pdice->drand());
    }
    
    double* calc_prob(){
        to_prob();
        to_real_prob();
        return score;
    }
    
    void to_real_prob(){
        for(size_t i = 0; i < moves; ++i){
            score[i] /= sum;
        }
    }
    
    double entropy()const{
        double ent = 0;
        for(size_t i = 0; i < moves; ++i){
            double prb = prob(i);
            if(prb > 0){
                ent -= prb * log(prb);
            }
        }
        return ent / log((double)2);
    }
};

struct ThresholdSoftmaxSelector{
    // softmax関数による確率に閾値を設ける
    
    double *const score;
    const int moves;
    const double temp;
    const double threshold;
    
    double newSum;
    
    ThresholdSoftmaxSelector(double *const ascore, const int amoves,
                             const double atemp, const double athreshold):
    score(ascore), moves(amoves), temp(atemp), threshold(athreshold){}
    
    void cut(){
        // 確率に変換、最大値を検索
        double sum = 0;
        for(int i = 0; i < moves; ++i){
            double es = exp(score[i] / temp);
            score[i] = es;
            sum += es;
        }
        for(int i = 0; i < moves; ++i){
            score[i] /= sum;
        }
        // 確率を閾値によって調整
        for(int i = 0; i < moves; ++i){
            score[i] = max(score[i] - threshold, 0.0);
        }
    }
    
    void to_prob(){
        // 新しい和を求める
        newSum = 0;
        for(int i = 0; i < moves; ++i){
            newSum += score[i];
        }
    }
    
    double prob(size_t i)const{
        return score[i] / newSum;
    }
    
    int select(double urand)const{
        double r = urand * newSum;
        int i = 0;
        for(; i < moves - 1; ++i){
            r -= score[i];
            if(r <= 0){ break; }
        }
        return i;
    }
    
    template<class dice_t>
    int select(dice_t *const pdice)const{
        return select(pdice->drand());
    }
    
    int run_all(double urand){
        cut();
        to_prob();
        return select(urand);
    }
    
    template<class dice_t>
    int run_all(dice_t *const pdice){
        return select(pdice->drand());
    }
    
    double entropy()const{
        double ent = 0;
        for(size_t i = 0; i < moves; ++i){
            double prb = prob(i);
            if(prb > 0){
                ent -= prb * log(prb);
            }
        }
        return ent / log((double)2);
    }
};

struct BiasedSoftmaxSelector{
    
    double *const score;
    const int moves;
    const double temp;
    const double coef;
    const double rate;
    
    double sum;
    
    BiasedSoftmaxSelector(double *const ascore, const int amoves,
                          const double atemp, const double acoef, const double arate):
    score(ascore), moves(amoves), temp(atemp), coef(acoef), rate(arate){}
    
    void amplify(){
        double maxScore = -DBL_MAX;
        for(int i = 0; i < moves; ++i){
            maxScore = max(maxScore, score[i]);
        }
        // トップからの差によって調整
        for(int i = 0; i < moves; ++i){
            score[i] = score[i] - coef * pow(maxScore - score[i], rate);
        }
    }
    
    void to_prob(){
        sum = 0;
        for(int i = 0; i < moves; ++i){
            double es = exp(score[i] / temp);
            score[i] = es;
            sum += es;
        }
    }
    
    double prob(size_t i)const{
        return score[i] / sum;
    }
    
    int select(double urand)const{
        double r = urand * sum;
        int i = 0;
        for(; i < moves - 1; ++i){
            r -= score[i];
            if(r <= 0){ break; }
        }
        return i;
    }
    
    template<class dice_t>
    int select(dice_t *const pdice)const{
        return select(pdice->drand());
    }
    
    int run_all(double urand){
        amplify();
        to_prob();
        return select(urand);
    }
    
    template<class dice_t>
    int run_all(dice_t *const pdice){
        return run_all(pdice->drand());
    }
    
    double* calc_prob(){
        amplify();
        to_prob();
        to_real_prob();
        return score;
    }
    
    void to_real_prob(){
        for(size_t i = 0; i < moves; ++i){
            score[i] /= sum;
        }
    }
    
    double entropy()const{
        double ent = 0;
        for(size_t i = 0; i < moves; ++i){
            double prb = prob(i);
            if(prb > 0){
                ent -= prb * log(prb);
            }
        }
        return ent / log((double)2);
    }
};

struct ExpBiasedSoftmaxSelector{
    
    double *const score;
    const int moves;
    const double temp;
    const double coef;
    const double etemp;
    
    double sum;
    
    ExpBiasedSoftmaxSelector(double *const ascore, const int amoves,
                             const double atemp, const double acoef, const double aetemp):
    score(ascore), moves(amoves), temp(atemp), coef(acoef), etemp(aetemp){}
    
    void amplify(){
        double maxScore = -DBL_MAX;
        for(int i = 0; i < moves; ++i){
            maxScore = max(maxScore, score[i]);
        }
        // トップからの差によって調整
        for(int i = 0; i < moves; ++i){
            score[i] = score[i] - coef * exp((maxScore - score[i]) / etemp);
        }
    }
    
    void to_prob(){
        sum = 0;
        for(int i = 0; i < moves; ++i){
            double es = exp(score[i] / temp);
            score[i] = es;
            sum += es;
        }
    }
    
    double prob(size_t i)const{
        return score[i] / sum;
    }
    
    int select(double urand)const{
        double r = urand * sum;
        int i = 0;
        for(; i < moves - 1; ++i){
            r -= score[i];
            if(r <= 0){ break; }
        }
        return i;
    }
    
    template<class dice_t>
    int select(dice_t *const pdice)const{
        return select(pdice->drand());
    }
    
    int run_all(double urand){
        amplify();
        to_prob();
        return select(urand);
    }
    
    template<class dice_t>
    int run_all(dice_t *const pdice){
        return run_all(pdice->drand());
    }
    
    double* calc_prob(){
        amplify();
        to_prob();
        to_real_prob();
        return score;
    }
    
    void to_real_prob(){
        for(size_t i = 0; i < moves; ++i){
            score[i] /= sum;
        }
    }
    
    double entropy()const{
        double ent = 0;
        for(size_t i = 0; i < moves; ++i){
            double prb = prob(i);
            if(prb > 0){
                ent -= prb * log(prb);
            }
        }
        return ent / log((double)2);
    }
};

struct SparsemaxSelector{
    // sparsemax によって選択
    
    double *const score;
    std::vector<int> indice;
    const int moves;
    
    int k;
    double sum;
    double tau;
    
    SparsemaxSelector(double *const ascore, const int amoves):
    score(ascore), moves(amoves){
        indice.reserve(moves);
        for(auto i = 0; i < moves; ++i){
            indice.push_back(i);
        }
    }
    
    void sort(){
        // 候補を得点順に並べ替え
        std::sort(indice.begin(), indice.end(), [this](int a, int b)->bool{
            return this->score[a] > this->score[b];
        });
    }
    
    double calc_tau(){
        double sum_upper_i = 0;
        for(int i = 0; i < moves; ++i){
            sum_upper_i += score[indice[i]];
            if(1 + k * score[indice[i]] <= sum_upper_i){
                k = i - 1 + 1;
                tau = (sum_upper_i - 1) / k;
                return tau;
            }
        }
        return 0;
    }
    
    void to_prob(){
        // thresholdを引く
        sum = 0;
        for(int i = 0; i < moves; ++i){
            double ss = max(0.0, score[i] - tau);
            score[i] = ss;
            sum += ss;
        }
    }
    
    double prob(size_t i)const{
        return score[i] / sum;
    }
    
    int select(double urand)const{
        double r = urand * sum;
        int i = 0;
        for(; i < moves - 1; ++i){
            r -= score[i];
            if(r <= 0){ break; }
        }
        return i;
    }
    
    template<class dice_t>
    int select(dice_t *const pdice)const{
        return select(pdice->drand());
    }
    
    int run_all(double urand){
        sort();
        calc_tau();
        to_prob();
        return select(urand);
    }
    
    template<class dice_t>
    int run_all(dice_t *const pdice){
        return run_all(pdice->drand());
    }
    
    double* calc_prob(){
        sort();
        calc_tau();
        to_prob();
        to_real_prob();
        return score;
    }
    
    void to_real_prob(){
        for(size_t i = 0; i < moves; ++i){
            score[i] /= sum;
        }
    }
    
    double entropy()const{
        double ent = 0;
        for(size_t i = 0; i < moves; ++i){
            double prb = prob(i);
            if(prb > 0){
                ent -= prb * log(prb);
            }
        }
        return ent / log((double)2);
    }
};

#endif // UTIL_SELECTION_HPP_
