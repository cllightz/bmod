#ifndef UTIL_BISTACK_HPP_
#define UTIL_BISTACK_HPP_

#include <cassert>

//双方向性スタック
//サイズ固定

template<class data_t,int SIZE=6 >
class BiStack{
private:
    bool exam_idx(int idx){
        if( idx<0 || SIZE-1<idx ){return false;}
        return true;
    }
public:
    int st,ed;
    data_t data[SIZE];
    
    /*void shift(const int n){
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
     
     void put_back(data_t arg){
     //オーバーしたら何もしない
     if(qty != SIZE){
     data[qty]=arg;
     ++qty;
     }
     }
     
     void push_back(data_t arg){
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
     }*/
    
    void push_front_fast(const data_t& adt){
        //オーバーしないと仮定
        assert( exam_idx(st) );
        data[st]=adt;
        --st;
    }
    
    void push_back_fast(const data_t& adt){
        //オーバーしないと仮定
        assert( exam_idx(ed) );
        data[ed]=adt;
        ++ed;
    }
    
    void replace_front(const data_t& arg){
        data[0]=arg;
    }
    void replace_back(const data_t& arg){
        data[SIZE-1]=arg;
    }
    
    int size()const{return ed-st-1;}
    
    data_t get_data(const int idx)const{
        return data[idx];
    }
    
    bool any()const{
        if( size() ){return true;}else{return false;}
    }
    
    void pop_front(){
        assert( exam_idx(st) );
        ++st;
    }
    
    void pop_back(){
        assert( exam_idx(ed) );
        --ed;
    }
    
    const data_t& front()const{
        assert( exam_idx(st+1) );
        return data[st+1];
    }
    
    const data_t& back()const{
        assert( exam_idx(ed-1) );
        return data[ed-1];
    }
    
    data_t* begin(){
        assert( exam_idx(st+1) );
        return &data[st+1];
    }
    
    data_t* end(){
        assert( exam_idx(ed-1) );
        return &data[ed-1];
    }
    
    void init(int bound){
        assert( exam_idx(bound) && exam_idx(bound-1) );
        st=bound-1;
        ed=bound;
    }
    
    void clear(int bound){
        init(bound);
        memset(data,0,sizeof(data_t)*SIZE);//memory clear
    }
    
    BiStack(int bound)
    { init(bound); }
    
    ~BiStack(){}
};

#endif // UTIL_BISTACK_HPP_