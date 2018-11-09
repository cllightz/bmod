#ifndef _ARRAY_HPP_
#define _ARRAY_HPP_

#include<cassert>

//配列
//収納個数のデータを一緒に持つ

template<class data_t,int SIZE>
struct Stack{
    
    int qty;
    data_t data[SIZE];
    
    void shift(const int n){
        //指定分ずらす
        //サイズよりも大きくずらそうとした場合にはサイズを0にする
        int dist=qty-n;
        for(int i=0;i<dist;++i){
            data[i]=data[i+n];
        }
        if(dist>0){
            qty-=dist;
        }else{
            qty=0;
        }
    }
    
    void push(const data_t& arg){
        assert( qty<SIZE );
        data[qty]=arg;
        ++qty;
    }
    
    int size()const{return qty;}
    
    data_t getData(const int id)const{
        return data[id];
    }
    
    bool any()const{
        assert(qty >= 0);
        if(qty != 0){return true;}else{return false;}
    }
    
    void pop(){
        assert(qty > 0);
        --qty;
    }
    
    data_t pop_get(){
        pop();
        return data[qty];
    }
    
    void set(data_t arg){
        //初期化して一番下に置く
        qty=0;
        data[0]=arg;
    }
    
    void init(){qty=0;}
    
    void clear(){
        init();
        memset(data,0,sizeof(data_t)*SIZE);//memory clear
    }
    
    Array(){
        init();
    }
    
    ~Array(){}
};

#endif /* _STACK_HPP_ */