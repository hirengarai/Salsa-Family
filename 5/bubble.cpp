#include <iostream>

using namespace std;

void bubble_sort(int *arr, size_t size)
{
    for (size_t i{size - 1}; i >= 1; i--)
    {
        bool flag = 0;
        for (int j{0}; j < i; ++j)
        {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
            flag = 1;
            if(!flag)
                break;
        }
    }
}

int main()
{
    int arr[4] = {78, 45, -9, 67};
    int arr1[9] = {1,2,3,4,5,6,7,8,9};
    bubble_sort(arr1, 4);
    for (int index{0}; index < 9; ++index)
        cout << arr1[index] << " ";
    cout << "\n";

    return 0;
}
