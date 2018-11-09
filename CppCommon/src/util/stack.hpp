#ifndef _STACK_HPP_
#define _STACK_HPP_

#include<cassert>

//スタック
//サイズ固定で、オーバーした場合の対処は適当

template<class data_t,int SIZE=6 >
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
    
    void put(data_t arg)noexcept{
        //オーバーしたら何もしない
        if(qty != SIZE){
            data[qty]=arg;
            ++qty;
        }
    }
    
    void push(data_t arg)noexcept{
        //オーバーしたら最後のを消して後ろにずらす
        
        if(qty != SIZE){
            data[qty]=arg;
            ++qty;
        }else{
            for(int i=0;i<(SIZE-1);++i){
                data[i]=data[i+1];
            }
            data[SIZE-1]=arg;
        }
    }
    
    void replaceTop(data_t arg){
        data[qty-1]=arg;
    }
    
    int size()const noexcept{return qty;}
    
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
    
    data_t top()const{
        assert(qty > 0);
        return data[qty-1];
    }
    
    data_t second()const{
        assert(qty > 1);
        return data[qty-2];
    }
    
    void set(data_t arg)noexcept{
        //初期化して一番下に置く
        qty=0;
        data[0]=arg;
    }
    
    void init()noexcept{qty=0;}
    
    void clear(){
        init();
        memset(data,0,sizeof(data_t)*SIZE);//memory clear
    }
    
    Stack(){
        init();
    }
    
    ~Stack(){}
};

#endif /* _STACK_HPP_ */