/*
 random.hpp
 Katsuki Ohto
 */

#ifndef UTIL_RANDOM_HPP_
#define UTIL_RANDOM_HPP_

#include <random>
#include <cmath>
#include <array>
#include <iostream>
#include <sstream>

#include "../defines.h"
#include "math.hpp"

// 確率分布いろいろ

// mean() ... 平均値
// med() ... 中央値
// mod() ... 最頻値
// var() ... 分散
// std() ...　標準偏差
// cov(i, j) ... 共分散
// skew() ... 歪度
// kur() ... 尖度

// dens(x) ... 確率密度
// relative_dens(x) ... 相対確率密度(計算量削減)

// dist(x) ... 累積分布関数

// ent ... エントロピー

// rand(pdice) ... 確率密度に従う乱数発生

/**************************指数分布**************************/

struct ExponentialDistribution{
    double lambda;
    
    double rand(double urand)const{
        return -std::log(urand) / lambda;
    }
    
    template<class dice_t>
    double rand(dice_t *const pdice)const{
        return rand(pdice->drand());
    }
    
    double relative_dens(double x)const noexcept{
        return exp(-lambda * x);
    }
    double dens(double x)const noexcept{
        return lambda * relative_dens(x);
    }
    double dist(double x)const noexcept{
        return 1 - exp(-lambda * x);
    }
    double mean()const{ return 1 / lambda; }
    double med()const{ return log((double)2) / lambda; }
    double var()const{ return 1 / (lambda * lambda); }
    double std()const{ return 1 / lambda; }
    double ent()const{ return 1 - log(lambda); }
    
    static constexpr double mod()noexcept{ return 0; }
    static constexpr double skew()noexcept{ return 2; }
    static constexpr double kur()noexcept{ return 6; }
    
    ExponentialDistribution& set(double l)noexcept{
        lambda = l;
        return *this;
    }
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "Exp(" << lambda << ")";
        return oss.str();
    }
    
    constexpr ExponentialDistribution():
    lambda(){}
    explicit constexpr ExponentialDistribution(double l):
    lambda(l){}
};

std::ostream& operator<<(std::ostream& out, const ExponentialDistribution& e){
    out << e.toString();
    return out;
}

/**************************ガンマ分布**************************/

struct GammaDistribution{
    double k;
    double theta;
    
    template<class dice_t>
    double rand(dice_t *const pdice)const{
        double x, y, z;
        double u, v, w, b, c, e;
        if(k < 1){
            ExponentialDistribution ex(1);
            e = ex.rand(pdice);
            do{
                x = pow(pdice->drand(), 1 / k);
                y = pow(pdice->drand(), 1 / (1 - k));
            }while(x + y > 1);
            return (e * x / (x + y)) * theta;
        }else{
            b = k - 1;
            c = 3 * k - 0.75;
            while(true){
                u = pdice->drand();
                v = pdice->drand();
                w = u * (1 - u);
                y = sqrt(c / w) * (u - 0.5);
                x = b + y;
                if(x >= 0){
                    z = 64 * w * w * w * v * v;
                    if(z <= 1 - (2 * y * y) / x){
                        return x * theta;
                    }else{
                        if(log(z) < 2 * (b * log(x / b) - y)){
                            return x * theta;
                        }
                    }
                }
            }
            return x * theta;
        }
    }
    
    GammaDistribution& set(double ak, double atheta = 1)noexcept{
        k = ak;
        theta = atheta;
        return *this;
    }
    
    double ralative_dens(double x)const{
        return pow(x, k - 1) * exp(-x / theta);
    }
    double dens(double x)const{
        return ralative_dens(x) / tgamma(k) / pow(theta, k);
    }
    /*double dist(double x)const{
        return 1 - igamma(k, x / theta) / tgamma(k);
    }*/
    
    constexpr double mean()const noexcept{
        return k * theta;
    }
    constexpr double mod()const noexcept{
        return (k - 1) * theta;
    }
    constexpr double var()const noexcept{
        return k * theta * theta;
    }
    double std()const noexcept{
        return sqrt(k) * theta;
    }
    double med()const;
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "G(" << k << ", " << theta << ")";
        return oss.str();
    }
    
    constexpr GammaDistribution():
    k(), theta(){}
    explicit constexpr GammaDistribution(double ak):
    k(ak), theta(1){}
    explicit constexpr GammaDistribution(double ak, double atheta):
    k(ak), theta(atheta){}
};

std::ostream& operator<<(std::ostream& out, const GammaDistribution& g){
    out << g.toString();
    return out;
}

/**************************逆ガンマ分布**************************/

struct InverseGammaDistribution{
    double k;
    static constexpr double rate = 1.0;
    
    InverseGammaDistribution& set(double ak)noexcept{
        k = ak;
        return *this;
    }
    
    double relative_dens(double x)const{
        return pow(x, -k - 1) * exp(-rate / x);
    }
    double dens(double x)const{
        return pow(rate, k) / tgamma(k) * relative_dens(x);
    }
    /*double dist(double x)const{
        return igamma(k, rate / x) / tgamma(k);
    }*/
    
    constexpr double mean()const noexcept{
        return rate / (k - 1); // for k > 1
    }
    constexpr double mod()const noexcept{
        return rate / (k + 1);
    }
    constexpr double var()const noexcept{
        return rate * rate / (k - 1) / (k - 1) / (k - 2); // for k > 2
    }
    double std()const noexcept{
        return rate / (k - 1) / sqrt(k - 2);
    }
    double med()const;
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "IG(" << k << ", " << rate << ")";
        return oss.str();
    }
    
    constexpr InverseGammaDistribution():
    k(){}
    explicit constexpr InverseGammaDistribution(double ak):
    k(ak){}
};

std::ostream& operator<<(std::ostream& out, const InverseGammaDistribution& ig){
    out << ig.toString();
    return out;
}

/**************************ガンマ分布, 逆ガンマ分布の中央値**************************/

double GammaDistribution::med()const{ return InverseGammaDistribution(k).dens(1 / 2.0); }
double InverseGammaDistribution::med()const{ return GammaDistribution(k).dens(1 / 2.0); }

/**************************ベータ分布**************************/

struct BetaDistribution{
    double a, b;
    
    template<class dice_t>
    double rand(dice_t *const pdice)const{
        double r1 = GammaDistribution(a).rand(pdice);
        double r2 = GammaDistribution(b).rand(pdice);
        return r1 / (r1 + r2);
    }
    constexpr double size()const noexcept{
        return a + b;
    }
    constexpr double mean()const{
        return a / (a + b);
    }
    constexpr double rate()const{
        return a / b;
    }
    constexpr double med()const{
        return (a - 1 / 3.0) / (a + b - 2 / 3.0); // a, b > 1 での近似値
    }
    constexpr double mod()const{
        return (a - 1) / (a + b - 2); // a, b > 1 のとき
    }
    double var()const{
        double sum = a + b;
        return (a * b) / (sum * sum * (sum + 1.0));
    }
    double std()const{
        double sum = a + b;
        return sqrt(a * b / (sum + 1.0)) / sum;
    }
    double relative_dens(double x)const{
        return pow(x, a - 1) * pow(1 - x, b - 1);
    }
    double dens(double x)const{
        return relative_dens(x) / beta(a, b);
    }
    double log_relative_dens(double x)const{
        return (a - 1) * log(x) + (b - 1) * log(1 - x);
    }
    double log_dens(double x)const{
        DERR << log_beta(a, b) << endl;
        return log_relative_dens(x) - log_beta(a, b);
    }
    
    BetaDistribution& add(const BetaDistribution& arg)noexcept{
        a += arg.a;
        b += arg.b;
        return *this;
    }
    BetaDistribution& subtr(const BetaDistribution& arg)noexcept{
        a -= arg.a;
        b -= arg.b;
        return *this;
    }
    
    BetaDistribution& operator+=(const BetaDistribution& rhs)noexcept{
        a += rhs.a;
        b += rhs.b;
        return *this;
    }
    BetaDistribution& operator-=(const BetaDistribution& rhs)noexcept{
        a -= rhs.a;
        b -= rhs.b;
        return *this;
    }
    BetaDistribution& operator*=(const double m)noexcept{
        a *= m;
        b *= m;
        return *this;
    }
    BetaDistribution& operator/=(const double d){
        (*this) *= 1 / d;
        return *this;
    }
    BetaDistribution& mul(const double m)noexcept{
        a *= m;
        b *= m;
        return *this;
    }
    
    BetaDistribution& rev()noexcept{
        std::swap(a, b);
        return *this;
    }
    
    BetaDistribution& set(const double aa, const double ab)noexcept{
        a = aa;
        b = ab;
        return *this;
    }
    BetaDistribution& set_by_mean(double m, double size)noexcept{
        set(m * size, (1 - m) * size);
        return *this;
    }
    BetaDistribution& set_by_rate(double r, double size)noexcept{
        set_by_mean(r / (1 + r), size);
        return *this;
    }
    
    BetaDistribution& resize(double h){
        // サイズをhにする
        double s = size();
        
        assert(s);
        
        double h_s = h / s;
        a *= h_s;
        b *= h_s;
        return *this;
    }
    
    bool exam()const noexcept{
        if(a < 0 || b < 0){ return false; }
        if(a == 0 && b == 0){ return false; }
        return true;
    }
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "Be(" << a << ", " << b << ")";
        return oss.str();
    }
    
    constexpr BetaDistribution():
    a(), b(){}
    explicit constexpr BetaDistribution(const double aa, const double ab):
    a(aa), b(ab){}
};

BetaDistribution operator+(const BetaDistribution& lhs, const BetaDistribution& rhs)noexcept{
    return BetaDistribution(lhs.a + rhs.a, lhs.b + rhs.b);
}
BetaDistribution operator-(const BetaDistribution& lhs, const BetaDistribution& rhs)noexcept{
    return BetaDistribution(lhs.a - rhs.a, lhs.b - rhs.b);
}
BetaDistribution operator*(const BetaDistribution& lhs, const double m)noexcept{
    return BetaDistribution(lhs.a * m, lhs.b * m);
}
BetaDistribution operator*(const double m, const BetaDistribution& rhs)noexcept{
    return BetaDistribution(rhs.a * m, rhs.b * m);
}

std::ostream& operator<<(std::ostream& out, const BetaDistribution& b){
    out << b.toString();
    return out;
}

/**************************ディリクレ分布**************************/

// 各確率変数が報酬を持つときに，ランダムに報酬を返すことも可能とする

template<std::size_t N>
struct DirichletDistribution{
    
    std::array<double, N> a;
    double sum;
    
    static constexpr std::size_t dim()noexcept{ return N; }
    
    template<class dice_t, typename reward_t>
    double rand(const reward_t reward[], dice_t *const pdice)const{
        double sum = 0.0;
        reward_t sum_rew = 0.0;
        for(auto i = 0; i < N ; ++i){
            double r = GammaDistribution(a[i]).rand(pdice);
            sum += r;
            sum_rew += reward[i] * r;
        }
        sum_rew /= sum;
        return sum_rew;
    }
    
    template<class dice_t>
    std::array<double, N> rand(dice_t *const pdice)const{
        std::array<double, N> ans;
        double sum = 0;
        for(auto i = 0; i < N ; ++i){
            double r = GammaDistribution(a[i]).rand(pdice);
            ans[i] = r;
            sum += r;
        }
        double _sum = 1 / sum;
        for(auto i = 0; i < N ; ++i){
            ans[i] *= _sum;
        }
        return ans;
    }
    
    double size()const noexcept{
        return sum;
    }
    double mean(int i)const{
        return a[i] / sum;
    }
    std::array<double, N> mean()const{
        std::array<double, N> ans;
        for(auto i = 0; i < N ; ++i){
            ans[i] = mean(i);
        }
        return ans;
    }
    
    double var(int i)const{
        return a[i] * (sum - a[i]) / (sum * sum * (sum + 1.0));
    }
    double std(int i)const{
        return sqrt(var(i));
    }
    std::array<double, N> var()const{
        std::array<double, N> ans;
        for(auto i = 0; i < N ; ++i){
            ans[i] = var(i);
        }
        return ans;
    }
    
    double cov(int i, int j)const{
        return - a[i] * a[j] / (sum * sum * (sum + 1.0));
    }
    std::array<std::array<double, N>, N> cov()const{
        std::array<std::array<double, N>, N> ans;
        for(auto i = 0; i < N ; ++i){
            for(auto j = 0; j < N; ++j){
                ans[i][j] = cov(i, j);
            }
        }
        return ans;
    }
    
    double relative_dens(const std::array<double, N>& x)const{
        double m = 1;
        for(auto i = 0; i < N ; ++i){
            m *= pow(x[i], a[i] - 1);
        }
        return m;
    }
    double dens(const std::array<double, N>& x)const{
        return relative_dens(x) / multivariate_beta(a, sum);
    }
    double log_relative_dens(const std::array<double, N>& x)const{
        double r = 0;
        for(auto i = 0; i < N ; ++i){
            r += (a[i] - 1) * log(x[i]);
        }
        return r;
    }
    double log_dens(const std::array<double, N>& x)const{
        return log_relative_dens(x) - log_multivariate_beta(a, sum);
    }
    
    DirichletDistribution& set(std::size_t i, double v){
        ASSERT(i < N, cerr << i << " in " << N << endl;);
        sum -= a[i];
        a[i] = v;
        sum += v;
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    DirichletDistribution& add(std::size_t i, double v){
        ASSERT(i < N, cerr << i << " in " << N << endl;);
        a[i] += v;
        sum += v;
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    DirichletDistribution& subtr(std::size_t i, double v){
        ASSERT(i < N, cerr << i << " in " << N << endl;);
        a[i] -= v;
        sum -= v;
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    
    DirichletDistribution& operator+=(const DirichletDistribution& rhs)noexcept{
        for(auto i = 0; i < N; ++i){
            a[i] += rhs.a[i];
            sum += rhs.a[i];
        }
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    DirichletDistribution& operator-=(const DirichletDistribution& rhs)noexcept{
        for(auto i = 0; i < N; ++i){
            a[i] -= rhs.a[i];
            sum -= rhs.a[i];
        }
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    DirichletDistribution& operator*=(const double m)noexcept{
        for(auto i = 0; i < N; ++i){
            a[i] *= m;
        }
        sum *= m;
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    DirichletDistribution& operator/=(const double d){
        (*this) *= 1 / d;
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    
    DirichletDistribution& set(const double aa[]){
        sum = 0;
        for(auto i = 0; i < N; ++i){
            double t = aa[i];
            a[i] = t;
            sum += t;
        }
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    DirichletDistribution& set(const std::array<double, N>& aa){
        sum = 0;
        for(auto i = 0; i < N; ++i){
            double t = aa[i];
            a[i] = t;
            sum += t;
        }
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    DirichletDistribution& fill(const double t){
        for(auto i = 0; i < N; ++i){
            a[i] = t;
        }
        sum = t * N;
        ASSERT(exam(), std::cerr << toDebugString() << std::endl;);
        return *this;
    }
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "Di(";
        for(auto i = 0; i < N - 1; ++i){
            oss << a[i] << ", ";
        }
        if(N > 0){
            oss << a[N - 1];
        }
        oss << ")";
        return oss.str();
    }
    std::string toDebugString()const{
        std::ostringstream oss;
        oss << toString() << " sum = " << sum;
        return oss.str();
    }
    
    DirichletDistribution(){
        std::array<double, N> tmp;
        tmp.fill(0);
        set(tmp);
    }
    DirichletDistribution(const double aa[]){
        set(aa);
    }
    DirichletDistribution(const std::array<double, N>& aa){
        set(aa);
    }
    
    bool exam()const noexcept{
        double tsum = 0;
        for(double v : a){
            tsum += v;
        }
        if(fabs(sum - tsum) > 0.0000001 * sum){ return false; }
        return true;
    }
};

template<std::size_t N>
std::ostream& operator<<(std::ostream& out, const DirichletDistribution<N>& d){
    out << d.toString();
    return out;
}

/**************************正規分布**************************/

template<typename float_t = double>
struct NormalDistribution{
    float_t mu, sigma;
    
    template<class dice_t>
    float_t rand(dice_t *const pdice)const{
        // Box-Muller
        float_t r1 = pdice->drand();
        float_t r2 = pdice->drand();
        float_t z1 = std::sqrt(-2.0 * std::log(r1)) * std::cos(2.0 * M_PI * r2);
        return z1 * sigma + mu;
    }
    template<class dice_t>
    void rand(float_t *const pa, float_t *const pb, dice_t *const pdice)const{
        // 2つ同時に発生させる
        float_t r1 = pdice->drand();
        float_t r2 = pdice->drand();
        
        float_t z1 = std::sqrt(-2.0 * std::log(r1)) * std::cos(2.0 * M_PI * r2);
        float_t z2 = std::sqrt(-2.0 * std::log(r1)) * std::sin(2.0 * M_PI * r2);
        *pa = z1 * sigma + mu;
        *pb = z2 * sigma + mu;
    }
    
    float_t relative_dens(float_t x)const{
        return exp(-(x - mu) * (x - mu) / (2 * sigma * sigma));
    }
    float_t dens(float_t x)const{
        return relative_dens(x) / sigma * (1 / sqrt(2 * M_PI));
    }
    float_t dist(float_t x)const{
        return (1 + erf((x - mu) / sigma * (1 / sqrt((double)2)))) / 2;
    }
    
    float_t ent()const{
        return sigma * sqrt(2 * M_PI * exp((double)1));
    }
    constexpr float_t mean()const noexcept{ return mu; }
    constexpr float_t var()const noexcept{ return sigma * sigma; }
    constexpr float_t std()const noexcept{ return sigma; }
    constexpr float_t med()const noexcept{ return mu; }
    constexpr float_t mod()const noexcept{ return mu; }
    
    NormalDistribution<float_t>& operator*=(const double m)noexcept{
        sigma *= m;
        return *this;
    }
    NormalDistribution<float_t>& operator/=(const double m){
        sigma /= m;
        return *this;
    }
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "N(" << mu << ", " << sigma << ")";
        return oss.str();
    }
    
    constexpr NormalDistribution():mu(),sigma(){}
    constexpr NormalDistribution(float_t argMu, float_t argSigma)
    :mu(argMu), sigma(argSigma){}
    
    NormalDistribution& set(float_t argMu, float_t argSigma)noexcept{
        mu = argMu;
        sigma = argSigma;
        return *this;
    }
};

template<class float_t>
std::ostream& operator<<(std::ostream& out, const NormalDistribution<float_t>& n){
    out << n.toString();
    return out;
}

/**************************対数正規分布**************************/

template<typename float_t = double>
struct LogNormalDistribution{
    float_t mu, sigma;
    
    template<class dice_t>
    float_t rand(dice_t *const pdice)const{
        return exp(NormalDistribution<float_t>(mu, sigma).rand(pdice));
    }
    template<class dice_t>
    void rand(float_t *const pa, float_t *const pb, dice_t *const pdice)const{
        // 2つ同時に発生させる
        float_t ta, tb;
        NormalDistribution<float_t>(mu, sigma).rand(&ta, &tb, pdice);
        *pa = exp(ta);
        *pb = exp(tb);
    }
    
    float_t relative_dens(float_t x)const{
        return exp(-pow(log(x) - mu, 2) / (2 * sigma * sigma)) / x;
    }
    float_t dens(float_t x)const{
        return relative_dens(x) / sigma * (1 / sqrt(2 * M_PI));
    }
    float_t dist(float_t x)const{
        return (1 - erf(-(log(x) - mu) / sigma * (1 / sqrt((double)2)))) / 2;
    }
    
    float_t ent()const{
        return 1 / 2.0 + log(2 * M_PI * sigma * sigma) / 2 + mu;
    }
    
    float_t mean()const noexcept{ return exp(mu + sigma * sigma / 2); }
    float_t var()const noexcept{ return exp(2 * mu + sigma * sigma) * (exp(sigma * sigma) - 1); }
    float_t std()const noexcept{ return sqrt(var()); }
    float_t med()const noexcept{ return exp(mu); }
    float_t mod()const noexcept{ return exp(mu - sigma * sigma); }
    
    float_t skew()const noexcept{
        return sqrt(exp(sigma * sigma) - 1) * (exp(sigma * sigma) + 2);
    }
    float_t kur()const noexcept{
        return exp(4 * sigma * sigma) + 2 * exp(3 * sigma * sigma) + 3 * exp(2 * sigma * sigma) - 6;
    }
    
    LogNormalDistribution<float_t>& operator*=(const double m)noexcept{
        sigma *= m;
        return *this;
    }
    LogNormalDistribution<float_t>& operator/=(const double m){
        sigma /= m;
        return *this;
    }
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "LN(" << mu << ", " << sigma << ")";
        return oss.str();
    }
    
    constexpr LogNormalDistribution():mu(),sigma(){}
    constexpr LogNormalDistribution(float_t argMu, float_t argSigma)
    :mu(argMu), sigma(argSigma){}
    
    LogNormalDistribution& set(float_t argMu, float_t argSigma)noexcept{
        mu = argMu;
        sigma = argSigma;
        return *this;
    }
};

template<class float_t>
std::ostream& operator<<(std::ostream& out, const LogNormalDistribution<float_t>& n){
    out << n.toString();
    return out;
}

/**************************ワイブル分布**************************/

template<typename float_t = double>
struct WeibullDistribution{
    float_t m, eta;
    
    float_t rand(float_t urand)const{
        return -eta * pow(log(urand), 1 / m);
    }
    template<class dice_t>
    float_t rand(dice_t *const pdice)const{
        return rand(pdice->drand());
    }
    
    float_t relative_dens(float_t t)const{
        return pow(t, m - 1) * exp(-pow(t / eta, m));
    }
    float_t dens(float_t t)const{
        return relative_dens(t) * m / pow(eta, m);
    }
    float_t dist(float_t t)const{
        return 1 - exp(-pow(t / eta, m));
    }
    
    float_t mu_dash(float_t r)const{
        // 平均, 分散等を計算するための関数
        return pow(eta, r) * tgamma(1 + r / m);
    }
    
    float_t mean()const{ return mu_dash(1); }
    float_t var()const{ return mu_dash(2) - m * m; }
    float_t std()const{ return sqrt(var()); }
    float_t med()const{ return eta * pow(log((double)2), 1 / m); }
    float_t mod()const{ return eta * pow(1 - 1 / m, 1 / m); }
    
    float_t skew()const{
        float_t me = mean();
        float_t v = var();
        return (mu_dash(3) - 3 * me * v - me * me * me) / pow(v, 3 / 2.0);
    }
    float_t kur()const{
        float_t me = mean();
        float_t v = var();
        return (mu_dash(4) - 4 * skew() * pow(v, 3 / 2.0) * me - 6 * me * me * v - me * me * me * me) / (v * v) - 3;
    }
    
    WeibullDistribution<float_t>& operator*=(const double r)noexcept{
        m *= r;
        return *this;
    }
    WeibullDistribution<float_t>& operator/=(const double r){
        m /= r;
        return *this;
    }
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "WB(" << m << ", " << eta << ")";
        return oss.str();
    }
    
    constexpr WeibullDistribution():m(),eta(){}
    constexpr WeibullDistribution(float_t am, float_t aeta)
    :m(am), eta(aeta){}
    
    WeibullDistribution& set(float_t am, float_t aeta)noexcept{
        m = am;
        eta = aeta;
        return *this;
    }
};

template<class float_t>
std::ostream& operator<<(std::ostream& out, const WeibullDistribution<float_t>& w){
    out << w.toString();
    return out;
}

/**************************カイ2乗分布**************************/

template<typename float_t = double>
struct ChiSquaredDistribution{
    float_t k;

    template<class dice_t>
    float_t rand(dice_t *const pdice)const{
        GammaDistribution gm(k / 2, 2);
        return gm.rand(pdice);
    }
    
    float_t relative_dens(float_t x)const{
        return pow(x, k / 2 - 1) * exp(-x / 2);
    }
    float_t dens(float_t x)const{
        return relative_dens(x) / pow(2, k / 2) / tgamma(k / 2);
    }
    /*
     TODO:
     float_t dist(float_t x)const{
     return igamma(k / 2, x / 2) / tgamma(k / 2);
     }*/
    
    constexpr float_t mean()const noexcept{ return k; }
    constexpr float_t var()const noexcept{ return 2 * k; }
    constexpr float_t std()const noexcept{ return sqrt(var()); }
    constexpr float_t med()const noexcept{
        return k - (2 / 3) + (4 / 27 / k) - (8 / 729 / k / k); // approximation
    }
    constexpr float_t mod()const noexcept{ return (k <= 2) ? 0 : (k - 2); }
    
    constexpr float_t skew()const noexcept{ return 2 * sqrt((double)2) / sqrt(k); }
    constexpr float_t kur()const noexcept{ 12 / k; }
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "CS(" << k << ")";
        return oss.str();
    }
    
    constexpr ChiSquaredDistribution():k(){}
    constexpr ChiSquaredDistribution(float_t ak):k(ak){}
    
    ChiSquaredDistribution& set(float_t ak)noexcept{
        k = ak;
        return *this;
    }
};

template<class float_t>
std::ostream& operator<<(std::ostream& out, const ChiSquaredDistribution<float_t>& cs){
    out << cs.toString();
    return out;
}

/**************************T分布**************************/

template<typename float_t = double>
struct TDistribution{
    float_t nu;
    
    template<class dice_t>
    float_t rand(dice_t *const pdice)const{
        NormalDistribution<float_t> n(0, 1);
        ChiSquaredDistribution<float_t> cs(nu);
        return n.rand(pdice) / sqrt(cs.rand(pdice) / nu);
    }
    
    float_t relative_dens(float_t x)const{
        return pow(1 + x * x / nu, -(nu + 1) / 2);
    }
    float_t dens(float_t x)const{
        return relative_dens(x) * tgamma((nu + 1) / 2) / sqrt(nu * M_PI) / tgamma(nu / 2);
    }
    /*
     TODO:
     float_t dist(float_t x)const{
        return (1 / 2) + x * tgamma((nu + 1) / 2) * ;
    }*/
    
    constexpr float_t mean()const noexcept{ return 0; }
    constexpr float_t var()const noexcept{ return nu / (nu - 2); } // for nu > 2
    constexpr float_t std()const noexcept{ return sqrt(var()); }
    constexpr float_t med()const noexcept{ return 0; }
    constexpr float_t mod()const noexcept{ return 0; }
    
    constexpr float_t skew()const noexcept{ return 0; } // for nu > 3
    constexpr float_t kur()const noexcept{ 6 / (nu - 4); } // for nu > 4
    
    std::string toString()const{
        std::ostringstream oss;
        oss << "T(" << nu << ")";
        return oss.str();
    }
    
    constexpr TDistribution():nu(){}
    constexpr TDistribution(float_t anu):nu(anu){}
    
    TDistribution& set(float_t anu)noexcept{
        nu = anu;
        return *this;
    }
};

template<class float_t>
std::ostream& operator<<(std::ostream& out, const TDistribution<float_t>& t){
    out << t.toString();
    return out;
}

#endif // UTIL_RANDOM_HPP_