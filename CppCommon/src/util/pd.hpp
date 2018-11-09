/*
 pd.hpp
 Katsuki Ohto
 */

#ifndef UTIL_PD_HPP_
#define UTIL_PD_HPP_

// 確率分布の扱い
template<std::size_t I>
void normalize(double pd[I]){
    double sum = 0;
    for(int i = 0; i < I; ++i){
        sum += pd[i];
    }
    if(sum > 0){
        for(int i = 0; i < I; ++i){
            pd[i] *= 1 / sum;
        }
    }
}
template<std::size_t I, std::size_t J>
void normalize(double pd[I][J], int j){
    double sum = 0;
    for(int i = 0; i < I; ++i){
        sum += pd[i][j];
    }
    if(sum > 0){
        for(int i = 0; i < I; ++i){
            pd[i][j] *= 1 / sum;
        }
    }
}

template<std::size_t I, std::size_t J>
void normalize2(double pd[I][J], int R){
    for(int r = 0; r < R; ++r){
        // j方向で正規化
        for(int i = 0; i < I; ++i){
            double sum = 0;
            for(int j = 0; j < J; ++j){
                sum += pd[i][j];
            }
            if(sum > 0){
                for(int j = 0; j < J; ++j){
                    pd[i][j] *= 1 / sum;
                }
            }else{ // たまたま0になってしまった場合、全て等確率に戻す
                for(int j = 0; j < J; ++j){
                    pd[i][j] = 1 / double(J);
                }
            }
        }
        // i方向で正規化
        for(int j = 0; j < J; ++j){
            double sum = 0;
            for(int i = 0; i < I; ++i){
                sum += pd[i][j];
            }
            if(sum > 0){
                for(int i = 0; i < I; ++i){
                    pd[i][j] *= 1 / sum;
                }
            }else{ // たまたま0になってしまった場合、全て等確率に戻す
                for(int i = 0; i < I; ++i){
                    pd[i][j] = 1 / double(I);
                }
            }
        }
    }
}

template<std::size_t I, std::size_t J>
void normalize2(std::array<std::array<double, J>, I>& pd, int R){
    for(int r = 0; r < R; ++r){
        // j方向で正規化
        for(int i = 0; i < I; ++i){
            double sum = 0;
            for(int j = 0; j < J; ++j){
                sum += pd[i][j];
            }
            if(sum > 0){
                for(int j = 0; j < J; ++j){
                    pd[i][j] *= 1 / sum;
                }
            }else{ // たまたま0になってしまった場合、全て等確率に戻す
                for(int j = 0; j < J; ++j){
                    pd[i][j] = 1 / double(J);
                }
            }
        }
        // i方向で正規化
        for(int j = 0; j < J; ++j){
            double sum = 0;
            for(int i = 0; i < I; ++i){
                sum += pd[i][j];
            }
            if(sum > 0){
                for(int i = 0; i < I; ++i){
                    pd[i][j] *= 1 / sum;
                }
            }else{ // たまたま0になってしまった場合、全て等確率に戻す
                for(int i = 0; i < I; ++i){
                    pd[i][j] = 1 / double(I);
                }
            }
        }
    }
}

template<std::size_t I, std::size_t J, class is_t, class js_t>
void normalize2(double pd[I][J], int R, const is_t& isum, const js_t& jsum){
    for(int r = 0; r < R; ++r){
        // j方向で正規化
        for(int i = 0; i < I; ++i){
            double sum = 0;
            for(int j = 0; j < J; ++j){
                sum += pd[i][j];
            }
            if(sum > 0){
                for(int j = 0; j < J; ++j){
                    pd[i][j] *= jsum[i] / sum;
                }
            }else{ // たまたま0になってしまった場合、全て等確率に戻す
                for(int j = 0; j < J; ++j){
                    pd[i][j] = jsum[i] / double(J);
                }
            }
        }
        // i方向で正規化
        for(int j = 0; j < J; ++j){
            double sum = 0;
            for(int i = 0; i < I; ++i){
                sum += pd[i][j];
            }
            if(sum > 0){
                for(int i = 0; i < I; ++i){
                    pd[i][j] *= isum[j] / sum;
                }
            }else{ // たまたま0になってしまった場合、全て等確率に戻す
                for(int i = 0; i < I; ++i){
                    pd[i][j] = isum[j] / double(I);
                }
            }
        }
    }
}

double kullbackLeiblerDivergence(const double a[], const double b[], const int n){
    double kld = 0;
    for(int i = 0; i < n; ++i){
        kld += b[i] * log(b[i] / a[i]);
    }
    return kld;
}

double concordanceRate(const double a[], const double b[], const int n){
    double cr = 0;
    for(int i = 0; i < n; ++i){
        cr += a[i] * b[i];
    }
    return cr;
}

double absoluteError(const double a[], const double b[], const int n){
    double ae = 0;
    for(int i = 0; i < n; ++i){
        ae += fabs(a[i] - b[i]);
    }
    return ae;
}

double meanAbsoluteError(const double a[], const double b[], const int n){
    return absoluteError(a, b, n) / n;
}

double weightedMeanAbsoluteError(const double a[], const double b[], const int n){
    double ae = 0;
    for(int i = 0; i < n; ++i){
        ae += a[i] * fabs(a[i] - b[i]);
    }
    return ae;
}

#endif // UTIL_PD_HPP_




