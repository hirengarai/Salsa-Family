#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <iomanip>

double GenerateRandomBits()
{
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dis(23000000, 44000000);
    return dis(gen);
}

void threadfunc(double rand)
{
    auto threadstart = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration(rand);
    std::this_thread::sleep_for(duration);
    auto threadend = std::chrono::high_resolution_clock::now();

    auto dura = std::chrono::duration<double, std::micro>(threadend - threadstart).count()/1000000;
    std::cout << "⌛: " << std::fixed << std::setprecision(3) << dura << "\n";
}

int main()
{
    int mthread = 8;
    std::thread threadpool[mthread];

    double rand;
    for (int i{0}; i < pow(2, 0); ++i)
    {
        for (int i{0}; i < mthread; ++i)
        {
            rand = GenerateRandomBits();
            threadpool[i] = std::thread(&threadfunc, rand);
        }

        for (int i{0}; i < mthread; ++i)
            threadpool[i].join();
    }
}