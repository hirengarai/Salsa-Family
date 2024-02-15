#include<iostream>

using namespace std;


void function (int *a){
    cout << a[0] << a[1] << a[2] << a[3] + 1 << "\n";
}

int main(){

    int arr[4] = {0, 5, 2, 3};
    int *barr;
    int(&carr)[4] = arr;
    barr = arr;

    // cout << barr[0] << barr[1] << barr[2] << barr[3] << "\n";
    // cout << carr[0] << carr[1] << carr[2] << carr[3] << "\n";

    function(arr);
}