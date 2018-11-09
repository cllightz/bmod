/*
grid.hpp
Katsuki Ohto
*/


#ifndef UTIL_GRID_HPP_
#define UTIL_GRID_HPP_

#include <cfloat>

#include "../defines.h"

//格子点を利用した演算

template<int _DIMENSION = 1>
class UniformGridSolver{
    //一様ソルバ
public:
    using gridIndex_t = long long int;
    using gridPoint_t = std::array<gridIndex_t, _DIMENSION>;
    using gridVariable_t = std::array<double, _DIMENSION>;
    using gridVariableZone_t = std::array<std::array<double, 2>, _DIMENSION>;
    
private:
    double *score_;
    double *integratedScore_;
    
    gridIndex_t size_;
    gridPoint_t length_; // size of layer
    gridVariable_t step_; // step of variable
    gridVariableZone_t zone_; // variable zone
    
    // lower or higher bound
    template<typename T>static T& lb(std::array<T, 2>& azone)noexcept{ return std::get<0>(azone); }
    template<typename T>static T& lb(std::tuple<T, T>& azone)noexcept{ return std::get<0>(azone); }
    template<typename T>static T& hb(std::array<T, 2>& azone)noexcept{ return std::get<1>(azone); }
    template<typename T>static T& hb(std::tuple<T, T>& azone)noexcept{ return std::get<1>(azone); }

    void getPoint(gridIndex_t i, gridPoint_t* const ppoint){
        for (int d = _DIMENSION - 1;; --d){
            (*ppoint)[d] = i % length_[d];
            if (d == 0){ break; }
            i /= length_[d];
        }
    }
    void getVariable(const gridPoint_t& point, gridVariable_t *const pvar){
        for (int d = 0; d<_DIMENSION; ++d){
            (*pvar)[d] = zone_[d][0] + step_[d] * point[d];
        }
    }
    void getVariable(gridIndex_t i, gridVariable_t *const pvar){
        for (int d = _DIMENSION - 1;; --d){
            (*pvar)[d] = zone_[d][0] + step_[d] * (i % length_[d]);
            if (d == 0){ break; }
            i /= length_[d];
        }
    }

public:
    constexpr static int dimension()noexcept{ return _DIMENSION; }
    constexpr static int ld()noexcept{ return _DIMENSION - 1; }
    
    gridIndex_t size()const noexcept{ return size_; }
    gridIndex_t length(int d)const{ return length_[d]; }
    double lb(int d)const{ return lb(zone_[d]); }
    double hb(int d)const{ return hb(zone_[d]); }
    
    template<typename callback_t>
    void iterateGridVariable(const callback_t& callback){
        gridPoint_t point;
        gridVariable_t variable;
        for (int d=0; d<dimension(); ++d){
            point[d] = 0;
            variable[d] = lb(zone_[d]);
        }
        for (;;){
            callback(variable);
            for(int d = dimension()-1;; --d){
                ++point[d];
                variable[d] += step_[d];
                if(point[d] < length_[d]){
                    break;
                }else{
                    if(d == 0){ return; }
                    point[d] = 0;
                    variable[d] = lb(zone_[d]);
                }
            }
        }
    }
    template<typename callback_t>
    void iterateGridVariableWithPosition(const callback_t& callback){
        gridIndex_t i = 0;
        iterateGridVariable([callback, &i](const gridVariable_t& variable)->void{
            callback(variable, i);
            ++i;
        });
    }
    
    template<typename callback_t>
    void evalAllGrids(const callback_t& callback){
        iterateGridVariableWithPosition([this, callback](const gridVariable_t& variable, gridIndex_t i)->void{
            score_[i] = callback(variable);
        });
    }
    /*
    template<typename pdCallback_t>
    void integrate(const pdCallback_t& pdCallback){
        iterateGridVariableWithPosition([](const gridVariable_t& variable0, long long int i)->void{
            iterateGridValue([variable0, i](const gridVariable_t& variable1)->double{
                
            });
        });
    }
    */
    
    template<typename pdCallback_t>
    std::tuple<gridVariable_t, double> getBestIntegratedGridVariable(const pdCallback_t& pdCallback){
        gridIndex_t best = 0;
        double bestIntegratedScore = -DBL_MAX;
        iterateGridVariableWithPosition([this, pdCallback, &best, &bestIntegratedScore](const gridVariable_t& var0, gridIndex_t i0)->void{
            double scoreSum = 0;
            double pdfSum = 0;
            iterateGridVariableWithPosition([this, pdCallback, var0, &scoreSum, &pdfSum](const gridVariable_t& var1, gridIndex_t i1)->void{
                double pdf = pdCallback(var0, var1);
                //cerr << pdf << endl;
                scoreSum += score_[i1] * pdf;
                pdfSum += pdf;
            });
            if(pdfSum > 0){
                double integratedScore = scoreSum;// / pdfSum;
                //cerr << integratedScore << " ";
                if(integratedScore > bestIntegratedScore){
                    best = i0;
                    bestIntegratedScore = integratedScore;
                }
            }
        });
        gridVariable_t variable;
        getVariable(best, &variable);
        return make_tuple(variable, bestIntegratedScore);
    }

    template<class length_t>
    void setGridSize(const length_t& alength){
        size_ = 1;
        for (int d=0; d<dimension(); ++d){
            length_[d] = alength[d];
            size_ *= alength[d];
        }
        score_ = new double[size_];
    }
    
    template<class zone_t>
    void setVariableZone(const zone_t& azone){
        for (int d=0; d<dimension(); ++d){
            zone_[d][0] = std::get<0>(azone[d]);
            zone_[d][1] = std::get<1>(azone[d]);
            step_[d] = (zone_[d][1] - zone_[d][0]) / length_[d];
        }
        if(score_ == nullptr){
            score_ = new double[size_];
        }
    }
    
    UniformGridSolver():
    score_(nullptr), integratedScore_(nullptr){
        length_.fill(1);
        size_ = 1;
    }
    
    UniformGridSolver(const gridIndex_t& alendth, const gridVariableZone_t& azone):
    score_(nullptr), integratedScore_(nullptr){
        setGridSize(alendth);
        setVariableZone(azone);
    }

    ~UniformGridSolver(){
        if(score_ != nullptr){
            delete[] score_;
            score_ = nullptr;
        }
        
        if(integratedScore_ != nullptr){
            delete[] integratedScore_;
            integratedScore_ = nullptr;
        }
    }
};

template<int _DIMENSION = 1>
class UncertaintyUniformGridSolver{
    //一様不確定ソルバ
public:
    using gridIndex_t = long long int;
    using gridPoint_t = std::array<gridIndex_t, _DIMENSION>;
    using gridVariable_t = std::array<double, _DIMENSION>;
    using gridVariableZone_t = std::array<std::array<double, 2>, _DIMENSION>;
    
private:
    double (*score_)[2];
    double *integratedScore_;
    
    gridIndex_t size_;
    gridPoint_t length_; // size of layer
    gridVariable_t step_; // step of variable
    gridVariableZone_t zone_; // variable zone
    
    // lower or higher bound
    template<typename T>static T& lb(std::array<T, 2>& azone)noexcept{ return std::get<0>(azone); }
    template<typename T>static T& lb(std::tuple<T, T>& azone)noexcept{ return std::get<0>(azone); }
    template<typename T>static T& hb(std::array<T, 2>& azone)noexcept{ return std::get<1>(azone); }
    template<typename T>static T& hb(std::tuple<T, T>& azone)noexcept{ return std::get<1>(azone); }
    
    void getPoint(gridIndex_t i, gridPoint_t* const ppoint){
        for (int d = _DIMENSION - 1;; --d){
            (*ppoint)[d] = i % length_[d];
            if (d == 0){ break; }
            i /= length_[d];
        }
    }
    void getVariable(const gridPoint_t& point, gridVariable_t *const pvar){
        for (int d = 0; d<_DIMENSION; ++d){
            (*pvar)[d] = zone_[d][0] + step_[d] * point[d];
        }
    }
    void getVariable(gridIndex_t i, gridVariable_t *const pvar){
        for (int d = _DIMENSION - 1;; --d){
            (*pvar)[d] = zone_[d][0] + step_[d] * (i % length_[d]);
            if (d == 0){ break; }
            i /= length_[d];
        }
    }
    
public:
    constexpr static int dimension()noexcept{ return _DIMENSION; }
    constexpr static int ld()noexcept{ return _DIMENSION - 1; }
    
    gridIndex_t size()const noexcept{ return size_; }
    gridIndex_t length(int d)const{ return length_[d]; }
    double lb(int d)const{ return lb(zone_[d]); }
    double hb(int d)const{ return hb(zone_[d]); }
    
    template<typename callback_t>
    void iterateGridVariable(const callback_t& callback){
        gridPoint_t point;
        gridVariable_t variable;
        for (int d=0; d<dimension(); ++d){
            point[d] = 0;
            variable[d] = lb(zone_[d]);
        }
        for (;;){
            callback(variable);
            for(int d = dimension()-1;; --d){
                ++point[d];
                variable[d] += step_[d];
                if(point[d] < length_[d]){
                    break;
                }else{
                    if(d == 0){ return; }
                    point[d] = 0;
                    variable[d] = lb(zone_[d]);
                }
            }
        }
    }
    template<typename callback_t>
    void iterateGridVariableWithPosition(const callback_t& callback){
        gridIndex_t i = 0;
        iterateGridVariable([callback, &i](const gridVariable_t& variable)->void{
            callback(variable, i);
            ++i;
        });
    }
    
    template<typename callback_t>
    void evalAllGrids(const callback_t& callback, int itr = 1){
        for(int i = 0; i < itr; ++i){
            iterateGridVariableWithPosition([this, callback](const gridVariable_t& variable, gridIndex_t j)->void{
                score_[j][0] += 1.0;
                score_[j][1] += callback(variable);
            });
        }
    }
    /*
     template<typename pdCallback_t>
     void integrate(const pdCallback_t& pdCallback){
     iterateGridVariableWithPosition([](const gridVariable_t& variable0, long long int i)->void{
     iterateGridValue([variable0, i](const gridVariable_t& variable1)->double{
     
     });
     });
     }
     */
    
    template<typename pdCallback_t>
    std::tuple<gridVariable_t, double> getBestIntegratedGridVariable(const pdCallback_t& pdCallback){
        gridIndex_t best = 0;
        double bestIntegratedScore = -DBL_MAX;
        iterateGridVariableWithPosition([this, pdCallback, &best, &bestIntegratedScore](const gridVariable_t& var0, gridIndex_t i0)->void{
            double scoreSum = 0;
            double pdfSum = 0;
            iterateGridVariableWithPosition([this, pdCallback, var0, &scoreSum, &pdfSum](const gridVariable_t& var1, gridIndex_t i1)->void{
                double pdf = pdCallback(var0, var1);
                //cerr << pdf << endl;
                scoreSum += score_[i1][1] / score_[i1][0] * pdf;
                pdfSum += pdf;
            });
            if(pdfSum > 0){
                double integratedScore = scoreSum;// / pdfSum;
                //cerr << integratedScore << " ";
                if(integratedScore > bestIntegratedScore){
                    best = i0;
                    bestIntegratedScore = integratedScore;
                }
            }
        });
        gridVariable_t variable;
        getVariable(best, &variable);
        return make_tuple(variable, bestIntegratedScore);
    }
    
    template<class length_t>
    void setGridSize(const length_t& alength){
        size_ = 1;
        for (int d=0; d<dimension(); ++d){
            length_[d] = alength[d];
            size_ *= alength[d];
        }
        score_ = new double[size_][2];
    }
    
    template<class zone_t>
    void setVariableZone(const zone_t& azone){
        for (int d=0; d<dimension(); ++d){
            zone_[d][0] = std::get<0>(azone[d]);
            zone_[d][1] = std::get<1>(azone[d]);
            step_[d] = (zone_[d][1] - zone_[d][0]) / length_[d];
        }
        if(score_ == nullptr){
            score_ = new double[size_][2];
        }
    }
    
    UncertaintyUniformGridSolver():
    score_(nullptr), integratedScore_(nullptr){
        length_.fill(1);
        size_ = 1;
    }
    
    UncertaintyUniformGridSolver(const gridIndex_t& alendth, const gridVariableZone_t& azone):
    score_(nullptr), integratedScore_(nullptr){
        setGridSize(alendth);
        setVariableZone(azone);
    }
    
    ~UncertaintyUniformGridSolver(){
        if(score_ != nullptr){
            delete[] score_;
            score_ = nullptr;
        }
        if(integratedScore_ != nullptr){
            delete[] integratedScore_;
            integratedScore_ = nullptr;
        }
    }
};

#endif // UTIL_GRID_HPP_