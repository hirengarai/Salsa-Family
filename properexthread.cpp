#include <iostream>
#include <future>
#include <thread>
#include <cmath>
#include <vector>
#include <iomanip>

#define ul unsigned int

using namespace std;
using namespace std::chrono;

// threaded way
ul threadFun(ul start, ul end, ul id, ul id1, promise<ul> &&promise)
{
    ul loop{0}, count{0}, N = 20676, T = 10501, success{0};

    for (ul i{start}; i <= end; ++i)
    {

        // seed generate 1
        time_t timeInSec;
        time(&timeInSec);

        unsigned short seed[3];
        seed[0] = (short)(i + 3) * (i + 3) * (id + 7) + timeInSec;
        seed[1] = (short)(i + 5) * (i + 5) * (id + 17) + timeInSec;
        seed[2] = (short)(i + 11) * (i + 11) * (id + 29) + timeInSec;
        // end here

        // // seed generate 2
        // // setup unbuffered urandom
        // FILE* urandom = fopen("/dev/urandom", "r");
        // setvbuf(urandom, NULL, _IONBF, 0);  // turn off buffering

        // // setup state buffer
        // unsigned short seed[3];
        // // fgetc() returns a `char`, we need to fill a `short`
        // seed[0] = (fgetc(urandom) << 8) | fgetc(urandom);
        // seed[1] = (fgetc(urandom) << 8) | fgetc(urandom);
        // seed[2] = (fgetc(urandom) << 8) | fgetc(urandom);
        // fclose(urandom);
        // // end here

        count = 0;
        loop = 0;
        while (loop < N)
        {
            if (erand48(seed) < 0.5)
            {
                count++;
            }
            loop++;
        }
        if (count >= T)
        {
            success++;
        }
    }
    promise.set_value(success);
    return success;
}

// normal way
ul normalFun(ul start, ul end)
{
    ul loop{0}, count{0}, N = 20676, T = 10501, success{0};
    for (ul i = start; i <= end; ++i)
    {
        count = 0;
        loop = 0;
        while (loop < N)
        {
            if (drand48() < 0.5)
            {
                count++;
            }
            loop++;
        }
        if (count >= T)
        {
            success++;
        }
    }
    return success;
}

int main()
{

    const int processor_count = std::thread::hardware_concurrency();
    cout << "Number of CPU cores : " << processor_count << endl;
    cout << "Thread Portion Started ... \n";
    const int max_num_threads = processor_count;

    const ul CON = pow(2, 14);

    vector<thread> threadPool(max_num_threads);
    vector<future<ul>> futuResults(max_num_threads);
    vector<ul> threadSum(max_num_threads);
    double SUM{0.0}, probability;

    srand48(time(NULL));

    // thread portion started
    auto start = chrono::high_resolution_clock::now();

    // threads are called
    for (ul i{0}; i < max_num_threads; ++i)
    {
        promise<ul> prms;

        futuResults.at(i) = prms.get_future();
        threadPool.at(i) = thread(&threadFun, ((CON * i) + 1), (CON * (i + 1)), i, i, move(prms));
    }

    for (ul i{0}; i < max_num_threads; ++i)
    {
        threadPool.at(i).join();
    }

    for (ul i{0}; i < max_num_threads; ++i)
    {
        threadSum.at(i) = futuResults.at(i).get();
        cout << "SUM from " << i << "-th thread = " << threadSum.at(i) << endl;
        SUM += threadSum.at(i);
    }

    probability = (SUM / (max_num_threads * CON));

    auto end = chrono::high_resolution_clock::now();
    cout << "time taken is " << chrono::duration<double, std::milli>(end - start).count() << " msec" << endl;
    cout << "False Alarm = " << setprecision(5) << probability << endl;
    cout << "Thread Portion Ended Successfully.\n";
    // thread ended

    // normal portion started
    cout << "Normal Prog Started ... \n";
    start = chrono::high_resolution_clock::now();
    double S = normalFun(0, CON * max_num_threads);
    end = chrono::high_resolution_clock::now();
    cout << "time taken in normal way is " << chrono::duration<double, std::milli>(end - start).count() << " msec" << endl;
    probability = (S / (CON * max_num_threads));
    cout << "False Alarm = " << setprecision(5) << probability << endl;
    cout << "Normal Prog Ended Successfully.\n";
    // normal portion ended
}