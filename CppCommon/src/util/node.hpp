/*
 node.hpp
 Katsuki Ohto
 */

#ifndef UTIL_NODE_HPP_
#define UTIL_NODE_HPP_

#include "../defines.h"
#include "../analyze/analyzer.hpp"
#include "lock.hpp"

// 並列化可能な局面データ

template<class _lock_t, int IDENTITY_SIZE>
class AtomicNode{
private:
    // スピンロック兼局面の同一性の弱確認
    
#ifdef MULTI_THREADING
    using plock_t = _lock_t;
    mutable plock_t lock_;
#else
    using plock_t = NullNodeLock;
    uint64_t lock_;
#endif
    
public:
    using lock_t = plock_t;
    
    // 同一性確認のためのマスク
    constexpr static uint64_t IDENTITY_MASK = ((1ULL << IDENTITY_SIZE) - 1ULL) << lock_t::lock_size();
    
    AtomicNode(): lock_(0ULL){}
    
    constexpr static uint64_t knitIdentityValue(uint64_t ahash, uint64_t aval)noexcept{
        return (ahash & (~IDENTITY_MASK)) | (aval << lock_t::lock_size());
    }
    bool any()const noexcept{
        return lockValue() ? true : false;
    }
    
    // 排他制御のための関数
#ifdef MULTI_THREADING
    uint64_t lockValue()const noexcept{ return lock_.data(); }
    void startReading()const noexcept{ lock_.start_read(); }
    void finishReading()const noexcept{ lock_.finish_read(); }
    void startFeeding()const noexcept{ lock_.start_feed(); }
    void finishFeeding()const noexcept{ lock_.finish_feed(); }
    void startMaking()const noexcept{ lock_.start_make(); }
    void finishMaking()const noexcept{ lock_.finish_make(); }
    bool isMaking()const noexcept{ return lock_.is_being_made(); }
    bool isReading()const noexcept{ return lock_.is_being_read(); }
    bool regist(uint64_t val)noexcept{ return lock_.regist(val); }
    bool registAndStartMaking(uint64_t val)noexcept{ return lock_.regist_start_make(val); }
    void forceRegist(uint64_t val)noexcept{ return lock_.force_regist(val); }
    bool compare(uint64_t val)const noexcept{ return lock_.compare(val); }
    constexpr static bool compare(uint64_t val0, uint64_t val1)noexcept{ return lock_t::compare(val0, val1); }
#else
    uint64_t lockValue()const noexcept{ return lock_; }
    void startReading()const noexcept{}
    void finishReading()const noexcept{}
    void startFeeding()const noexcept{}
    void finishFeeding()const noexcept{}
    void startMaking()const noexcept{}
    void finishMaking()const noexcept{}
    bool isMaking()const noexcept{ return false; }
    bool isReading()const noexcept{ return false; }
    bool regist(uint64_t val)noexcept{ lock_ = val; return true; }
    bool registAndStartMaking(uint64_t val)noexcept{ return regist(val); }
    void forceRegist(uint64_t val)noexcept{ lock_ = val; }
    bool compare(uint64_t val)const noexcept{ return (lock_ == val); }
    constexpr static bool compare(uint64_t val0, uint64_t val1)noexcept{ return (val0 == val1); }
#endif
};

template<class _page_t, int _SIZE, int _REHASH_MAX>
class NodeTranspositionTable{ // 開番地法局面置換表
public:
    using page_t = _page_t;
    
private:
    using index_t = int;
    
    constexpr static int SIZE_ = _SIZE;
    constexpr static uint64_t DELETED_ = 0xffffffffffffffff;
    constexpr static int REHASH_MAX_ = _REHASH_MAX;
    constexpr static int REAL_SIZE_ = SIZE_ + REHASH_MAX_ * 2;
    
    constexpr static bool exam_index(index_t index)noexcept{
        return (0 <= index && index < REAL_SIZE_);
    }
    static void assert_index(const index_t index){
        ASSERT(exam_index(index),
               cerr << "invalid index " << index << " in(0 ~ " << (REAL_SIZE_ - 1) << ")" << endl;);
    }
    
    constexpr static index_t convHash_Index(const uint64_t hash)noexcept{
        return index_t(hash % (unsigned int)SIZE_);
    }
    constexpr static index_t procIndex(const index_t index)noexcept{
        return index + 1;
    }
    bool examPagePointer(const page_t *const p)const noexcept{
        return (&this->page(0) <= p && p <= &this->page(REAL_SIZE_ - 1));
    }
    int convPointer_Index(const page_t *const p)const noexcept{
        return p - &this->page(0);
    }
    void assert_page_pointer(const page_t *const p)const{
        ASSERT(examPagePointer(p),
               cerr << "invalid pointer index " << convPointer_Index(p);
               cerr << " in(0 ~ " << (REAL_SIZE_ - 1) << ")" << endl;);
    }
    
    std::atomic<uint32_t> pages_;
public:
    HashBookAnalyzer ana;
    
    page_t page_[REAL_SIZE_];
    
    NodeTranspositionTable():
    ana("NodeTranspositionTable", REAL_SIZE_, sizeof(*this)){}
    
    bool isFilledOver(double percentage)const noexcept{
        return (pages_ > (REAL_SIZE_ * percentage));
    }
    
    uint32_t pages()const noexcept{ return pages_; }
    const page_t& page(const index_t index)const{ assert_index(index); return page_[index]; }
    page_t& page(const index_t index){ assert_index(index); return page_[index]; }
    
    void init(){
        memset(page_, 0, sizeof(page_));
        pages_ = 0;
        ana.init();
    }
    
    page_t* registAndStartMaking(const uint64_t value, page_t *const pfirst)noexcept{
        ASSERT(0 <= (pfirst - page_) && (pfirst - page_) < (SIZE_ + REHASH_MAX_),);
        page_t *p = pfirst;
        if(p->registAndStartMaking(value)){ // 登録成功
            //cerr << value << endl;
            ++pages_; ana.addRegistration(); ana.addFilled();
            assert_page_pointer(p);
            return p;
        }
        if(REHASH_MAX_ > 0){
            const page_t *const plast = p + REHASH_MAX_ - 1;
            for(;;){
                ++p;
                if(!p->lockValue()){
                    if(p->registAndStartMaking(value)){ // 登録成功
                        ++pages_; ana.addRegistration(); ana.addFilled();
                        assert_page_pointer(p);
                        return p;
                    }
                }
                if(p == plast){ ana.addRegistrationFailure(); return nullptr; }
            }
        }
        UNREACHABLE;
    }
    
    page_t* regist(const uint64_t value, page_t *const pfirst)noexcept{
        ASSERT(0 <= (pfirst - page_) && (pfirst - page_) < (SIZE_ + REHASH_MAX_),);
        page_t *p = pfirst;
        if(p->regist(value)){ // 登録成功
            //cerr << value << endl;
            ++pages_; ana.addRegistration(); ana.addFilled();
            assert_page_pointer(p);
            return p;
        }
        if(REHASH_MAX_ > 0){
            const page_t *const plast = p + REHASH_MAX_ - 1;
            for(;;){
                ++p;
                if(!p->lockValue()){
                    if(p->regist(value)){ // 登録成功
                        ++pages_; ana.addRegistration(); ana.addFilled();
                        assert_page_pointer(p);
                        return p;
                    }
                }
                if(p == plast){ ana.addRegistrationFailure(); return nullptr; }
            }
        }
        UNREACHABLE;
    }
    
    page_t* read(const uint64_t ahash, const uint64_t avalue, bool *const pfound)noexcept{
        int index = convHash_Index(ahash);
        page_t *p = &page_[index];
        const page_t *const plast = p + REHASH_MAX_ - 1;
        for(;;){
            const uint64_t value = p->lockValue();
            if(!value){ // 1行上の時点では空
                *pfound = false; ana.addWhite();
                assert_page_pointer(p);
                return p;
            }else if(p->compare(value, avalue)){ // 弱い同一性確認
                //cerr << "same " << std::hex << value << ", " << avalue << std::dec << endl;
                *pfound = true; ana.addHit();
                assert_page_pointer(p);
                return p;
            }else if(p == plast){
                *pfound = false; ana.addUnfounded();
                return nullptr;
            }
            //cerr << "diff " << std::hex << value << ", " << avalue << std::dec << endl;getchar();
            ++p;
        }
        UNREACHABLE;
    }
    
    template<class callback_t>
    int collect(const callback_t& callback)noexcept{
        // 条件で指定したデータを消して整理する
        int cleared = 0;
        for(int i = 0; i < REAL_SIZE_; ++i){
            if(callback(page(i))){ // もう不要
                page(i).forceRegist(0); // データが入っていないことを示す
                ++cleared;
            }else{ // まだ使えるかもしれない
                const uint64_t hash = page(i).lockValue();
                const index_t bi = convHash_Index(hash);
                for(int ii = bi; ii < i; ++ii){
                    if(page(ii).lockValue() == 0){ // 場所がある
                        memcpy(&page(ii), &page(i), sizeof(page_t)); // 移動
                        page(i).forceRegist(0); // データが入っていないことを示す
                        break;
                    }
                }
            }
        }
        pages_ -= cleared;
        return cleared;
    }
    /*
     page_t* readAndRegist(const uint64_t ahash, const uint64_t avalue, bool *const pfound)noexcept{
     int index = convHash_Index(ahash);
     page_t *p = &page[index];
     const page_t *const plast = p + REHASH_MAX_ - 1;
     for(;;){
     const uint64_t value = p->lockValue;
     if(!value){ // 1行上の時点では空
     page_t *pregisted = regist(avalue, p);
     *pfound = false; ana.addWhite();
     return pregisted;
     }else if(p->compare(value, avalue)){ // 弱い同一性確認
     *pfound = true; ana.addHit();
     return p;
     }else if(p == plast){
     *pfound = false; ana.addDiffHash();
     return nullptr;
     }
     ++p;
     }
     UNREACHABLE;
     }
     void print()const{
     for (int i = 0; i < REAL_SIZE_; ++i){
					if (page(i).lockValue() && page(i).lockValue() != DELETED_){
     cerr << "node " << i << " " << page(i) << endl;
					}
     }
     }
     */
};

#endif // UTIL_NODE_HPP_