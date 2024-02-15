#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Function to determine if a number should be included in the array
bool includeNumber(int number, int loopSize)
{
    int counter = 0;
    for (int i = 0; i < loopSize; ++i)
    {
        if ((double)rand() / RAND_MAX > 0.5)
        {
            counter++;
        }
    }

    double inclusionProbability = static_cast<double>(counter) / loopSize;
    return inclusionProbability > 0.4;
}

// Function to process numbers in a given range
void processNumbers(int start, int end, int loopSize, std::vector<int> &resultArray)
{
    for (int i = start; i <= end; ++i)
    {
        if (includeNumber(i, loopSize))
        {
            resultArray.push_back(i);
        }
    }
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(0))); // Seed for random number generation

    std::vector<int> resultArray;

    // Create three threads for each range of numbers
    std::thread thread1(processNumbers, 1, 3, static_cast<int>(std::pow(2, 15)), std::ref(resultArray));
    std::thread thread2(processNumbers, 4, 6, static_cast<int>(std::pow(2, 20)), std::ref(resultArray));
    std::thread thread3(processNumbers, 7, 10, static_cast<int>(std::pow(2, 25)), std::ref(resultArray));

    // Wait for all threads to finish
    thread1.join();
    thread2.join();
    thread3.join();

    // Print the result array
    std::cout << "Result Array: ";
    for (int num : resultArray)
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}
