/*
 * REFERENCE IMPLEMENTATION OF forward bias (epsilon_d) searching programme in multi-threaded manner in cpp
 *
 * Filename: forroforward.cpp
 *
 * created: 20/2/24
 * updated: 20/2/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the forward bias searching programme for the stream cipher Forro.
 * running command: g++ forroforward.cpp && ./a.out
 * necessary files to run the prog: commonfiles2.h, forro.h
 */

#include "forro.h"    // forro round functions
#include <cmath>      // pow function
#include <ctime>      // time
#include <chrono>     // execution time duration
#include <fstream>    // storing output in a file
#include <future>     // multithreading
#include <sys/time.h> // execution time started
#include <thread>     // multithreading

using namespace std;
using namespace Forro;
using namespace Parameters;
using namespace Operation;

// eurocrypt ID-OD
u16 ID[][2] = {{5, 11}}, OD[][2] = {{8, 0}};

double fwbiascount(promise<double> &&matchcount);

int main()
{
    auto progstart = std::chrono::high_resolution_clock::now();
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime(&t);
    // printf("######## Execution started on: %d-%d-%d %d:%d:%d ########\n", lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900, lt->tm_hour, lt->tm_min, lt->tm_sec);
    cout << "######## Execution started on: "
         << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
         << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
         << " ########\n";

    basicdetails.cipher = "Forro14";
    basicdetails.programtype = "Forward Bias Determiantion";

    diffdetails.fwdround = 2;

    u16 max_num_threads = pow(2, 3);
    sampledetails.samplesperThread = pow(2, 18);
    sampledetails.totalLoop = pow(2, 14);
    sampledetails.samplesperLoop = sampledetails.samplesperThread * max_num_threads;

    diffdetails.ID = ID;
    diffdetails.OD = OD;
    diffdetails.IDsize = sizeof(ID) / sizeof(ID[0]);
    diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);
    diffdetails.precision_digit = 5;

    PrintBasicDetails(basicdetails, cout);
    PrintDiffDetails(diffdetails, cout);
    PrintPNBDetails(PNBdetails, cout);
    PrintSampleDetails(sampledetails, cout);

    cout << "# of threads: " << max_num_threads << "\n";
    cout << "Bias Calculation Started . . . (Shh! It's a multi-threaded programme and will take some time !) \n";
    cout << "Have patience with a cup of tea . . . \n";
    PrintBiasLoopEtc(cout);

    thread threadPool[max_num_threads];
    future<double> futureCounts[max_num_threads];

    u16 apprxbiascounter{0}, apprxbiasindex{0}, continuitycount{1000};
    double apprxbiaslist[continuitycount], loop{0}, SUM{0}, varepsilon, total_t, temp{0}, precisionlimit = pow(10, -diffdetails.precision_digit);
    bool closeflag = false;
    while (loop < sampledetails.totalLoop)
    {
        auto loopstart = std::chrono::high_resolution_clock::now();
        for (int i{0}; i < max_num_threads; ++i)
        {
            promise<double> prms;
            futureCounts[i] = prms.get_future();
            threadPool[i] = thread(&fwbiascount, move(prms));
        }
        // Wait for threads to finish
        for (int i{0}; i < max_num_threads; ++i)
            threadPool[i].join();

        for (int i{0}; i < max_num_threads; ++i)
            SUM += futureCounts[i].get();

        varepsilon = 2 * SUM / (++loop * ((sampledetails.samplesperThread * max_num_threads))) - 1.0;

        closeflag = false;
        if (fabs(fabs(varepsilon) - temp) <= precisionlimit)
        {
            closeflag = true;
            apprxbiascounter++;
            apprxbiaslist[apprxbiasindex++] = varepsilon;
        }
        else
        {
            apprxbiascounter = 0;
            apprxbiasindex = 0;
        }
        auto loopend = chrono::high_resolution_clock::now();

        cout << "|" << setw(10) << static_cast<ull>(loop) << setw(13) << "|"
             << (varepsilon >= 0 ? (setw(10)) : setw(10)) << fixed << setprecision(6) << varepsilon << setw(3) << " ~ "
             << setw(5) << setprecision(5) << fabs(varepsilon) << setw(6)
             << setprecision(2) << " ~ 2^{" << setw(6) << log2(fabs(varepsilon)) << setw(1) << "}" << setw(2)
             << "|" << setw(18) << fixed << setprecision(3)
             << (chrono::duration<double, std::micro>(loopend - loopstart).count()) / 1000000.0 << setw(16) << "|"
             << setw(14) << ((closeflag) ? "✅ (" + to_string(apprxbiascounter) + ")" : "❌") << setw(10) << "|\n";

        temp = fabs(varepsilon);

        if (apprxbiascounter >= continuitycount)
            break;
    }
    if (loop == sampledetails.totalLoop)
    {
        cout << "The bias does not converge\n";
    }

    time(&t);           // Gets the current time since epoch (Jan 1, 1970)
    lt = localtime(&t); // Converts the time stored in t to a local time representation and stores it in the tm structure pointed to by lt.
    auto progend = std::chrono::high_resolution_clock::now();
    auto duration = chrono::duration<double, std::micro>(progend - progstart).count();
    cout << "######## Execution ended on: "
         << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
         << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
         << " ########\n";

    cout << "Total execution time: " << duration / 1000000.0 << " seconds ~ " << duration / 60000000.0 << " minutes ~ " << duration / 3600000000.0 << " hours.\n";
    return 0;
}

double fwbiascount(promise<double> &&matchcount)
{
    double threadloop{0}, thread_match_count{0};
    u32 x0[WORD_COUNT], strdx0[WORD_COUNT], key[KEY_COUNT], dx0[WORD_COUNT], dstrdx0[WORD_COUNT], DiffState[WORD_COUNT];
    u16 fwdBit;

    while (threadloop < sampledetails.samplesperThread)
    {
        inisialize(x0);
        init_key.Key256(key);
        insertkey(x0, key);

        CopyState(strdx0, x0);
        CopyState(dx0, x0);
        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
        for (const auto &row : ID)
            ToggleBit(dx0[row[0]], row[1]);
        CopyState(dstrdx0, dx0);
        // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

        FWDQR((x0[0]), (x0[4]), (x0[8]), (x0[12]), (x0[3]), false);
        FWDQR((dx0[0]), (dx0[4]), (dx0[8]), (dx0[12]), (dx0[3]), false);

        XFWDQR(x0[1], x0[5], x0[9], x0[13], x0[0]);
        XFWDQR(dx0[1], dx0[5], dx0[9], dx0[13], dx0[0]);

        frward.Half_2_ODDRF(x0);
        frward.Half_2_ODDRF(dx0);

        for (int i{2}; i <= diffdetails.fwdround; ++i)
        {
            frward.RoundFunction(x0, i);
            frward.RoundFunction(dx0, i);
        }

        frward.Half_1_ODDRF(x0);
        frward.Half_1_ODDRF(dx0);

        XORDifference(DiffState, WORD_COUNT, x0, dx0);

        fwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

        if (fwdBit & 1)
            thread_match_count++;
        threadloop++;
    }
    matchcount.set_value(thread_match_count);
    return thread_match_count;
}
