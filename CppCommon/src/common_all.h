#ifndef COMMON_ALL_H_
#define COMMON_ALL_H_

//commonディレクトリ内全てをインクルードする
//簡単に使うためのラッパークラスも用意する

//基本的定義
#include "defines.h"

//乱数
/*
#include"lib/SFMT-src-1.4.1/SFMT.h"
#include"lib/SFMT-src-1.4.1/SFMT.hpp"

struct SFMT32{
sfmt_t sfmt;
uint32_t rand(){
return sfmt_genrand_uint32_t(&sfmt);
}
void srand(uint32_t s){
sfmt_init_gen_rand(&sfmt,s);
}
};
*/
/*
*/
/*
#include"lib/TinyMT-src-1.0.3/tinymt/tinymt32.h"
#include"lib/TinyMT-src-1.0.3/tinymt/tinymt32.hpp"
struct TinyMT32{
sfmt_t sfmt;
uint32_t rand(){
return sfmt_genrand_uint32_t(&sfmt);
}
void srand(uint32_t s){
sfmt_init_gen_rand(&sfmt,s);
}
};
*/
/*
#include"lib/TinyMT-src-1.0.3/tinymt/tinymt64.h"
#include"lib/TinyMT-src-1.0.3/tinymt/tinymt64.hpp"
struct TinyMT64{
sfmt_t sfmt;
uint32_t rand(){
return sfmt_genrand_uint32_t(&sfmt);
}
void srand(uint32_t s){
sfmt_init_gen_rand(&sfmt,s);
}
};
*/

//#include "type.hpp"

// 文字列処理
#include "util/string.hpp"

#include "util/xorShift.hpp"
#include "util/rkiss.hpp"
#include "util/random.hpp"

// ビット演算
#include "util/bitOperation.hpp"

// 包括ユーティリティ
#include "util/container.hpp"
#include "util/arrays.h"

// クラスユーティリティ
#include "util/noncopyable.hpp"
#include "util/bitSet.hpp"
#include "util/bitArray.hpp"
#include "util/stack.hpp"
#include "util/biStack.hpp"
#include "util/linearlyInterpolatedTable.hpp"
#include "util/lineZone.hpp"
#include "util/accessor.hpp"
#include "util/comparableBitSet.hpp"
#include "util/index.hpp"

// atomic性のあるクラス
#include "util/atomic.hpp"

// 応用ユーティリティ
#include "util/bitPartition.hpp"

// 解析用
#include "util/math.hpp"
#include "util/fmath.hpp"

// 確率分布テーブルの扱い
#include "util/pd.hpp"

// ハッシュ
#include "hash/hashFunc.hpp"
#include "hash/hashBook.hpp"

// 多腕バンディット問題
#include "util/multiArmedBandit.hpp"

// 方策勾配法
#include "util/softmaxPolicy.hpp"

// スコアからの選択
#include "util/selection.hpp"

// 探索
#include "util/search.hpp"

// 確率込みの探索
#include "util/grid.hpp"

// 並列化
#include "util/lock.hpp"

// 置換表
#include "util/node.hpp"

// 出力
#include "util/io.hpp"

// 統計
#include "util/statistics.hpp"

#endif  // COMMON_ALL_H_
