// To declare an 2d array, we must declare a dimension. e.g. int doublearray[m][n] = {{, , , ... , n elements},
//                                                                                    {, , , ... , n elements},
//                                                                                            .
//                                                                                            .
//                                                                                            .
//                                                                                    {, , , ... , n elements}}
//                                                                                             m rows

#include <iostream>

void func(int darray[][2], int rows, int columns) // No matter what circumstances, you must have to include the column number of an 2d array
{
    for (int i{0}; i < rows; ++i)
        for (int j{0}; j < columns; ++j)
            darray[i][j] = 1;
};

// If the number of columns is fixed but the number of rows can vary, you can use the following syntax:
void func(int (*darray)[2], int rows) // No matter what circumstances, you must have to include the column number of an 2d array
{
    for (int i{0}; i < rows; ++i)
        for (int j{0}; j < 2; ++j)
            darray[i][j] = 1;
};
void printfunc(int (*darray)[2], int rows) // No matter what circumstances, you must have to include the column number of an 2d array
{
    for (int i{0}; i < rows; ++i)
    {
        for (int j{0}; j < 2; ++j)
        {
            std::cout << darray[i][j] << " ";
        }
        std::cout << "\n";
    }
}
// Function that takes a 2D array using a double pointer
void processDoubleArray(double **arr, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            // Access elements of the array
            arr[i][j] = 70;
        }
    }
}

void printDoubleArray(double **arr, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            // Access elements of the array
            std::cout << arr[i][j] <<" ";
        }
        std::cout << "\n";
    }
}



int main()
{
    int arr[][2] = {{0, 1}, {1, 2}};
    // You have to be precise while calling a 2d-array argument, e.g if the 2d array has a 2 columns, you can not pass a 2d array with 3 columns, even with changing the functions parameters
    // func(darray, 3);
    // processDoubleArray(darray, 3, 2);// ❌ we can not do that for 2d array
    double **darray1; // ❌ can not do that, same as for 1d array, int* arr; will not work, dynamically allocate memory
    darray1 = new double *[3]; // rows are to be specified
    for (int i{0}; i < 3;++i)
        darray1[i] = new double [2]; // columns are to be specified
    
    processDoubleArray(darray1, 3, 2);

    printDoubleArray(darray1, 3, 2);

    // Don't forget to free the allocated memory
    for (int i = 0; i < 3; ++i)
        delete[] darray1[i];
    delete[] darray1;

    int darray[3][2];
    func(darray, 3);
    printfunc(darray, 3);


//-----------------------------------------------------------------------------------------------------
    int(*dar)[2];
    return 0;
}