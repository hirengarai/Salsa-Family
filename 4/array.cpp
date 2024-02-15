#include<iostream>

int arr[5];
int main()
{
    arr[4] = 9;
    arr[5] = 10;

    std::cout << arr[6] << "\n";
}