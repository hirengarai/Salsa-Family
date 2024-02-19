/*
 * REFERENCE IMPLEMENTATION OF backward bias (epsilon_a) searching programme in multi-threaded manner in cpp
 *
 * Filename: cbwbiasthreaded.cpp
 *
 * created: 24/09/23
 * updated: 12/02/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the backward bias searching programme for the stream cipher ChaCha.
 * running command: g++ cbwbiasthreaded.cpp && ./a.out
 * necessary files to run the prog: commonfiles2.h, chacha.h, one txt file containg the PNBs
 */

#include "chacha.h"   // chacha round functions
#include <cmath>      // pow function
#include <ctime>      // time
#include <chrono>     // execution time duration
#include <fstream>    // storing output in a file
#include <future>     // multithreading
#include <sys/time.h> // execution time started
#include <thread>     // multithreading

using namespace std;
using namespace ChaCha;
using namespace Parameters;
using namespace Operation;

// eurocrypt ID-OD
u16 ID[][2] = {{13, 6}}, OD[][2] = {{2, 0}, {7, 7}, {8, 0}};

double bwbias(promise<double> &&matchcount);

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

    basicdetails.cipher = "ChaCha-256";
    basicdetails.programtype = "Backward Bias Determiantion";
    basicdetails.totalround = 7;

    diffdetails.fwdround = 4;

    u16 max_num_threads = pow(2, 3);
    sampledetails.samplesperThread = pow(2, 18);
    sampledetails.totalLoop = pow(2, 14);
    sampledetails.samplesperLoop = sampledetails.samplesperThread * max_num_threads;

    PNBdetails.PNBblockflag = true;
    PNBdetails.PNBfile = "pnbblock.txt";
    u16 **PNB;
    PNB = OpenFile("pnbblock.txt", &PNBdetails.PNBinblocksize, &PNBdetails.restPNBsize);
    PNBdetails.PNBinblock = PNB[0];
    PNBdetails.restPNB = PNB[1];
    

    // u16 PNB[] = {2, 3, 4, 5, 6, 39, 47, 48, 66, 67, 68, 69, 70, 71, 72, 73, 74, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 90, 91, 92, 95, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 123, 124, 125, 126, 127, 155, 156, 157, 158, 159, 168, 191, 192, 193, 194, 199, 200, 201, 207, 208, 211, 212, 213, 219, 220, 221, 222, 223, 224, 225, 226, 227, 244, 245, 246, 255};

    // PNBdetails.PNB = PNB;
    // PNBdetails.PNBsize = sizeof(PNB) / sizeof(PNB[0]);

    diffdetails.ID = ID;
    diffdetails.OD = OD;
    diffdetails.IDsize = sizeof(ID) / sizeof(ID[0]);
    diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);
    diffdetails.precision_digit = 5;

    PrintBasicDetails(basicdetails, cout);
    PrintDiffDetails(diffdetails, cout);
    PrintPNBDetails(PNBdetails, cout);
    PrintSampleDetails(sampledetails, cout);

    ofstream file("blockbias" + PNBdetails.PNBfile);
    if (file.is_open())
    {
        file << "######## Execution started on: "
             << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
             << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
             << " ########\n";
        file << "The file contains biases of the PNBs from the file " << PNBdetails.PNBfile << "\n";
    }

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
            threadPool[i] = thread(&bwbias, move(prms));
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
        {
            if (file.is_open())
            {
                file << "Median Bias ~ " << fixed << setprecision(5) << CalculateMedian(apprxbiaslist, sizeof(apprxbiaslist) / sizeof(apprxbiaslist[0])) << "\n";
                file << "The latest " << continuitycount << " biases are as follows:\n";
                for (const auto &i : apprxbiaslist)
                    file << i << "\n";

                file << "Number of loops: " << loop << "\n";
            }
            break;
        }
    }
    if (loop == sampledetails.totalLoop)
    {
        cout << "The bias does not converge\n";
        file << "The bias does not converge\n";
    }

    time(&t);           // Gets the current time since epoch (Jan 1, 1970)
    lt = localtime(&t); // Converts the time stored in t to a local time representation and stores it in the tm structure pointed to by lt.
    auto progend = std::chrono::high_resolution_clock::now();
    auto duration = chrono::duration<double, std::micro>(progend - progstart).count();
    cout << "######## Execution ended on: "
         << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
         << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
         << " ########\n";
    if (file.is_open())
        file << "######## Execution ended on: "
             << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
             << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
             << " ########\n";
    cout << "Total execution time: " << duration / 1000000.0 << " seconds ~ " << duration / 60000000.0 << " minutes ~ " << duration / 3600000000.0 << " hours.\n";
    if (file.is_open())
        file << "Total execution time: " << duration / 1000000.0 << " seconds ~ " << duration / 60000000.0 << " minutes ~ " << duration / 3600000000.0 << " hours.\n";
    file.close();
    return 0;
}

double bwbias(promise<double> &&matchcount)
{
    double threadloop{0}, thread_match_count{0};
    u32 x0[WORD_COUNT], strdx0[WORD_COUNT], key[KEY_COUNT], dx0[WORD_COUNT], dstrdx0[WORD_COUNT], DiffState[WORD_COUNT];
    u16 fwdBit, bwdBit;

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
        for (int i{1}; i <= diffdetails.fwdround; ++i)
        {
            frward.RoundFunction(x0, i);
            frward.RoundFunction(dx0, i);
        }

        XORDifference(DiffState, WORD_COUNT, x0, dx0);

        fwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

        for (int i{diffdetails.fwdround + 1}; i <= basicdetails.totalround; ++i)
        {
            frward.RoundFunction(x0, i);
            frward.RoundFunction(dx0, i);
        }
        // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

        // modular addition
        AddStates(x0, strdx0);
        AddStates(dx0, dstrdx0);

        // randomise the PNBs
        u32 WORD;
        u16 BIT;
        // for (int i{0}; i < PNBdetails.PNBsize; ++i)
        // {
        //     // int WORD = (PNBdetails.PNB[i] / WORD_SIZE) + 4;
        //     // int BIT = PNBdetails.PNB[i] % WORD_SIZE;
        //     calculateWORDandBIT(PNBdetails.PNB[i], WORD, BIT);

        //     if (GenerateRandomBoolean())
        //     {
        //         ToggleBit(strdx0[WORD], BIT);
        //         ToggleBit(dstrdx0[WORD], BIT);
        //     }
        // }

        // u32 WORD;
        // u16 BIT;
        for (int i{0}; i < PNBdetails.PNBinblocksize; ++i)
        {
            calculateWORDandBIT(PNBdetails.PNBinblock[i], WORD, BIT);
            if ((i % 5) == 4)
            {
                SetBit(strdx0[WORD], BIT);
                SetBit(dstrdx0[WORD], BIT);
            }
            else
            {
                UnsetBit(strdx0[WORD], BIT);
                UnsetBit(dstrdx0[WORD], BIT);
            }
        }
        for (int i{0}; i < PNBdetails.restPNBsize; ++i)
        {
            calculateWORDandBIT(PNBdetails.restPNB[i], WORD, BIT);

            if (GenerateRandomBoolean())
            {
                ToggleBit(strdx0[WORD], BIT);
                ToggleBit(dstrdx0[WORD], BIT);
            }
        }

        // modular subtraction of states
        SubtractStates(x0, strdx0);
        SubtractStates(dx0, dstrdx0);

        // ---------------------------BW ROUND STARTS--------------------------------------------------------------------
        for (int i{basicdetails.totalround}; i > diffdetails.fwdround; i--)
        {
            bckward.RoundFunction(x0, i);
            bckward.RoundFunction(dx0, i);
        }
        // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

        XORDifference(DiffState, WORD_COUNT, x0, dx0);

        bwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

        if (fwdBit == bwdBit)
            thread_match_count++;
        threadloop++;
    }
    matchcount.set_value(thread_match_count);
    return thread_match_count;
}
