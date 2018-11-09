/*
 linearlyInerpolatedTable.hpp
 Katsuki Ohto
 */

#ifndef UTIL_LINEARLY_INTERPOLATED_TABLE_HPP_
#define UTIL_LINEARLY_INTERPOLATED_TABLE_HPP_

#include <iostream>
#include <cstdio>
#include <array>
#include <initializer_list>
#include <tuple>

#include <boost/fusion/include/io.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>

#include "../defines.h"

namespace LinearlyInterpolatedTableSub{
    
    template<typename key_t>
    key_t calculateFraction(key_t key, key_t step, unsigned int i);
    
    template<>
    float calculateFraction(float key, float step, unsigned int i){
        return key - i*step;
    }
    template<>
    double calculateFraction(double key, double step, unsigned int i){
        return key - i*step;
    }
    template<>
    unsigned int calculateFraction(unsigned int key, unsigned int step, unsigned int i){
        return key % step;
    }
    template<>
    unsigned long long calculateFraction(unsigned long long key, unsigned long long step, unsigned int i){
        return key % step;
    }
}


template<class _data_t, typename _key_t, int _NUM, long long _MIN_NUM, long long _MAX_NUM, long long _DEN = 1>
class LinearlyInterpolatedTable{
    
public:
    using key_t = _key_t;
    using data_t = _data_t;
    using index_t = int;
    
    static void assert_key(key_t key){
        ASSERT(examKey(key),
               cerr << key << " in [" << MIN_ << ", " << MAX_ << "]" << endl;);
    }
    static void assert_index(index_t idx){
        ASSERT(examIndex(idx),
               cerr << idx << " in [" << 0 << ", " << ARRAY_SIZE_ << "]" << endl;);
    }
    static void assert_base_index(index_t idx){
        ASSERT(examBaseIndex(idx),
               cerr << idx << " in [" << 0 << ", " << NUM_ << "]" << endl;);
    }
    constexpr static bool examKey(key_t key)noexcept{
        return (MIN_ <= key && key < MAX_);
    }
    constexpr static bool examIndex(index_t idx)noexcept{
        return (0 <= idx && idx < ARRAY_SIZE_);
    }
    constexpr static bool examBaseIndex(index_t idx)noexcept{
        return (0 <= idx && idx < NUM_);
    }
    
    constexpr static std::size_t size()noexcept{ return NUM_; }
    constexpr static std::size_t arraySize()noexcept{ return ARRAY_SIZE_; }
    constexpr static key_t step()noexcept{ return STEP_; }
    constexpr static key_t minKey()noexcept{ return MIN_; }
    constexpr static key_t maxKey()noexcept{ return MAX_; }
    
    index_t getIndex(key_t key)const{
        assert_key(key);
        return (key - minKey()) / step();
    }
    key_t getKey(index_t idx)const{
        assert_index(idx);
        return (minKey() + (key_t)(idx * step()));
    }
    index_t getIndexSafely(key_t key)const noexcept{
        return min((index_t)size(), max((index_t)(0), (key - minKey()) / step()));
    }
    key_t getKeySafely(index_t idx)const noexcept{
        return (minKey() + (key_t)(min((index_t)size(), max((index_t)(0), idx)) * step()));
    }
    /*
     data_t operator[](key_t key)const{
     return accessByKey(key);
     }*/
    
    template<int _TUPLE_NUM>
    typename std::tuple_element<_TUPLE_NUM, data_t>::type at(key_t key)const{
        return accessByKey<_TUPLE_NUM>(key);
    }
    /*
     data_t accessByKey(key_t key)const{
     index_t idx=getIndex(key);
     assert_index(idx);
     key_t frac=LinearlyInterpolatedTableSub::calculateFraction<key_t>(key-minKey(),STEP_,idx);
     return (array_[idx]*(STEP_-frac)+array_[idx+1]*frac)/STEP_;
     }*/
    
    template<int _TUPLE_NUM>
    typename std::tuple_element<_TUPLE_NUM, data_t>::type accessByKey(key_t key)const{
        index_t idx = getIndex(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), STEP_, idx);
        return (std::get<_TUPLE_NUM>(array_[idx]) * (STEP_ - frac) + std::get<_TUPLE_NUM>(array_[idx + 1]) * frac) / STEP_;
    }
    template<int _TUPLE_NUM>
    typename std::tuple_element<_TUPLE_NUM, data_t>::type accessByKeySafely(key_t key)const{
        index_t idx = getIndexSafely(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), STEP_, idx);
        return (std::get<_TUPLE_NUM>(array_[idx]) * (STEP_ - frac) + std::get<_TUPLE_NUM>(array_[idx + 1]) * frac) / STEP_;
    }
    
    const data_t& accessByIndex(index_t idx)const{
        assert_index(idx);
        return array_[idx];
    }
    const data_t& accessByIndexSafely(index_t idx)const{
        index_t safeIndex = min((int)size(), max(0, idx));
        assert_index(idx);
        return array_[safeIndex];
    }
    
    void assign(index_t idx, const data_t& adat){
        assert_index(idx);
        array_[idx] = adat;
    }
    
    std::string toDebugString()const{
        std::ostringstream oss;
        oss << "size = " << size() << std::endl;
        oss << "array size = " << arraySize() << std::endl;
        oss << "step = " << step() << std::endl;
        oss << "min key = " << minKey() << std::endl;
        oss << "max key = " << maxKey() << std::endl;
        return oss.str();
    }
    
    std::string toDataString()const{
        std::ostringstream oss;
        for (int i = 0; i < ARRAY_SIZE_; ++i){
            oss << std::get<0>(array_[i]) << endl;
        }
        return oss.str();
    }
    
    LinearlyInterpolatedTable(){}
    
    LinearlyInterpolatedTable(std::initializer_list<data_t> list)
    {
        index_t idx = 0;
        for (auto itr = list.begin(); itr != list.end(); ++itr){
            cerr << std::get<0>(*itr) << "," << std::get<0>(*itr) << endl;
            array_[idx] = *itr;
            if (idx == ARRAY_SIZE_ - 1){ break; }
            ++idx;
        }
        
        //cerr << toDataString() << endl;
    }
    
private:
    constexpr static int NUM_ = _NUM;
    constexpr static int ARRAY_SIZE_ = NUM_ + 1;
    constexpr static key_t MIN_ = (key_t)_MIN_NUM / (key_t)_DEN;
    constexpr static key_t MAX_ = (key_t)_MAX_NUM / (key_t)_DEN;
    constexpr static key_t KEY_RANGE_ = (key_t)(_MAX_NUM - _MIN_NUM) / (key_t)_DEN;
    constexpr static key_t STEP_ = KEY_RANGE_ / (key_t)NUM_;
    constexpr static key_t INV_STEP_ = (key_t)NUM_ / KEY_RANGE_;
    
    data_t array_[ARRAY_SIZE_];
};


template<class _data_t, typename _key_t>
class LinearlyInterpolatedFreeTable{
    
public:
    using key_t = _key_t ;
    using data_t = _data_t;
    using index_t = unsigned int ;
    
    void assert_key(key_t key)const{
        ASSERT(examKey(key),
               cerr << key << " in [" << min_ << ", " << max_ << "]" << endl;);
    }
    void assert_index(index_t idx)const{
        ASSERT(examIndex(idx),
               cerr << idx << " in [" << 0 << ", " << array_size_<< "]" << endl;);
    }
    void assert_base_index(index_t idx)const{
        ASSERT(examBaseIndex(idx),
               cerr << idx << " in [" << 0 << ", " << n_ << "]" << endl;);
    }
    bool examKey(key_t key)const noexcept{
        return (min_ <= key && key<max_);
    }
    bool examIndex(index_t idx)const noexcept{
        return (0 <= idx && idx<array_size_);
    }
    bool examBaseIndex(index_t idx)const noexcept{
        return (0 <= idx && idx<n_);
    }
    
    std::size_t size()const noexcept{ return n_; }
    std::size_t arraySize()const noexcept{ return array_size_; }
    key_t step()const noexcept{ return step_; }
    key_t minKey()const noexcept{ return min_; }
    key_t maxKey()const noexcept{ return max_; }
    
    index_t getIndex(key_t key)const{
        assert_key(key);
        return (key - minKey()) / step();
    }
    key_t getKey(index_t idx)const{
        assert_index(idx);
        return (minKey() + (key_t)(idx * step()));
    }
    index_t getIndexSafely(key_t key)const{
        return min((index_t)size(), max((index_t)0, (key - minKey()) / step()));
    }
    key_t getKeySafely(index_t idx)const{
        return (minKey() + (key_t)(min((index_t)size(), max((index_t)0, idx)) * step()));
    }
    /*
     data_t operator[](key_t key)const{
     return accessByKey(key);
     }*/
    /*
     data_t accessByKey(key_t key)const{
     index_t idx=getIndex(key);
     assert_index(idx);
     key_t frac=LinearlyInterpolatedTableSub::calculateFraction<key_t>(key-minKey(),step_,idx);
     return (array_[idx]*(step_-frac)+array_[idx+1]*frac)/step_;
     }*/
    
    template<int _NUM>
    typename std::tuple_element<_NUM, data_t>::type accessByKey(key_t key)const{
        index_t idx = getIndex(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), step_, idx);
        return (std::get<_NUM>(array_[idx])*(step_ - frac) + std::get<_NUM>(array_[idx + 1])*frac) / step_;
    }
    template<int _NUM>
    typename std::tuple_element<_NUM, data_t>::type accessByKeySafely(key_t key)const{
        index_t idx = getIndexSafely(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), step_, idx);
        return (std::get<_NUM>(array_[idx])*(step_ - frac) + std::get<_NUM>(array_[idx + 1])*frac) / step_;
    }
    
    const data_t& accessByIndex(index_t idx)const{
        assert_index(idx);
        return array_[idx];
    }
    const data_t& accessByIndexSafely(index_t idx)const{
        index_t safeIndex = min((index_t)size(), max((index_t)0, idx));
        assert_index(idx);
        return array_[safeIndex];
    }
    
    void assign(index_t idx, const data_t& adat){
        array_[idx] = adat;
    }
    
    std::string toDebugString()const{
        std::ostringstream oss;
        oss << "size = " << size() << std::endl;
        oss << "array size = " << arraySize() << std::endl;
        oss << "step = " << step() << std::endl;
        oss << "min key = " << minKey() << std::endl;
        oss << "max key = " << maxKey() << std::endl;
        return oss.str();
    }
    
    std::string toDataString()const{
        std::ostringstream oss;
        for (int i = 0; i < array_size_; ++i){
            oss << boost::fusion::as_vector(array_[i]) << "," << endl;
        }
        return oss.str();
    }
    
    LinearlyInterpolatedFreeTable(int an, key_t amin, key_t amax)
    :n_(an), array_size_(n_ + 1), min_(amin), max_(amax), step_((amax - amin) / (key_t)n_)
    {
        array_ = new data_t[array_size_];
    }
    
    LinearlyInterpolatedFreeTable(int an, key_t amin, key_t amax, std::initializer_list<data_t> list)
    :n_(an), array_size_(n_ + 1), min_(amin), max_(amax), step_((amax - amin) / (key_t)n_)
    {
        array_ = new data_t[array_size_];
        index_t idx = 0;
        for (auto itr = list.begin(); itr != list.end(); ++itr){
            array_[idx] = std::make_tuple(*itr);
            if (idx == array_size_ - 1){ break; }
            ++idx;
        }
    }
    
    ~LinearlyInterpolatedFreeTable(){
        delete[] array_;
    }
    
private:
    const int n_;
    const int array_size_;
    const key_t min_, max_;
    const key_t step_;
    
    data_t *array_;
};
/*
template<class _data_t, typename _key_t, int _NUM, long long _MIN_NUM, long long _MAX_NUM, long long _DEN = 1>
class MDLinearlyInterpolatedTable{
    
public:
    using key_t = _key_t;
    using data_t = _data_t;
    using index_t = int;
    
    static void assert_key(key_t key){
        ASSERT(examKey(key),
               cerr << key << " in [" << MIN_ << ", " << MAX_ << "]" << endl;);
    }
    static void assert_index(index_t idx){
        ASSERT(examIndex(idx),
               cerr << idx << " in [" << 0 << ", " << ARRAY_SIZE_ << "]" << endl;);
    }
    static void assert_base_index(index_t idx){
        ASSERT(examBaseIndex(idx),
               cerr << idx << " in [" << 0 << ", " << NUM_ << "]" << endl;);
    }
    constexpr static bool examKey(key_t key)noexcept{
        return (MIN_ <= key && key < MAX_);
    }
    constexpr static bool examIndex(index_t idx)noexcept{
        return (0 <= idx && idx < ARRAY_SIZE_);
    }
    constexpr static bool examBaseIndex(index_t idx)noexcept{
        return (0 <= idx && idx < NUM_);
    }
    
    constexpr static std::size_t size()const noexcept{ return NUM_; }
    constexpr static std::size_t arraySize()const noexcept{ return ARRAY_SIZE_; }
    constexpr static key_t step()const noexcept{ return STEP_; }
    constexpr static key_t minKey()const noexcept{ return MIN_; }
    constexpr static key_t maxKey()const noexcept{ return MAX_; }
    
    index_t getIndex(key_t key)const{
        assert_key(key);
        return (key - minKey()) / step();
    }
    key_t getKey(index_t idx)const{
        assert_index(idx);
        return (minKey() + (key_t)(idx * step()));
    }
    index_t getIndexSafely(key_t key)const noexcept{
        return min((index_t)size(), max((index_t)(0), (key - minKey()) / step()));
    }
    key_t getKeySafely(index_t idx)const noexcept{
        return (minKey() + (key_t)(min((index_t)size(), max((index_t)(0), idx)) * step()));
    }
     data_t operator[](key_t key)const{
     return accessByKey(key);
     }
    
    template<int _TUPLE_NUM>
    typename std::tuple_element<_TUPLE_NUM, data_t>::type at(key_t key)const{
        return accessByKey<_TUPLE_NUM>(key);
    }
 
     data_t accessByKey(key_t key)const{
     index_t idx=getIndex(key);
     assert_index(idx);
     key_t frac=LinearlyInterpolatedTableSub::calculateFraction<key_t>(key-minKey(),STEP_,idx);
     return (array_[idx]*(STEP_-frac)+array_[idx+1]*frac)/STEP_;
     }
    
    template<int _TUPLE_NUM>
    typename std::tuple_element<_TUPLE_NUM, data_t>::type accessByKey(key_t key)const{
        index_t idx = getIndex(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), STEP_, idx);
        return (std::get<_TUPLE_NUM>(array_[idx])*(STEP_ - frac) + std::get<_TUPLE_NUM>(array_[idx + 1])*frac) / STEP_;
    }
    template<int _TUPLE_NUM>
    typename std::tuple_element<_TUPLE_NUM, data_t>::type accessByKeySafely(key_t key)const{
        index_t idx = getIndexSafely(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), STEP_, idx);
        return (std::get<_TUPLE_NUM>(array_[idx])*(STEP_ - frac) + std::get<_TUPLE_NUM>(array_[idx + 1])*frac) / STEP_;
    }
    
    const data_t& accessByIndex(index_t idx)const{
        assert_index(idx);
        return array_[idx];
    }
    const data_t& accessByIndexSafely(index_t idx)const{
        index_t safeIndex = min(size(), max(0, idx));
        assert_index(idx);
        return array_[safeIndex];
    }
    
    void assign(index_t idx, const data_t& adat){
        assert_index(idx);
        array_[idx] = adat;
    }
    
    std::string toDebugString()const{
        std::ostringstream oss;
        oss << "size = " << size() << std::endl;
        oss << "array size = " << arraySize() << std::endl;
        oss << "step = " << step() << std::endl;
        oss << "min key = " << minKey() << std::endl;
        oss << "max key = " << maxKey() << std::endl;
        return oss.str();
    }
    
    std::string toDataString()const{
        std::ostringstream oss;
        for (int i = 0; i < ARRAY_SIZE_; ++i){
            oss << std::get<0>(array_[i]) << endl;
        }
        return oss.str();
    }
    
    LinearlyInterpolatedTable(){}
    
    LinearlyInterpolatedTable(std::initializer_list<data_t> list)
    {
        index_t idx = 0;
        for (auto itr = list.begin(); itr = list.end(); ++itr){
            cerr << std::get<0>(*itr) << "," << std::get<0>(*itr) << endl;
            array_[idx] = *itr;
            if (idx == ARRAY_SIZE_ - 1){ break; }
            ++idx;
        }
        
        //cerr << toDataString() << endl;
    }
    
private:
    constexpr static int NUM_ = _NUM;
    constexpr static int ARRAY_SIZE_ = NUM_ + 1;
    constexpr static key_t MIN_ = (key_t)_MIN_NUM / (key_t)_DEN;
    constexpr static key_t MAX_ = (key_t)_MAX_NUM / (key_t)_DEN;
    constexpr static key_t KEY_RANGE_ = (key_t)(_MAX_NUM - _MIN_NUM) / (key_t)_DEN;
    constexpr static key_t STEP_ = KEY_RANGE_ / (key_t)NUM_;
    constexpr static key_t INV_STEP_ = (key_t)NUM_ / KEY_RANGE_;
    
    data_t array_[ARRAY_SIZE_];
};
*/
template<class _data_t, typename _key_t>
class MDLinearlyInterpolatedFreeTable{
    
public:
    typedef _key_t key_t;
    typedef _data_t data_t;
    typedef int index_t;
    typedef unsigned int dimension_t;
    
    void assert_dimension(int dimension)const{
        ASSERT(examDimension(dimension),
               cerr << dimension << " in [" << 0 << ", " << dimension << "]" << endl;);
    }
    void assert_key(int dimension, key_t key)const{
        ASSERT(examKey(dimension, key),
               cerr << key << " in [" << min_[dimension] << ", " << max_[dimension] << "]" << endl;);
    }
    void assert_index(index_t idx)const{
        ASSERT(examIndex(idx),
               cerr << idx << " in [" << 0 << ", " << array_size_<< "]" << endl;);
    }
    void assert_base_index(index_t idx)const{
        ASSERT(examBaseIndex(idx),
               cerr << idx << " in [" << 0 << ", " << n_ << "]" << endl;);
    }
    bool examDimension(dimension_t dimension)const noexcept{
        return (0 <= dimension && dimension < dimensions_);
    }
    bool examKey(dimension_t dimension, key_t key)const noexcept{
        return (min_[dimension] <= key && key < max_[dimension]);
    }
    bool examIndex(index_t idx)const noexcept{
        return (0 <= idx && idx < array_size_);
    }
    bool examBaseIndex(index_t idx)const noexcept{
        return (0 <= idx && idx < n_);
    }
    
    std::size_t dimensions()const noexcept{ return dimensions_; }
    std::size_t size()const noexcept{ return n_; }
    std::size_t arraySize()const noexcept{ return array_size_; }
    key_t step()const noexcept{ return step_; }
    key_t minKey()const noexcept{ return min_; }
    key_t maxKey()const noexcept{ return max_; }
    
    index_t getIndex(key_t key)const{
        assert_key(key);
        return (key - minKey()) / step();
    }
    key_t getKey(index_t idx)const{
        assert_index(idx);
        return (minKey() + (key_t)(idx * step()));
    }
    /*
     data_t operator[](key_t key)const{
     return accessByKey(key);
     }*/
    
    data_t accessByKey(key_t key)const{
        index_t idx = getIndex(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), step_, idx);
        return (array_[idx] * (step_ - frac) + array_[idx + 1] * frac) / step_;
    }
    
    template<int _NUM>
    typename std::tuple_element<_NUM, data_t>::type accessByKey(key_t key)const{
        index_t idx = getIndex(key);
        assert_index(idx);
        key_t frac = LinearlyInterpolatedTableSub::calculateFraction<key_t>(key - minKey(), step_, idx);
        return (std::get<_NUM>(array_[idx])*(step_ - frac) + std::get<_NUM>(array_[idx + 1])*frac) / step_;
    }
    
    const data_t& accessByIndex(index_t idx)const{
        assert_index(idx);
        return array_[idx];
    }
    
    void assign(index_t idx, const data_t& adat){
        assert_index(idx);
        array_[idx] = adat;
    }
    
    std::string toDebugString()const{
        std::ostringstream oss;
        oss << "size = " << size() << std::endl;
        oss << "array size = " << arraySize() << std::endl;
        oss << "step = " << step() << std::endl;
        oss << "min key = " << minKey() << std::endl;
        oss << "max key = " << maxKey() << std::endl;
        return oss.str();
    }
    
    std::string toDataString()const{
        std::ostringstream oss;
        for (int i = 0; i < array_size_; ++i){
            oss << "{";
            for (int d = 0; d<dimensions_; ++d){
                oss << boost::fusion::as_vector(array_[i][d]) << "," << endl;
            }
            oss << "}" << endl;
        }
        return oss.str();
    }
    
    MDLinearlyInterpolatedFreeTable(int adimensions, int an, key_t *const amin, key_t *const amax)
    :dimensions_(adimensions), n_(an), array_size_(n_ + 1), min_(amin), max_(amax), step_((amax - amin) / (key_t)n_)
    {
        array_ = new data_t[array_size_][dimensions_];
        min_ = new key_t[dimensions_];
        max_ = new key_t[dimensions_];
        step_ = new key_t[dimensions_];
    }
    
    MDLinearlyInterpolatedFreeTable(int adimensions, int an, key_t *const amin, key_t *const amax,
                                                  std::initializer_list<std::initializer_list<data_t>> list)
    :dimensions_(adimensions), n_(an), array_size_(n_ + 1), min_(amin), max_(amax), step_((amax - amin) / (key_t)n_)
    {
        array_ = new data_t[array_size_][dimensions_];
        min_ = new key_t[dimensions_];
        max_ = new key_t[dimensions_];
        step_ = new key_t[dimensions_];
        
        int dimension = 0;
        index_t idx = 0;
        for (auto itr = list.begin(); itr != list.end(); ++itr){
            for (auto itr1 = itr->begin(); itr1 != itr->end(); ++itr1){
                array_[idx][dimension] = *itr1;
                if (dimension == dimensions_ - 1){ break; }
                ++dimension;
            }
            if (idx == array_size_ - 1){ break; }
            ++idx;
        }
    }
    
    ~MDLinearlyInterpolatedFreeTable(){
        delete[] min_;
        delete[] max_;
        delete[] step_;
        delete[] array_;
    }
    
private:
    const int dimensions_;
    const int n_;
    const int array_size_;
    key_t *min_, *max_;
    key_t *step_;
    
    data_t *array_;
};

#endif // UTIL_LINEARLY_INTERPOLATED_TABLE_HPP_