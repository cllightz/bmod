//実験

#include <cstring>
#include <unistd.h>
#include <sys/time.h>
#include <ctime>

#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <random>
#include <algorithm>
#include <string>

#include "../defines.h"
#include "../util/search.hpp"
#include "../util/multiArmedBandit.hpp"

using namespace std;

int main(){

    Clock cl;
    uint64_t time;
    
    //y = pow(x, 3) -0.5 << x << 1.5
    //y = 0
    {
        auto f = [](double x)->double{ return x * x * x; };
        
        cerr << "y = pow(x, 3) , y = 0 ... how is x?" << endl;
        
        cerr << "BiSolver = ";
        
        cl.start();
        BiSolver bi(-0.5, +1.5);
        for(int i=0; i<10000; ++i){
            auto x = bi.play();
            bool res = (f(x) >= 0);
            bi.feed(res);
        }
        time = cl.stop();
        cerr << bi.answer() << " in " << time << " clock ( " << 10000 << " trials )" <<  endl;
        
        cerr << "MCTSSolver = ";
        cl.start();
        array<array<double, 2>, 1> var = {{-0.5, +1.5}};
        array<double, 2> rew = {-pow(1.5, 3), 0};
        
        MCTSSolver<1> mcts(var, rew);
        for(int i=0; i<100000; ++i){
            auto x = mcts.play();
            double res = -fabs(f(x[0]) - 0);
            mcts.feed(x, res);
        }
        time = cl.stop();
        cerr << mcts.answer()[0] << " in " << time << " clock ( " << 100000 << " trials )" <<  endl;
        cerr << mcts.toInfoString() << endl;
    }
    return 0;
}