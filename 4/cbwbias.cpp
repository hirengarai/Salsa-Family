/*
 * REFERENCE IMPLEMENTATION OF of backward bias (epsilon_a) searching programme
 *
 * Filename: cbwbias.cpp
 *
 * created: 24/09/23
 * updated: 11/02/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the backward bias searching programme for the stream cipher ChaCha.
 * running command: g++ cbwbias.cpp && ./a.out
 * necessary files to run the prog: commonfiles.h, chacha.h, one txt file containg the PNBs
 */

#include "chacha.h"
#include <cmath>   // pow function
#include <ctime>   // time
#include <chrono>  // time
#include <fstream> // file

using namespace std;
using namespace ChaCha;
using namespace Parameters;
using namespace Operation;

// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}; // Aumasson et al.
// int ODword[] = { 2,6,6,10,14 }, ODbit[] = { 0,7,19,12,0};
// int ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};

// eurocrypt ID-OD
u16 ID[][2] = {{13, 6}}, OD[][2] = {{2, 0}, {7, 7}, {8, 0}};
ull loopcounter = 0;

int main(int argc, char *argv[])
{
  auto mainstart = chrono::high_resolution_clock::now();

  time_t t;
  tm *lt;             // Declares a pointer lt to a structure of type tm. The tm structure holds information about date and time
  time(&t);           // Gets the current time since epoch (Jan 1, 1970)
  lt = localtime(&t); // Converts the time stored in t to a local time representation and stores it in the tm structure pointed to by lt.

  cout << "######## Execution started on: "
       << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
       << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
       << " ########\n";

  basicdetails.cipher = "ChaCha-256";
  basicdetails.programtype = "Backward Bias Determiantion";
  basicdetails.totalround = 7;

  diffdetails.fwdround = 4;

  sampledetails.samplesperLoop = pow(2, 18);
  sampledetails.totalLoop = pow(2, 5);
  ull LIMIT = sampledetails.samplesperLoop * sampledetails.totalLoop;

  // PNBdetails.PNBblockflag = true;
  PNBdetails.PNBfile = "europnb.txt";
  PNBdetails.PNB = OpenFile("europnb.txt", &PNBdetails.PNBsize);

  // PNBdetails.PNBinblock = {};
  // PNBdetails.restPNB = {};

  // PNBdetails.PNBinblocksize = sizeof(PNBdetails.PNBinblock) / sizeof(PNBdetails.PNBinblock[0]);
  // PNBdetails.restPNBsize = sizeof(PNBdetails.restPNB) / sizeof(PNBdetails.restPNB[0]);

  diffdetails.ID = ID;
  diffdetails.OD = OD;
  diffdetails.IDsize = sizeof(ID) / sizeof(ID[0]);
  diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);

  PrintBasicDetails(basicdetails, cout);
  PrintDiffDetails(diffdetails, cout);
  PrintPNBDetails(PNBdetails, cout);
  PrintSampleDetails(sampledetails, cout);
  PrintBiasLoopEtc(cout);

  u32 x0[WORD_COUNT], strdx0[WORD_COUNT], key[KEY_COUNT], dx0[WORD_COUNT], dstrdx0[WORD_COUNT], DiffState[WORD_COUNT];
  u16 fwdBit, bwdBit;
  ull loop{0};
  double matchcount{0};

  while (loop <= LIMIT)
  {
    auto loopstart = chrono::high_resolution_clock::now();

    inisialize(x0);
    init_key.Key256(key);
    insertkey(x0, key);

    // prntstate.x = x0;
    // PrintState(prntstate);

    CopyState(strdx0, x0);
    CopyState(dx0, x0);

    // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
    for (const auto &row : ID)
      InputDifference(dx0, row[0], row[1]);
    // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

    CopyState(dstrdx0, dx0);

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

    // prntstate.x = strdx0;
    // prntstate.binaryform = true;
    // PrintState(prntstate);

    PNBdetails.x = strdx0;
    PNBdetails.x1 = dstrdx0;

    // randomise the PNBs
    PNBdetails.sure_PNBrandomvaluechangeflag = true;
    valuePNB(PNBdetails);
    
    // for (int i{0}; i < PNBdetails.PNBsize; ++i)
    // {
    //   int WORD = (PNBdetails.PNB[i] / WORD_SIZE) + 4;
    //   int BIT = PNBdetails.PNB[i] % WORD_SIZE;

    //   if (GenerateRandomBoolean())
    //   {
    //     ToggleBit(strdx0[WORD], BIT);
    //     ToggleBit(dstrdx0[WORD], BIT);
    //   }
    // }

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

    // epsilon_a find
    if (fwdBit == bwdBit)
      matchcount++;
    loop++;

    if (loop > 0 && loop % (sampledetails.samplesperLoop) == 0)
    {
      auto loopend = chrono::high_resolution_clock::now();
      cout << "matchcount = " << matchcount << "\n";
      double varepsilon = 2 * (matchcount / (double)loop) - 1.0;

      cout << "|" << setw(10) << ++loopcounter << setw(10) << "|"
           << (varepsilon >= 0 ? (setw(10)) : setw(10)) << fixed << setprecision(6) << varepsilon << setw(3) << " ~ "
           << setw(5) << setprecision(5) << fabs(varepsilon) << setw(6)
           << setprecision(2) << " ~ 2^{" << setw(6) << log2(fabs(varepsilon)) << setw(1) << "}" << setw(3)
           << "|" << setw(15) << setprecision(2)
           << (chrono::duration<double, std::micro>(loopend - loopstart).count()) << setw(15) << "|\n";
    }
  }
  if (loop == sampledetails.samplesperLoop * sampledetails.totalLoop)
    cout << "The bias does not converge\n";
  auto mainend = chrono::high_resolution_clock::now();
  auto dura = std::chrono::duration<double, std::micro>(mainend - mainstart).count();
  cout << "Total exec. time: " << (dura / 1000000.0) << "seconds ~ " << dura / 60000000 << " minutes ~ " << fixed << setprecision(3) << dura / 3600000000 << " hours\n";

  time(&t);           // Gets the current time since epoch (Jan 1, 1970)
  lt = localtime(&t); // Converts the time stored in t to a local time representation and stores it in the tm structure pointed to by lt.

  cout << "######## Execution ended on: "
       << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
       << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
       << " ########\n";

  return 0;
}
