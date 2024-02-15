/*
 * REFERENCE IMPLEMENTATION OF backward bias epsilon_a searching programme.
 *
 * Filename: cbwbias.cpp
 *
 * created: 24/09/23
 * updated: 19/12/23
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
#include <cmath>   // pow function
#include <ctime>   // time

using namespace std;

// DIFFFERENTIAL ATTACK PARAMETERS
// Change these parameters
char cipher[8] = "ChaCha";
u16 totalround = 7;
char mode[] = "random"; // "random" / "block" / "0value"
ull ITERATION = pow(2, 62), loopLim = pow(2, 21), counter{0};
u16 *PNB, **BLOCK;
size_t PNBcount, block_size, rest_size;

// Aumasson et al.
u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}, fwdround{3}, IDsize{1}, ODsize{1}; // Aumasson et al.
// Aumasson et al.

// new 5 round dist. of 23
// u16 IDword[] = {15, 15}, IDbit[] = {7, 19}, ODword[] = {2, 6, 6, 10, 14}, ODbit[] = {0, 7, 19, 12, 0}, fwdround{5};
// new 5 round dist. of 23

// // 4 round eurocrypt
// u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0}, fwdround{4}, IDsize{1}, ODsize{3};
// // eurocrypt

int main()
{
  srand48(time(NULL));

  // ---------------------------Printing--------------------------------------------------------------------
  Details(cipher, totalround, fwdround, IDword, IDbit, IDsize, ODword, ODbit, ODsize, PNBcount, mode, loopLim, ITERATION);
  // ---------------------------Printing ends---------------------------------------------------------------

  // --------------------- input the PNB file with PNB size as the last element ----------------------------
  if (!(strcasecmp(mode, "random")) || !(strcasecmp(mode, "0value")))
  {
    PNB = OpenFile("aumasson.txt", &PNBcount);
    cout << "|            # of bits that are not significant (PNB): " << PNBcount
         << "                       "<<setw(7)<<"|\n";
  }
  else
  {
    BLOCK = OpenFile("europnbblock.txt", &block_size, &rest_size);
    cout << "|            # of PNBs in block form: " << block_size << ", # of rest of PNBs: " << rest_size
         << "                 "<<setw(7)<<"|\n";
  }
  std::cout << "+------------------------------------------------------------------------------------+\n";

  FORWARD fwd;
  BACKWARD bwd;
  INIT_IV init_iv;
  INIT_KEY init_key;
  INSERT_KEY insert_key;
  PUT_VALUE pv;

  u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16];
  u16 fwBit, bwBit;
  ull loop{0};

  double count{0};

  while (loop < ITERATION)
  {
    init_iv.chacha(x0);
    init_key.Key256(key);
    insert_key.chacha(x0, key);

    CopyState(strdx0, x0, 16);
    CopyState(dx0, x0, 16);

    // ----------------------------------DIFF. INJECTION--------------------------------------------------------------

    // InputDifference(dx0, IDword[0], IDbit[0]);
    u16 i{0};
    for (u16 &j : IDword)
    {
      InputDifference(dx0, j, IDbit[i]);
      i++;
    }

    CopyState(dstrdx0, dx0, 16);

    // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

    for (int i{1}; i <= fwdround; ++i)
    {
      fwd.RoundFunction(x0, i);
      fwd.RoundFunction(dx0, i);
    }

    XORDifference(x0, dx0, DiffState, 16);
    // fwBit = DiffState[2] ^ (DiffState[6] >> 7) ^ (DiffState[6] >> 19) ^ (DiffState[10] >> 12) ^ DiffState[14]; // new dist. (5 rounds fw)
    // fwBit = DiffState[1] ^ (DiffState[6] >> 7) ^ DiffState[11]; // Eurocrypt (4 rounds fw)
    fwBit = (DiffState[2] ^ (DiffState[7] >> 7) ^ DiffState[8]) & 1; // Eurocrypt (4 rounds fw)

    //--------------------------------------------+
    // i = 0;                                     -
    // for (u16 &j : ODword)                      -
    // {                                          -
    //     temp ^= (DiffState[j] >> ODbit[i]);    -
    //     i++;                                   -
    // }                                          -
    // fwBit = temp;                              -
    //--------------------------------------------+

    for (int i{fwdround + 1}; i <= totalround; ++i)
    {
      fwd.RoundFunction(x0, i);
      fwd.RoundFunction(dx0, i);
    }

    // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

    // modular addition of two states
    AddStates(x0, strdx0);
    AddStates(dx0, dstrdx0);

    // randomise the PNBs
    if (!(strcasecmp(mode, "block")))
      BlockValue(strdx0, dstrdx0, BLOCK[0], BLOCK[1], block_size, rest_size);
    else if (!(strcasecmp(mode, "random")))
      pv.chacha(strdx0, dstrdx0, PNB, PNBcount, "random");
    else if (!(strcasecmp(mode, "0value")))
      pv.chacha(strdx0, dstrdx0, PNB, PNBcount, "zero");

    // modular subtraction of states
    SubtractStates(x0, strdx0);
    SubtractStates(dx0, dstrdx0);

    // ---------------------------BW ROUND STARTS--------------------------------------------------------------------

    for (int i{totalround}; i > fwdround; i--)
    {
      bwd.RoundFunction(x0, i);
      bwd.RoundFunction(dx0, i);
    }

    // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

    XORDifference(x0, dx0, DiffState, 16);
    //--------------------------------------------+
    // i = 0;                                     -
    // for (u16 &j : ODword)                      -
    // {                                          -
    //     temp1 ^= (DiffState[j] >> ODbit[i]);   -
    //     i++;                                   -
    // }                                          -
    // bwBit = temp1;                             -
    //--------------------------------------------+
    // bwBit = DiffState[1] ^ (DiffState[6] >> 7) ^ DiffState[11]; // Eurocrypt (4 rounds fw)
    bwBit = (DiffState[2] ^ (DiffState[7] >> 7) ^ DiffState[8]) & 1; // Eurocrypt (4 rounds fw)
    // bwBit = DiffState[2] ^ (DiffState[6] >> 7) ^ (DiffState[6] >> 19) ^ (DiffState[10] >> 12) ^ DiffState[14]; // new dist. (5 rounds fw)

    // epsilon_a find
    if (fwBit == bwBit)
      count++;
    loop++;

    if (loop > 0 && loop % (loopLim) == 0)
    {
      double varepsilon = 2 * (count / loop) - 1.0;
      cout << "🔢 #loop : " << setw(4) << ++counter << " | 🎆bias : " << fixed << setprecision(7) << varepsilon << " ~ " << setprecision(5) << fabs(varepsilon) << setprecision(2) << " ~ 2 ^{" << log2(fabs(varepsilon)) << "}\n";
    }
  }
}
