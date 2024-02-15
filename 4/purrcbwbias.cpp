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

#include "chacha1.h"
#include <cmath>  // pow function
#include <ctime>  // time
#include <chrono> // time

using namespace std;

// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}; // Aumasson et al.
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
  dparams.totalLoopCount = pow(2, 33);
  dparams.samplesPerloop = pow(2, 20);
  PutValueParams pvparams;
  Details(dparams, cout);
  // --------------------- input the PNB file with PNB size as the last element ----------------------------
  // if (argc > 1)
  // {
  //   pvparams.mode = "block";
  //   cout << "                 Block mode \n";
  //   u16 **BLOCK;
  //   BLOCK = OpenFile("pnbblock.txt", &pvparams.list1size, &pvparams.list2size);
  //   cout << "|            # of PNBs in block form: " << pvparams.list1size << ", # of rest of PNBs: " << pvparams.list2size
  //        << "                 " << setw(7) << "|\n";
  //   cout << "+------------------------------------------------------------------------------------+\n";
  //   pvparams.list1 = BLOCK[0];
  //   pvparams.list2 = BLOCK[1];
  // }
  // else
  // {
  //   pvparams.y = OpenFile("europnb.txt", &dparams.PNBcount);
  //   pvparams.ysize = dparams.PNBcount;
  //   cout << "+------------------------------------------------------------------------------------+\n";
  // }

  u16 PNB[] = {2, 3, 4, 5, 6, 39, 47, 48, 66, 67, 68, 69, 70, 71, 72, 73, 74, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 90, 91, 92, 95, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 123, 124, 125, 126, 127, 155, 156, 157, 158, 159, 168, 191, 192, 193, 194, 199, 200, 201, 207, 208, 211, 212, 213, 219, 220, 221, 222, 223, 224, 225, 226, 227, 244, 245, 246, 255};
  pvparams.y = PNB;
  dparams.PNBcount = sizeof(PNB) / sizeof(PNB[0]);
  pvparams.ysize = dparams.PNBcount;

  u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16];
  u16 fwdBit, bwdBit;
  ull loop{0};
  double count{0};
  cout << WORD_SIZE % (8 * sizeof(u32)) << "\n";
  FORWARD fwd;
  BACKWARD bwd;
  INITIALISE_IV init_iv;
  INITIALISE_KEY init_key;
  INSERT_KEY insert_key;
  STATES state;
  PUT_VALUE pv;

  while (loop < dparams.totalLoopCount)
  {
    auto start = chrono::high_resolution_clock::now();
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

    pvparams.x = strdx0;
    pvparams.x1 = dstrdx0;

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
    state.AddStates(x0, strdx0);
    state.AddStates(dx0, dstrdx0);

    // randomise the PNBs
    pv.chacha(pvparams);

    // modular subtraction of states
    state.SubtractStates(x0, strdx0);
    state.SubtractStates(dx0, dstrdx0);

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
      auto end = chrono::high_resolution_clock::now();
      double varepsilon = 2 * (count / (double)loop) - 1.0;
      cout << "🔢 #loop : " << setw(4) << ++counter << " | 🎆bias : " << fixed << setprecision(6) << varepsilon << " ~ " << setprecision(4) << fabs(varepsilon) << setprecision(2) << " ~ 2 ^{" << log2(fabs(varepsilon)) << "}"
           << "Time taken: " << setprecision(4) << chrono::duration<double, std::micro>(end - start).count() << " microsec\n";
    }
  }
}
