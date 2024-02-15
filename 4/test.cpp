#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <ctime>
#include <chrono>

thread_local int j{100};
void thFun(int i)
{
    std::cout << "Hi from thread " << i + j << "!\n";
}

bool generateRandomBoolean()
{
    thread_local std::mt19937 mt_engine(std::random_device{}());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return (distribution(mt_engine) > 0.5);

    // thread_local std::mt19937 mt_engine(static_cast<unsigned>(time(NULL)));
    // std::uniform_real_distribution<double> distribution(0.0, 1.0);
    // return (distribution(mt_engine) > 0.5);

    // return (drand48() > 0.5);
}

int main()
{
    srand48(time(NULL));
    // std::vector<std::thread> workers;
    // for (int i{0}; i < 8; ++i)
    // {
    //     auto th = std::thread(&thFun,i);
    //     workers.push_back(std::move(th));
    //     assert(!th.joinable());
    // }
    // std::cout << "Hi from main!\n";
    // std::for_each(workers.begin(), workers.end(), [](std::thread &th)
    //               { assert(th.joinable());
    //               th.join(); });
    // return 0;

    for (int i{0}; i < 100; ++i)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << generateRandomBoolean();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "||Time: " << std::chrono::duration<double, std::micro>(end - start).count() << "\n";
    }
}