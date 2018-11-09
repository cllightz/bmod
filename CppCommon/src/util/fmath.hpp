/*
 fmath.hpp
 Katsuki Ohto
 */

#ifndef UTIL_FMATH_HPP_
#define UTIL_FMATH_HPP_

// 連続空間を扱う際の基本関数

#include <cmath>
#include <cassert>

#ifdef QUADMATH
#if defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) ) && !defined(_WIN32)
#include <quadmath.h>
#endif
#endif

/**************************座標変換**************************/

struct dXY{ double x, y; };
struct dRT{ double r, t; };

static dRT XYtoRT(double x, double y){
    dRT ret;
    ret.r = std::sqrt(x * x + y * y);
    assert(x * y != 0);
    ret.r = atan2(y, x);
    return ret;
}

static dXY RTtoXY(double r, double t)noexcept{
    dXY ret;
    ret.x = r * std::cos(t);
    ret.y = r * std::sin(t);
    return ret;
}

constexpr static uint32_t XYtoR2(uint32_t x, uint32_t y)noexcept{
    return x * x + y * y;
}

constexpr static uint32_t XYtoR2(int32_t x, int32_t y)noexcept{
    return x * x + y * y;
}

constexpr static uint32_t XYtoR2(uint64_t x, uint64_t y)noexcept{
    return x * x + y * y;
}

constexpr static uint32_t XYtoR2(int64_t x, int64_t y)noexcept{
    return x * x + y * y;
}

constexpr static float XYtoR2(float x, float y)noexcept{
    return x * x + y * y;
}
constexpr static float XYtoR(float x, float y)noexcept{
    return hypotf(x, y);
}
constexpr static float XYtoT(float x, float y)noexcept{
    return atan2f(y, x);
}
static float XYtoCosT(float x, float y){
    assert(x != 0 || y != 0);
    return x / XYtoR(x, y);
}
static float XYtoSinT(float x, float y){
    assert(x != 0 || y != 0);
    return y / XYtoR(x, y);
}

constexpr static double XYtoR2(double x, double y)noexcept{
    return x * x + y * y;
}
constexpr static double XYtoR(double x, double y)noexcept{
    return hypot(x, y);
}
constexpr static double XYtoT(double x, double y)noexcept{
    return atan2(y, x);
}
static double XYtoCosT(double x, double y){
    assert(x != 0 || y != 0);
    return x / XYtoR(x, y);
}
static double XYtoSinT(double x, double y){
    assert(x != 0 || y != 0);
    return y / XYtoR(x, y);
}

#ifdef LONGDOUBLE
constexpr static long double XYtoR2(long double x, long double y){
    return x * x + y * y;
}
constexpr static long double XYtoR(long double x, long double y){
    return hypotl(x, y);
}
constexpr static long double XYtoT(long double x, long double y){
    return atan2l(y, x);
}
static long double XYtoCosT(long double x, long double y){
    assert(x != 0 || y != 0);
    return x / XYtoR(x, y);
}
static long double XYtoSinT(long double x, long double y){
    assert(x != 0 || y != 0);
    return y / XYtoR(x, y);
}
#endif

#ifdef QUADMATH
#if defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) ) && !defined(_WIN32)
constexpr static __float128 XYtoR2(__float128 x, __float128 y)noexcept{
    return x * x + y * y;
}
static __float128 XYtoR(__float128 x, __float128 y){
    return hypotq(x, y);
}
static __float128 XYtoT(__float128 x, __float128 y){
    return atan2q(y, x);
}
static __float128 XYtoCosT(__float128 x, __float128 y){
    assert(x != 0 || y != 0);
    return x / XYtoR(x, y);
}
static __float128 XYtoSinT(__float128 x, __float128 y){
    assert(x != 0 || y != 0);
    return y / XYtoR(x, y);
}
#endif
#endif

/************************** trigonometric function **************************/

template<typename T>
static T sinAaddB(T sinA, T cosA, T sinB, T cosB)noexcept{
    return sinA * cosB + cosA * sinB;
}

template<typename T>
static T sinAsubB(T sinA, T cosA, T sinB, T cosB)noexcept{
    return sinA * cosB - cosA * sinB;
}

template<typename T>
static T cosAaddB(T sinA, T cosA, T sinB, T cosB)noexcept{
    return cosA * cosB - sinA * sinB;
}

template<typename T>
static T cosAsubB(T sinA, T cosA, T sinB, T cosB)noexcept{
    return cosA * cosB + sinA * sinB;
}

template<typename T>
struct TriAngle{
    T cos_, sin_;
    
    constexpr TriAngle(T asin_, T acos_):
    cos_(acos_), sin_(asin_){}
    
    constexpr TriAngle(T atheta):
    cos_(cos(atheta)), sin_(cos(atheta)){}
    
    constexpr TriAngle(const TriAngle& ata):
    cos_(ata.acos_), sin_(ata.asin_){}
    
    constexpr TriAngle<T>& operator+=(const TriAngle& rhs)noexcept{
        sin_ = sinAaddB<T>(sin_, cos_, rhs.sin_, rhs.cos_);
        cos_ = cosAaddB<T>(sin_, cos_, rhs.sin_, rhs.cos_);
    }
    constexpr TriAngle<T>& operator-=(const TriAngle& rhs)noexcept{
        sin_ = sinAsubB<T>(sin_, cos_, rhs.sin_, rhs.cos_);
        cos_ = cosAsubB<T>(sin_, cos_, rhs.sin_, rhs.cos_);
    }
};

template<typename T>
static constexpr TriAngle<T>& operator+(const TriAngle<T>& lhs){
    return TriAngle<T>(lhs.sin_, lhs.cos_);
}
template<typename T>
static constexpr TriAngle<T>& operator-(const TriAngle<T>& lhs){
    return TriAngle<T>(-lhs.sin_, lhs.cos_);
}
template<typename T>
static constexpr TriAngle<T> operator+(const TriAngle<T>& lhs, const TriAngle<T>& rhs){
    TriAngle<T> ret;
    ret.sin_ = sinAaddB<T>(lhs.sin_, lhs.cos_, rhs.sin_, rhs.cos_);
    ret.cos_ = cosAaddB<T>(lhs.sin_, lhs.cos_, rhs.sin_, rhs.cos_);
    return ret;
}
template<typename T>
static constexpr TriAngle<T> operator-(const TriAngle<T>& lhs, const TriAngle<T>& rhs){
    TriAngle<T> ret;
    ret.sin_ = sinAsubB<T>(lhs.sin_, lhs.cos_, rhs.sin_, rhs.cos_);
    ret.cos_ = cosAsubB<T>(lhs.sin_, lhs.cos_, rhs.sin_, rhs.cos_);
    return ret;
}
template<typename T>
static constexpr T cos(const TriAngle<T>& rhs){
    return rhs.cos_;
}
template<typename T>
static constexpr T sin(const TriAngle<T>& rhs){
    return rhs.sin_;
}
template<typename T>
static constexpr T tan(const TriAngle<T>& rhs){
    return rhs.sin_ / rhs.cos_;
}

template<typename T>
struct RotationMatrix : public std::array<std::array<T, 2>, 2>{
    
    RotationMatrix(T theta){
        (*this)[0][0] = cos(theta);
        (*this)[0][1] = -sin(theta);
        (*this)[1][0] = sin(theta);
        (*this)[1][1] = cos(theta);
    }
    
};
/*
 template<typename T>
 static constexpr std::array<std::array<T, 2>, 2> rotationMatrix(T atheta){
    return {
        {cos(atheta), -sin(atheta)},
        {sin(atheta), cos(atheta)}
    };
}*/

/************************** rotation **************************/
template<class xy0_t, class xy1_t>
static void rotate(const xy0_t& arg, const double theta, xy1_t *const dst){
    dst->setX(arg.getX() * cos(theta) - arg.getY() * sin(theta));
    dst->setY(arg.getY() * cos(theta) + arg.getX() * sin(theta));
}

template<class xy0_t, class xy1_t>
static void rotate(const xy0_t& arg, const float theta, xy1_t *const dst){
    dst->setX(arg.getX() * cosf(theta) - arg.getY() * sinf(theta));
    dst->setY(arg.getY() * cosf(theta) + arg.getX() * sinf(theta));
}

template<class xy0_t>
static void rotate(xy0_t *const dst, const double theta){
    const double ax = dst->getX();
    const double ay = dst->getY();
    dst->setX(ax * cos(theta) - ay * sin(theta));
    dst->setY(ay * cos(theta) + ax * sin(theta));
}

template<class xy0_t>
static void rotate(xy0_t *const dst, const float theta){
    const float ax = dst->getX();
    const float ay = dst->getY();
    dst->setX(ax * cos(theta) - ay * sin(theta));
    dst->setY(ay * cos(theta) + ax * sin(theta));
}

template<class xy0_t, class xy1_t>
static void rotateToAdd(const xy0_t& arg, const double theta, xy1_t *const dst){
    dst->addX(arg.getX() * cos(theta) - arg.getY() * sin(theta));
    dst->addY(arg.getY() * cos(theta) + arg.getX() * sin(theta));
}

template<class xy0_t, class xy1_t>
static void rotateToAdd(const xy0_t& arg, const float theta, xy1_t *const dst){
    dst->addX(arg.getX() * cosf(theta) - arg.getY() * sinf(theta));
    dst->addY(arg.getY() * cosf(theta) + arg.getX() * sinf(theta));
}

static void rotate(const float ox, const float oy, const float theta, float *const x, float *const y){
    *x = ox * cosf(theta) - oy * sinf(theta);
    *y = oy * cosf(theta) + ox * sinf(theta);
}

static void rotateToAdd(const float ox, const float oy, const float theta, float *const x, float *const y){
    *x += ox * cosf(theta) - oy * sinf(theta);
    *y += oy * cosf(theta) + ox * sinf(theta);
}

static void rotate(const double ox, const double oy, const double theta, double *const x, double *const y){
    *x = ox * cos(theta) - oy * sin(theta);
    *y = oy * cos(theta) + ox * sin(theta);
}

static void rotateToAdd(const double ox, const double oy, const double theta, double *const x, double *const y){
    *x += ox * cos(theta) - oy * sin(theta);
    *y += oy * cos(theta) + ox * sin(theta);
}

#ifdef LONGDOUBLE
static void rotate(const long double ox, const long double oy, const long double theta, long double *const x, long double *const y){
    *x = ox * cosl(theta) - oy * sinl(theta);
    *y = oy * cosl(theta) + ox * sinl(theta);
}

static void rotateToAdd(const long double ox, const long double oy, const long double theta, long double *const x, long double *const y){
    *x += ox * cosl(theta) - oy * sinl(theta);
    *y += oy * cosl(theta) + ox * sinl(theta);
}
#endif

#ifdef QUADMATH
#if defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) ) && !defined(_WIN32)
static void rotate(const __float128 ox, const __float128 oy, const __float128 theta, __float128 *const x, __float128 *const y){
    *x = ox * cosq(theta) - oy * sinq(theta);
    *y = oy * cosq(theta) + ox * sinq(theta);
}

static void rotateToAdd(const __float128 ox, const __float128 oy, const __float128 theta, __float128 *const x, __float128 *const y){
    *x += ox * cosq(theta) - oy * sinq(theta);
    *y += oy * cosq(theta) + ox * sinq(theta);
}
#endif
#endif


/************************** rotation by trigonometric function **************************/

template<typename T>
static void rotate(const T ox, const T oy, const T sinA, const T cosA, T *const x, T *const y){
    *x = ox * cosA - oy * sinA;
    *y = oy * cosA + ox * sinA;
}
template<typename T>
static void rotateToAdd(const T ox, const T oy, const T sinA, const T cosA, T *const x, T *const y){
    *x += ox * cosA - oy * sinA;
    *y += oy * cosA + ox * sinA;
}

#endif // UTIL_FMATH_HPP_




