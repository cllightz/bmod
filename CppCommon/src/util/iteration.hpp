/*
iteration.hpp
Katsuki Ohto
*/

#ifndef UTIL_ITERATION_HPP_
#define UTIL_ITERAITON_HPP_

#include "../defines.h"

// ソート済みの2列のデータを、足並みを揃えながら見る
template<
class array0_t, class array1_t,
class soloCallback0_t, class soloCallback1_t, class biCallback_t,
class equalCallback_t, class inequalCallback_t>
void dateIterate(const array0_t& arr0, const int n0, const soloCallback0_t&
                 const array1_t& arr1, const int n1, const soloCallback1_t&
                 const callback_t& biCallback,
                 const equalCallback_t& isEqual, const inequalCallback_t& isPrior){
    int i0 = 0, i1 = 0;
    for(;;){
        if(isEqual(arr0[i0], arr1[i1])){ // 一致
            callback(arr0[i0], arr1[i1]);
            
            ++i0; ++i1;
            if(i0 >= n0){
                for(;i1 < n1; ++i1){
                    soloCallback1(arr1[i1]);
                }
                break;
            }
            if(i1 >= n1){
                for(;i0 < n0; ++i0){
                    soloCallback0(arr0[i0]);
                }
                break;
            }
        }else if(isPrior(arr0[i0], arr1[i1])){ // arr1の方が進んだ
            ++i0;
            if(i0 >= n0){
                for(;i1 < n1; ++i1){
                    soloCallback1(arr1[i1]);
                }
                break;
            }
        }else{ // arr0の方が進んだ
            ++i1;
            if(i1 >= n1){
                for(;i0 < n0; ++i0){
                    soloCallback0(arr0[i0]);
                }
                break;
            }
        }
    }
}


#endif // UTIL_ITERATION_HPP_