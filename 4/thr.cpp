#include <iostream>
#include <cmath>

int arr[10]{0};

void threadFunc(int sum)
{
    uint64_t loop = pow(2, 15);
    for (int i{0}; i < loop; ++i)
    {
        sum += arr[i];
        if (sum & 1)
            arr[i] = i;
    }
}