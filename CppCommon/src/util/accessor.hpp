/*
accessor.hpp
Katsuki Ohto
*/

#ifndef UTIL_ACCESSOR_HPP_
#define UTIL_ACCESSOR_HPP_

template<class _data_t>
class RandomAccessor{
    //ランダム順なアクセス用
public:
    using data_t = _data_t;
    using value_type = _data_t;
    
    void assert_index(int s)const{
        ASSERT(s < size() && s < order_.size(),
               std::cerr << "s = " << s << " data size = " << size() << " order size = " << order_.size() << std::endl;);
    }
    
    std::size_t size()const noexcept{ return data_.size(); }
    
    void initOrder(){
        order_.clear();
        order_.reserve(size());
        for(int i = 0, n = size(); i < n; ++i){
            order_.push_back(i);
        }
    }
    template<class dice_t>
    void shuffle(const int first, const int end, dice_t& dice){
        //random shuffle
        for(int i = min((int)size(), end) - 1; i > max(0, first); --i){
            std::swap(order_[i], order_[dice() % ((i - first) + 1)]);
        }
    }
    template<class dice_t>
    void shuffle(dice_t& dice){
        shuffle(0, size(), dice);
    }
    
    const data_t& access(const int s)const{
        assert_index(s);
        return data_[order_[s]];
    }
    data_t& access(const int s){
        assert_index(s);
        return data_[order_[s]];
    }
    void clear()noexcept{
        data_.clear();
        order_.clear();
    }
    void push_back(const data_t& arg){
        data_.push_back(arg);
    }
    void emplace_back(const data_t& arg){
        data_.emplace_back(arg);
    }
    
    data_t& data(int n){ return data_[n]; }
    const data_t& data(int n)const{ return data_[n]; }
    
protected:
    std::vector<data_t> data_;
    std::vector<int> order_;
};

template<class data_t, typename callback_t>
void iterate(const RandomAccessor<data_t>& db, const callback_t& callback){
    for(int i = 0, n = db.size(); i < n; ++i){
        callback(db.data(i));
    }
}

template<class data_t, typename callback_t>
void iterate(RandomAccessor<data_t>& db, const callback_t& callback){
    for(int i = 0, n = db.size(); i < n; ++i){
        callback(db.data(i));
    }
}

template<class data_t, typename callback_t>
void iterateRandomly(const RandomAccessor<data_t>& db, int first, int end, const callback_t& callback){
    for(int i = max(0, first); i < min((int)db.size(), end); ++i){
        callback(db.access(i));
    }
}

#endif // UTIL_ACCESSOR_HPP_

