#include<algorithm>  // sorting purpose
#include <bitset>   // binary representation
#include <cmath>    // pow function
#include <cstring>  // string
#include <fstream>  // files
#include <iostream> // cin cout
#include <iomanip>  // decimal numbers upto certain places, setw function
#include <vector>   // vector

using namespace std;

constexpr int WORD_SIZE = numeric_limits<uint32_t>::digits;

void swap(int &a, int &b){
    a = 7;
    b = 9;
}

int main(){
    cout << WORD_SIZE << "\n";
    int a = 10, b = 11;
    swap(a, b);
    cout << "a = " << a << " b = " << b << "\n";

    auto h = __builtin_popcount(4);
    cout << h << "\n";
}