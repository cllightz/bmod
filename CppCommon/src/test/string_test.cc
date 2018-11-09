#include "../util/string.hpp"

using namespace std;

int main(){

    ostringstream oss0;
    oss0 << "AA" << endl << "B" << endl;
    string s0 = oss0.str();
    
    ostringstream oss1;
    oss1 << "aaa" << endl << "bb" << endl << "c" << endl;
    string s1 = oss1.str();
    
    ostringstream oss2;
    oss2 << "AA" << endl;
    oss2 << "B" << endl;
    string s2 = oss2.str();
    
    cout << lineUp(s0, s1) << endl
    << lineUp(s0, s1, 1) << endl
    << packLineUp(s0, s1) << endl
    << packLineUp(s0, s1, 1) << endl;
    
    cout << packLineUp(s2, s2, 1) << endl;
    
    return 0;
}