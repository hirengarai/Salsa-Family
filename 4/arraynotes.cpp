// #include <iostream>
// #include <algorithm>

// // this is a way of updating an array:// it is always a good practice to include the size of the array as an argument
// void fun(int *arr, size_t size)
// {
//     for (int i{0}; i < size; ++i)
//         arr[i]++;
// }

// void fun2(int &i)
// {
//     i++;
// }

// // How to return an array or modify an array:
// // fun is an way to modify an array
// int *arrayreturn(int n) // return type- address of integer array
// {
//     int *temp = new int[10]; // You have to dynamically allocate memory (heap), not a local array (stack)
//     int i{0};
//     while (n)
//     {
//         temp[i++] = n & 1;
//         n >>= 1;
//     }
//     return temp;
// }

// int main()
// {
//     int *onedarray; // this variable is solely for storing an address worth of integer type variable
//     int onedarray1[] = {0, 1, 2, 3, 4};
//     size_t size = sizeof(onedarray1) / sizeof(onedarray1[0]);

//     // ---------------- way 1 ---------------------------------------
//     fun(onedarray1, sizeof(onedarray1) / sizeof(onedarray1[0]));

//     // ---------------- way 2 ---------------------------------------
//     std::for_each(onedarray1, onedarray1 + size, fun2);

//     std::cout
//         << onedarray1[0] << onedarray1[1] << onedarray1[2] << onedarray1[3] << onedarray1[4] << "\n";

//     //----------------------------------------------------------------------------------------------------
//     // int temp1[8]; ❌ as this creates an array temp1 and *temp1 means the first element of the array not the address of the first element
//     std::cout
//         << *onedarray1 << *onedarray1 + 1 << *onedarray1 + 2 << *onedarray1 + 3 << *onedarray1 + 4 << "\n";
//     int *temp1 = arrayreturn(7); // int* variablename basically declares an integer pointer, which can store an array address too.
//     std::cout
//         << temp1[0] << temp1[1] << temp1[2] << temp1[3] << temp1[4] << "\n";
// }

// https://www.digitalocean.com/community/tutorials/return-array-in-c-plus-plus-function
//=========================================================================================================================================
//Returning an array way 0
#include <iostream>
using namespace std;

// pass by pointer
void func(int *temp, size_t size) // return type- void, we are directly accessing the memory address of the array
{
    for (int i = 0; i < size; i++)
        temp[i] = i;
}

// pass by reference, here the size of the array is fixed in the argument
void func2(int (&arr)[5])
{
    // Your code here
}

int main()
{
    // int arr[5]; // int * arr; ❌ You can not declare int * arr and then compile, it will give seg fault.

    int *arr = new int[5]; //✅ This is correct

    func(arr,5);
    cout << "The Array is : ";
    for (int i = 0; i < 5; i++)
        cout << arr[i] << "\t";
    cout << "\n";

    delete[] arr;

    return 0;
}
//=========================================================================================================================================

//=========================================================================================================================================
// Returning an array way 1
// #include <iostream>
// using namespace std;

// int *func() // return type- address of integer array
// {
//     int *temp = new int[5]; // array declared, You have to dynamically allocate memory (heap), not a local array (stack)

//     for (int i = 0; i < 5; i++)
//         // array initialisation
//         temp[i] = i;

//     return temp; // array returned
// }

// int main()
// {
//     int arr[5];        // ❌ Can not do that
//     int *arr = func(); // function call

//     cout << "The Array is : ";
//     for (int i = 0; i < 5; i++)
//         cout << arr[i] << "\t";
// cout << "\n";
//     return 0;
// }
//=========================================================================================================================================

//=========================================================================================================================================
// //Returning an array way 2
// #include <iostream>
// #include <array>
// using namespace std;

// std::array<int, 5> func() // function with return type std::array
// {
//     std::array<int, 5> f_array; // array declared

//     for (int i = 0; i < 5; i++)
//     {
//         // array initialisation
//         f_array[i] = i;
//     }

//     return f_array; // array returned
// }

// int main()
// {
//     std::array<int, 5> arr; // array with length 5

//     arr = func(); // function call

//     cout << "The Array is : ";
//     for (int i = 0; i < 5; i++)
//         cout << arr[i] << "\t";
//   cout << "\n";

//     return 0;
// }
//=========================================================================================================================================

//=========================================================================================================================================
// //Returning an array way 3
// #include <iostream>
// using namespace std;

// struct demo
// {
//     // array declared inside structure
//     int arr[100];
// };

// struct demo func(int n) // return type is struct demo
// {
//     struct demo demo_mem; // demo structure member declared
//     for (int i = 0; i < n; i++)
//     {
//         // array initialisation
//         demo_mem.arr[i] = i;
//     }
//     return demo_mem; // address of structure member returned
// }

// int main()
// {
//     struct demo a;
//     int n = 5; // number of elements

//     a = func(n); // address of arr

//     cout << "The Array is : ";
//     for (int i = 0; i < n; i++)
//     {
//         cout << a.arr[i] << "\t";
//     }

//     return 0;
// }