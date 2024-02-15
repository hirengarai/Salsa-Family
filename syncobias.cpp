/*
 * REFERENCE IMPLEMENTATION OF backward bias epsilon_a searching programme.
 *
 * Filename: cbwepsilon.cpp
 *
 * created: 30/12/23
 * updated: 30/12/23
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the backward bias searching programme for the stream cipher Salsa/ ChaCha.
 * running command: g++ cbwepsilon.cpp && ./a.out
 */

#include "chacha.h"
#include <cmath>   // pow function
#include <ctime>   //  time
#include <iomanip> // decimal numbers upto certain places

using namespace std;

// DIFFFERENTIAL ATTACK PARAMETERS
// Change these parameters

char cipher[8] = "ChaCha";
u16 totalround = 7;
// // Aumasson et al.
// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}, fwdround{3}, totalround{6}; // Aumasson et al.
// Aumasson et al.

// new dist. of 23
// u16 IDword[] = {15, 15}, IDbit[] = {7, 19}, ODword[] = {2, 6, 6, 10, 14}, ODbit[] = {0, 7, 19, 12, 0}, fwdround{4}, totalround{6};
// new dist. of 23

// eurocrypt
u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0}, fwdround{4}, IDsize{1}, ODsize{3};
// eurocrypt

ull ITERATION = pow(2, 60), loopLim = pow(2, 21), counter{0};

u16 PNB[] =
    {6, 14, 39, 51, 59, 95, 111, 115, 119, 135, 160, 227};

u16 PNBcount = sizeof(PNB) / sizeof(PNB[0]);

int main()
{
    srand48(time(NULL));

    // ---------------------------Printing--------------------------------------------------------------------
    cout << "            Backward bias determining programe\n";
    cout << "            Cipher Name: " << cipher << ", # of rounds: " << (unsigned)totalround << ", # of fwd rounds: " << fwdround << "\n";
    cout << "            ID = ";
    for (int i{0}, n = sizeof(IDword) / sizeof(IDword[0]); i < n; ++i)
    {
        if (i == n - 1)
            cout << "(" << IDword[i] << ", " << IDbit[i] << ")\n";
        else
        {
            cout << "(" << IDword[i] << ", " << IDbit[i] << ")⊕";
        }
    }
    cout << "            OD = ";
    for (int i{0}, n = sizeof(ODword) / sizeof(ODword[0]); i < n; ++i)
    {
        if (i == n - 1)
            cout << "(" << ODword[i] << ", " << ODbit[i] << ")\n";
        else
        {
            cout << "(" << ODword[i] << ", " << ODbit[i] << ")⊕";
        }
    }
    cout << "            # of bits that are not significant: " << PNBcount
         << "\n";
    cout << "            per loop size : 2^{" << log2(loopLim) << "}\n";
    cout << "***********************************************************************************\n";
    // ---------------------------Printing ends--------------------------------------------------------------------

    FORWARD fwd;
    BACKWARD bwd;
    INIT_IV init_iv;
    INIT_KEY init_key;
    INSERT_KEY insert_key;
    PUT_VALUE pv;

    u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16], loop{0};
    u16 fwBit{0}, bwBit{0};

    double count{0};

    while (loop < ITERATION)
    {
        init_iv.chacha(x0);
        init_key.Key256(key);
        insert_key.chacha(x0, key);

        CopyState(strdx0, x0, 16);
        // ---------------------------FW ROUND STARTS--------------------------------------------------------------------
        for (int i{1}; i <= fwdround; ++i)
            fwd.RoundFunction(x0, i);

        fwBit = (x0[2] ^ (x0[7] >> 7) ^ x0[8]) & 1; // Eurocrypt (4 rounds fw)

        for (int i{fwdround + 1}; i <= totalround; ++i)
           fwd.RoundFunction(x0, i);
        // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

        // modular addition of two states
        AddStates(x0, strdx0);
    
        pv.chacha(strdx0, dstrdx0, PNB, PNBcount, "random");

        // modular subtraction of states
        SubtractStates(x0, strdx0);

        // ---------------------------BW ROUND STARTS--------------------------------------------------------------------
        for (int i{totalround}; i > fwdround; i--)
            bwd.RoundFunction(x0, i);
        // ---------------------------BW ROUND ENDS----------------------------------------------------------------------
        bwBit = (x0[2] ^ (x0[7] >> 7) ^ x0[8]) & 1; // Eurocrypt (4 rounds fw)

        // epsilon_a find
        if (fwBit == bwBit)
            count++;
        loop++;

        if (loop > 0 && loop % (loopLim) == 0)
            printf("🔢 # loop: %lld | 🎆bias: %.7lf ~ %.4lf ~ 2^{%.2f} \n", counter++, (2 * (count / loop) - 1.0), (2 * (count / loop) - 1.0), log2(fabs(2 * (count / loop) - 1.0)));
    }
}