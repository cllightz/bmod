/*
atomic.hpp
Katsuki Ohto
*/

#ifndef UTIL_ATOMIC_HPP_
#define UTIL_ATOMIC_HPP_

// データ構造のatomic版

#include <iostream>
#include <atomic>

#include "../defines.h"

#include "bitArray.hpp"
#include "bitSet.hpp"

template<int N = 2, int SIZE = 8 / N>
using AtomicBitArray8 = BitArrayInRegister<uint8_t, 8, N, SIZE, std::atomic<uint8_t>>;

template<int N = 4, int SIZE = 16 / N>
using AtomicBitArray16 = BitArrayInRegister<uint16_t, 16, N, SIZE, std::atomic<uint16_t>>;

template<int N = 4, int SIZE = 32 / N>
using AtomicBitArray32 = BitArrayInRegister<uint32_t, 32, N, SIZE, std::atomic<uint32_t>>;

template<int N = 4, int SIZE = 64 / N>
using AtomicBitArray64 = BitArrayInRegister<uint64_t, 64, N, SIZE, std::atomic<uint64_t>>;

using AtomicBitSet8 = BitSetInRegister<uint8_t, 8, std::atomic<uint8_t>>;
using AtomicBitSet16 = BitSetInRegister<uint16_t, 16, std::atomic<uint16_t>>;
using AtomicBitSet32 = BitSetInRegister<uint32_t, 32, std::atomic<uint32_t>>;
using AtomicBitSet64 = BitSetInRegister<uint64_t, 64, std::atomic<uint64_t>>;

#endif // UTIL_ATOMIC_HPP_