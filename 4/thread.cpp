#include "chacha.h"
#include <cmath>  // pow function
#include <ctime>  // time
#include <chrono> // time
#include <thread>
#include <sys/time.h>

using namespace std;
using namespace ChaCha;
using namespace Parameters;
using namespace Operation;

ull loopcounter{0};
ull loopcount{0};
u16 totalRound{7}, fwdRound{4};
u16 ID[][2] = {{13, 6}}, OD[][2] = {{2, 0}, {7, 7}, {8, 0}};
// u16 IDsize = sizeof(IDword) / sizeof(IDword[0]), ODsize = sizeof(ODword) / sizeof(ODword[0]);
u16 PNB[] = {2, 3, 4, 5, 6, 39, 47, 48, 66, 67, 68, 69, 70, 71, 72, 73, 74, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 90, 91, 92, 95, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 123, 124, 125, 126, 127, 155, 156, 157, 158, 159, 168, 191, 192, 193, 194, 199, 200, 201, 207, 208, 211, 212, 213, 219, 220, 221, 222, 223, 224, 225, 226, 227, 244, 245, 246, 255};

void bwbias(ull samplesperthread, ull totalSamples)
{
    ull loop{0};
    double matchcount{0};
    u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16];
    u16 fwdBit, bwdBit;
    double total_t;
    struct timeval start_t, end_t;
    while (loop < totalSamples)
    {
        auto loopstart = chrono::high_resolution_clock::now();
        gettimeofday(&start_t, NULL);
        inisialize(x0);
        init_key.Key256(key);
        insertkey(x0, key);

        CopyState(strdx0, x0);
        CopyState(dx0, x0);

        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
        for (const auto &row : ID)
        {
            InputDifference(dx0, row[0], row[1]);
        }
        // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

        CopyState(dstrdx0, dx0);

        for (int i{1}; i <= fwdRound; ++i)
        {
            frward.RoundFunction(x0, i);
            frward.RoundFunction(dx0, i);
        }

        XORDifference(DiffState, 16, x0, dx0);

        fwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

        for (int i{fwdRound + 1}; i <= totalRound; ++i)
        {
            frward.RoundFunction(x0, i);
            frward.RoundFunction(dx0, i);
        }
        // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

        // modular addition
        AddStates(x0, strdx0);
        AddStates(dx0, dstrdx0);

        // randomise the PNBs
        // for (int i{0}; i < sizeof(PNB) / sizeof(PNB[0]); ++i)
        // {
        //     int WORD = (PNB[i] / WORD_SIZE) + 4;
        //     int BIT = PNB[i] % WORD_SIZE;

        //     if (GenerateRandomBoolean())
        //     {
        //         ToggleBit(strdx0[WORD], BIT);
        //         ToggleBit(dstrdx0[WORD], BIT);
        //     }
        // }

        for (int i{0}; i < sizeof(PNB) / sizeof(PNB[0]); ++i)
        {
            int WORD = (PNBdetails.PNB[i] / WORD_SIZE) + 4;
            int BIT = PNBdetails.PNB[i] % WORD_SIZE;

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

        for (int i{totalRound}; i > fwdRound; i--)
        {
            bckward.RoundFunction(x0, i);
            bckward.RoundFunction(dx0, i);
        }

        // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

        XORDifference(DiffState, WORD_COUNT, x0, dx0);

        bwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

        if (fwdBit == bwdBit)
            matchcount++;
        loop++;

        if (loop > 0 && loop % (samplesperthread) == 0)
        {
            // printf("thread with id: %llu | counter: %lu", std::this_thread::get_id(), (counter));

            double varepsilon = (2 * matchcount) / loop - 1.0;
            auto loopend = chrono::high_resolution_clock::now();
            gettimeofday(&end_t, NULL);
            total_t = (end_t.tv_sec - start_t.tv_sec) + (double)(end_t.tv_usec - start_t.tv_usec);

            // printf("🔢#loop: %lld |🎆bias: ~ %.5lf (used %9.9f seconds)/(other measurement %9.9f seconds) \n", ++loopcount, varepsilon, (chrono::duration<double, std::micro>(end - start).count() / 1000000.0), total_t);

            // cout << "🔢 #loop : " << setw(4) << " | 🎆bias : " << fixed << setprecision(6) << varepsilon << " ~ " << setprecision(4) << fabs(varepsilon) << setprecision(2) << " ~ 2^{" << log2(fabs(varepsilon)) << "}"
            //      << " (" << setw(4) << setprecision(4) << chrono::duration<double, std::micro>(end - start).count() << ") microsec\n";

            cout << "|" << setw(10) << ++loopcounter << setw(10) << "|"
                 << (varepsilon >= 0 ? (setw(10)) : setw(10)) << fixed << setprecision(6) << varepsilon << setw(3) << " ~ "
                 << setw(5) << setprecision(5) << fabs(varepsilon) << setw(6)
                 << setprecision(2) << " ~ 2^{" << setw(6) << log2(fabs(varepsilon)) << setw(1) << "}" << setw(3)
                 << "|" << setw(15) << fixed<< setprecision(3)
                 << (chrono::duration<double, std::micro>(loopend - loopstart).count()) << "( " << total_t << " )" << setw(15) << "| \n";
        }
    }
}

int main()
{
    auto str1 = chrono::high_resolution_clock::now();
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime(&t);
    printf("######## Execution started on: %d-%d-%d %d:%d:%d ########\n", lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900, lt->tm_hour, lt->tm_min, lt->tm_sec);

    u16 max_num_threads = pow(2,3);
    cout << "# of threads: " << max_num_threads << "\n";
    cout << "Bias Calculation Started . . . (Shh! It's a multi-threaded programme and will take some time !) \n";
    cout << "Have patience with a cup of tea . . . \n";
    cout << "**************************************************************************************\n";

    basicdetails.cipher = "ChaCha-256";
    basicdetails.programtype = "Backward Bias Determiantion";
    basicdetails.totalround = 7;

    diffdetails.fwdround = 4;

    sampledetails.samplesperThread = pow(2, 20);
    sampledetails.totalLoop = pow(2, 14);
    ull LIMIT = sampledetails.samplesperThread * sampledetails.totalLoop;

    PNBdetails.PNBfile = "europnb.txt";
    PNBdetails.PNB = OpenFile("europnb.txt", &PNBdetails.PNBsize);

    /* u16 PNB[] = {2, 3, 4, 5, 6, 39, 47, 48, 66, 67, 68, 69, 70, 71, 72, 73, 74, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 90, 91, 92, 95, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 123, 124, 125, 126, 127, 155, 156, 157, 158, 159, 168, 191, 192, 193, 194, 199, 200, 201, 207, 208, 211, 212, 213, 219, 220, 221, 222, 223, 224, 225, 226, 227, 244, 245, 246, 255};

  // PNBdetails.PNB = PNB;
  // PNBdetails.PNBsize = sizeof(PNB) / sizeof(PNB[0]);*/

    diffdetails.ID = ID;
    diffdetails.OD = OD;
    diffdetails.IDsize = sizeof(ID) / sizeof(ID[0]);
    diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);
    PrintBasicDetails(basicdetails, cout);
    PrintDiffDetails(diffdetails, cout);
    PrintPNBDetails(PNBdetails, cout);
    PrintSampleDetails(sampledetails, cout);
    PrintBiasLoopEtc(cout);

    thread threadPool[max_num_threads];

    for (int i{0}; i < max_num_threads; ++i)
        threadPool[i] = thread(&bwbias, sampledetails.samplesperThread, LIMIT);
    // Wait for both threads to finish
    for (int i{0}; i < max_num_threads; ++i)
        threadPool[i].join();
    // auto end1 = chrono::high_resolution_clock::now();
    // cout << "Exec. time: " << (std::chrono::duration<double, std::micro>(end1 - str1).count() / 1000000.0) << "seconds\n";
    // return 0;
}
