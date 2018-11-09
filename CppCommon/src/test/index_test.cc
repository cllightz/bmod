#include <iostream>
#include "../util/index.hpp"

using namespace std;

int main(){

    TensorIndex<3> ti(2, 4, 8);
    cout << ti.get(1, 2, 3) << endl;
    
    cout << TensorIndexType<2, 4, 8>::get(1, 2, 3) << endl;
    
    return 0;
}