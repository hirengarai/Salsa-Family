/*
 * REFERENCE IMPLEMENTATION OF backward bias epsilon_a searching programme.
 *
 * Filename: cbwbias.cpp
 *
 * created: 24/09/23
 * updated: 05/01/24
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

#include "chacha.h"
#include <cmath> // pow function
#include <ctime> // time

using namespace std;

// Aumasson et al.
// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0};
// int ODword[] = { 2,6,6,10,14 }, ODbit[] = { 0,7,19,12,0};
// int ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
// eurocrypt
u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
ull counter = 0;
int main(int argc, char *argv[])
{
    srand48(time(NULL));

    DetailsParams dparams;
    dparams.cipher = "ChaCha";
    dparams.progtype = "BW bias finding";
    dparams.totalround = 7;
    dparams.fwdround = 4;
    dparams.IDbit = IDbit;
    dparams.IDword = IDword;
    dparams.IDsize = sizeof(IDword) / sizeof(IDword[0]);
    dparams.ODbit = ODbit;
    dparams.ODword = ODword;
    dparams.ODsize = sizeof(ODword) / sizeof(ODword[0]);
    dparams.totalLoopCount = pow(2, 63);
    dparams.samplesPerloop = pow(2, 21);
    PutValueParams pvparams;
    Details(dparams, cout);
    // --------------------- input the PNB file with PNB size as the last element ----------------------------
    if (argc > 1)
    {
        pvparams.mode = "block";
        cout << "                 Block mode \n";
        u16 **BLOCK;
        BLOCK = OpenFile("pnbblock.txt", &pvparams.list1size, &pvparams.list2size);
        cout << "|            # of PNBs in block form: " << pvparams.list1size << ", # of rest of PNBs: " << pvparams.list2size
             << "                 " << setw(7) << "|\n";
        cout << "+------------------------------------------------------------------------------------+\n";
        pvparams.list1 = BLOCK[0];
        pvparams.list2 = BLOCK[1];
    }
    // else
    // {
    //   pvparams.y = OpenFile("231339_06012024_synco_0.71_pnbs_ChaCha_7_id_13_13.txt", &dparams.PNBcount);
    //   pvparams.ysize = dparams.PNBcount;
    //   cout << "              Number of PNBs: " << pvparams.ysize << "\n";

    //   cout << "+------------------------------------------------------------------------------------+\n";
    // }

    u16 PNB[] = {2, 3, 4, 5, 47, 48, 49, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 77, 78, 79, 80, 81, 82, 83, 84,
                 85, 86, 87, 90, 91, 102, 103, 104, 105, 106, 107, 108, 109, 110, 123, 124, 125, 126, 127, 155, 156, 157, 158, 159, 168, 169, 192, 193, 194};
    pvparams.y = PNB;
    pvparams.ysize = sizeof(PNB) / sizeof(PNB[0]);
    cout << "              Number of PNBs: " << pvparams.ysize << "\n";

    u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16];
    u16 fwdBit, bwdBit;
    ull loop{0};
    double count{0};

    FORWARD fwd;
    BACKWARD bwd;
    INITIALISE_IV init_iv;
    INITIALISE_KEY init_key;
    INSERT_KEY insert_key;
    STATES state;
    PUT_VALUE pv;

    while (loop < dparams.totalLoopCount)
    {
        init_iv.chacha(x0);
        init_key.Key256(key);
        insert_key.chacha(x0, key);

        state.CopyState(strdx0, x0, 16);
        state.CopyState(dx0, x0, 16);

        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
        u16 index{0};
        for (const u16 &i : IDword)
        {
            state.InputDifference(dx0, i, IDbit[index]);
            index++;
        }
        // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

        state.CopyState(dstrdx0, dx0, 16);

        for (int i{1}; i <= dparams.fwdround; ++i)
        {
            fwd.RoundFunction(x0, i);
            fwd.RoundFunction(dx0, i);
        }

        state.XORDifference(DiffState, 16, x0, dx0);

        fwdBit = state.DifferenceBit(DiffState, dparams.ODword, dparams.ODbit, dparams.ODsize);

        for (int i{dparams.fwdround + 1}; i <= dparams.totalround; ++i)
        {
            fwd.RoundFunction(x0, i);
            fwd.RoundFunction(dx0, i);
        }
        // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

        // modular addition
        // state.AddStates(x0, strdx0);
        // state.AddStates(dx0, dstrdx0);

        // bool blck1 (GET_BIT(x0[4], 6) != GET_BIT(strdx0[4], 6));
        // bool dblck1 (GET_BIT(dx0[4], 6) != GET_BIT(dstrdx0[4], 6));

        // bool blck2(GET_BIT(x0[5], 18) != GET_BIT(strdx0[5], 18));
        // bool dblck2(GET_BIT(dx0[5], 18) != GET_BIT(dstrdx0[5], 18));

        // bool blck3(GET_BIT(x0[6], 12) != GET_BIT(strdx0[6], 12));
        // bool dblck3(GET_BIT(dx0[6], 12) != GET_BIT(dstrdx0[6], 12));

        // bool blck31(GET_BIT(x0[6], 24) != GET_BIT(strdx0[6], 24));
        // bool dblck31(GET_BIT(dx0[6], 24) != GET_BIT(dstrdx0[6], 24));

        // bool blck32(GET_BIT(x0[6], 28) != GET_BIT(strdx0[6], 28));
        // bool dblck32(GET_BIT(dx0[6], 28) != GET_BIT(dstrdx0[6], 28));

        // if (blck1 && dblck1 && blck2 && dblck2)
        //     break;

        pvparams.x = x0;
        pvparams.x1 = dx0;
        // randomise the PNBs
        pv.chacha(pvparams);

        // modular subtraction of states
        // state.SubtractStates(x0, strdx0);
        // state.SubtractStates(dx0, dstrdx0);

        // ---------------------------BW ROUND STARTS--------------------------------------------------------------------

        for (int i{dparams.totalround}; i > dparams.fwdround; i--)
        {
            bwd.RoundFunction(x0, i);
            bwd.RoundFunction(dx0, i);
        }

        // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

        state.XORDifference(DiffState, 16, x0, dx0);

        bwdBit = state.DifferenceBit(DiffState, dparams.ODword, dparams.ODbit, dparams.ODsize);

        // epsilon_a find
        if (fwdBit == bwdBit)
            count++;
        loop++;

        if (loop > 0 && loop % (dparams.samplesPerloop) == 0)
        {
            double varepsilon = 2 * (count / (double)loop) - 1.0;
            cout << "🔢 #loop : " << setw(4) << ++counter << " | 🎆bias : " << fixed << setprecision(7) << varepsilon << " ~ " << setprecision(5) << fabs(varepsilon) << setprecision(2) << " ~ 2 ^{" << log2(fabs(varepsilon)) << "}\n";
        }
    }
}
