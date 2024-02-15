/*
 * REFERENCE IMPLEMENTATION OF backward bias epsilon_a searching programme.
 *
 * Filename: cbwbias.cpp
 *
 * created: 24/09/23
 * updated: 03/01/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the backward bias searching programme for the stream cipher ChaCha.
 * running command: g++ cbwbias.cpp && ./a.out
 * necessary files to run the prog: commonfiles.h, chacha.h, 2 txt files containg the PNBs and PNBs in block form
 */

#include "final.h"
#include <cmath> // pow function
#include <ctime> // time
#include <chrono>

using namespace std;
using namespace ChaCha;
using namespace Operation;
using namespace Parameters;

// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}; // Aumasson et al.
// int ODword[] = { 2,6,6,10,14 }, ODbit[] = { 0,7,19,12,0};
// int ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
// eurocrypt
u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
size_t IDsize = sizeof(IDword) / sizeof(IDword[0]);
size_t ODsize = sizeof(ODword) / sizeof(ODword[0]);
ull counter = 0;
int main(int argc, char *argv[])
{
    srand48(time(NULL));

    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime(&t);
    printf("######## Start Time: %d-%d-%d %d:%d:%d ########\n\n\n", lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900, lt->tm_hour, lt->tm_min, lt->tm_sec);

    // // --------------------- input the PNB file with PNB size as the last element ----------------------------
    // if (argc > 1)
    // {
    //     pvparams.mode = "block";
    //     cout << "                 Block mode \n";
    //     u16 **BLOCK;
    //     BLOCK = OpenFile("pnbblock.txt", &pvparams.list1size, &pvparams.list2size);
    //     cout << "|            # of PNBs in block form: " << pvparams.list1size << ", # of rest of PNBs: " << pvparams.list2size
    //          << "                 " << setw(7) << "|\n";
    //     cout << "+------------------------------------------------------------------------------------+\n";
    //     pvparams.list1 = BLOCK[0];
    //     pvparams.list2 = BLOCK[1];
    // }
    // else
    // {
    //     pvparams.y = OpenFile("aumasson.txt", &dparams.PNBcount);
    //     pvparams.ysize = dparams.PNBcount;
    //     cout << "+------------------------------------------------------------------------------------+\n";
    // }

    u16 PNB[] = {2, 3, 4, 5, 6, 39, 47, 48, 66, 67, 68, 69, 70, 71, 72, 73, 74, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 90, 91, 92, 95, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 123, 124, 125, 126, 127, 155, 156, 157, 158, 159, 168, 191, 192, 193, 194, 199, 200, 201, 207, 208, 211, 212, 213, 219, 220, 221, 222, 223, 224, 225, 226, 227, 244, 245, 246, 255};
    pnbvalue.pnb = PNB;
    pnbvalue.pnbsize = sizeof(PNB) / sizeof(PNB[0]);

    details.totalsamples = pow(2, 62);
    details.samplesPerloop = pow(2, 20);
    details.ROUNDS = 7;
    details.FWDROUNDS = 4;

    u32 x0[WORD_COUNT], strdx0[WORD_COUNT], key[KEY_COUNT], dx0[WORD_COUNT], dstrdx0[WORD_COUNT], z[WORD_COUNT], dz[WORD_COUNT], diffState[WORD_COUNT];
    u16 fwdBit, bwdBit;
    ull loop{0};
    double count{0};

    while (loop < details.totalsamples)
    {
        auto start = chrono::high_resolution_clock::now();
        inisialize(x0);
        initkey.Key256(key);
        insertkey(x0, key);

        operate.copyState(strdx0, x0, 16);
        operate.copyState(dx0, x0, 16);

        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
        u16 index{0};
        for (const u16 &i : IDword)
        {
            operate.modifyBits(dx0[i], IDbit[index], SURE_TOGGLE);
            index++;
        }
        // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

        operate.copyState(dstrdx0, dx0, 16);

        // pvparams.x = strdx0;
        // pvparams.x1 = dstrdx0;

        for (int i{1}; i <= details.FWDROUNDS; ++i)
        {
            fwd.RoundFunction(x0, i);
            fwd.RoundFunction(dx0, i);
        }

        operate.xorDifference(diffState, WORD_COUNT, x0, dx0);

        fwdBit = operate.differenceBit(diffState, ODword, ODbit, ODsize);

        for (int i{details.FWDROUNDS + 1}; i <= details.ROUNDS; ++i)
        {
            fwd.RoundFunction(x0, i);
            fwd.RoundFunction(dx0, i);
        }
        // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

        // modular addition
        operate.addStates(x0, strdx0, z);
        operate.addStates(dx0, dstrdx0, dz);

        // randomise the PNBs
        pnbvalue.x = strdx0;
        pnbvalue.x1 = dstrdx0;
        pnbValue(pnbvalue);

        // modular subtraction of states
        operate.subtractStates(z, strdx0, x0);
        operate.subtractStates(dz, dstrdx0, dx0);

        // ---------------------------BW ROUND STARTS--------------------------------------------------------------------

        for (int i{details.ROUNDS}; i > details.FWDROUNDS; i--)
        {
            bwd.RoundFunction(x0, i);
            bwd.RoundFunction(dx0, i);
        }

        // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

        operate.xorDifference(diffState, WORD_COUNT, x0, dx0);

        bwdBit = operate.differenceBit(diffState, ODword, ODbit, ODsize);

        // epsilon_a find
        if (fwdBit == bwdBit)
            count++;
        loop++;

        if (loop > 0 && loop % details.samplesPerloop == 0)
        {
            auto end = chrono::high_resolution_clock::now();
            double varepsilon = 2 * (count / (double)loop) - 1.0;
            cout << "🔢 #loop : " << setw(4) << ++counter << " | 🎆bias : " << fixed << setprecision(6) << varepsilon << " ~ " << setprecision(4) << fabs(varepsilon) << setprecision(2) << " ~ 2 ^{" << log2(fabs(varepsilon)) << "}"
                 << "Time taken: "<< setprecision(4) << chrono::duration<double, std::micro>(end - start).count() << " microsec\n";
        }
    }
}
