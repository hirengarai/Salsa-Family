/*
 * REFERENCE IMPLEMENTATION OF verification of backward bias (epsilon_a) searching programme
 *
 * Filename: verifycbwbias.cpp
 *
 * created: 24/09/23
 * updated: 02/02/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the verification of the backward bias searching programme for the stream cipher ChaCha.
 * running command: g++ verifycbwbias.cpp && ./a.out
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

constexpr double BIAS = 0.00057; // only needed if you want to verify the bias
bool closeflag = false;
// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}; // Aumasson et al.
// int ODword[] = { 2,6,6,10,14 }, ODbit[] = { 0,7,19,12,0};
// int ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};

// eurocrypt ID-OD
u16 ID[][2] = {{13, 6}}, OD[][2] = {{2, 0}, {7, 7}, {8, 0}};
ull loopcounter = 0;

int main(int argc, char *argv[])
{
  auto mainstart = chrono::high_resolution_clock::now();

  // std::time_t start_time = std::chrono::system_clock::to_time_t(mainstart);
  // cout << "########## Execution started on: " << ctime(&start_time);
  // std::cout << local << '\n'; // 2021-05-03 15:02:44.130182 EDT
  // std::cout << local << '\n'; // 2021-05-03 15:02:44.130182 EDT

  time_t t;
  tm *lt;             // Declares a pointer lt to a structure of type tm. The tm structure holds information about date and time
  time(&t);           // Gets the current time since epoch (Jan 1, 1970)
  lt = localtime(&t); // Converts the time stored in t to a local time representation and stores it in the tm structure pointed to by lt.

  long timeZoneOffset = lt->tm_gmtoff;   // Adjust the time according to the local time zone
  time_t localTime = t + timeZoneOffset; // Adjust the time according to the local time zone

  lt = localtime(&localTime); // Convert local time to tm structure
  ofstream file("bias.txt");
  if (file.is_open())
    file << "######## Execution started on: "
         << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
         << lt->tm_hour + 8 << ':' << lt->tm_min + 23 << ':' << lt->tm_sec
         << " ########\n";
  cout << "######## Execution started on: "
       << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
       << lt->tm_hour + 8 << ':' << lt->tm_min + 23 << ':' << lt->tm_sec
       << " ########\n";

  basicdetails.cipher = "ChaCha-256";
  basicdetails.programtype = "Backward Bias Determiantion";
  basicdetails.totalround = 7;

  diffdetails.fwdround = 4;

  sampledetails.samplesperLoop = pow(2, 20);
  sampledetails.totalLoop = pow(2, 14);
  ull LIMIT = sampledetails.samplesperLoop * sampledetails.totalLoop;

  PNBdetails.PNBfile = "europnb.txt";
  PNBdetails.PNB = OpenFile("europnb.txt", &PNBdetails.PNBsize);

  diffdetails.ID = ID;
  diffdetails.OD = OD;
  diffdetails.IDsize = sizeof(ID) / sizeof(ID[0]);
  diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);

  PrintBasicDetails(basicdetails, cout);
  PrintDiffDetails(diffdetails, cout);
  PrintPNBDetails(PNBdetails, cout);
  PrintSampleDetails(sampledetails, cout);
  PrintBiasLoopEtc(cout);

  // PrintBasicDetails(basicdetails, file);
  // PrintDiffDetails(diffdetails, file);
  // PrintPNBDetails(PNBdetails, file);
  // PrintSampleDetails(sampledetails, file);
  // PrintBiasLoopEtc(file);

  // DetailsParams dparams;
  // dparams.cipher = "ChaCha";
  // dparams.progtype = "BW bias finding";
  // dparams.totalround = 7;
  // dparams.fwdround = 4;
  // dparams.IDbit = IDbit;
  // dparams.IDword = IDword;
  // dparams.IDsize = sizeof(IDword) / sizeof(IDword[0]);
  // dparams.ODbit = ODbit;
  // dparams.ODword = ODword;
  // dparams.ODsize = sizeof(ODword) / sizeof(ODword[0]);
  // dparams.totalLoopCount = pow(2, 13);
  // dparams.samplesPerloop = pow(2, 20);
  // PNBValueParams pvparams;
  // Details(dparams, cout);
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

  // u16 PNB[] = {2, 3, 4, 5, 6, 39, 47, 48, 66, 67, 68, 69, 70, 71, 72, 73, 74, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 90, 91, 92, 95, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 123, 124, 125, 126, 127, 155, 156, 157, 158, 159, 168, 191, 192, 193, 194, 199, 200, 201, 207, 208, 211, 212, 213, 219, 220, 221, 222, 223, 224, 225, 226, 227, 244, 245, 246, 255};

  // PNBdetails.PNB = PNB;
  // PNBdetails.PNBsize = sizeof(PNB) / sizeof(PNB[0]);

  u32 x0[WORD_COUNT], strdx0[WORD_COUNT], key[KEY_COUNT], dx0[WORD_COUNT], dstrdx0[WORD_COUNT], DiffState[WORD_COUNT];
  u16 fwdBit, bwdBit, apprxbiascounter{0}, apprxbiasindex{0}, continuitycount{500};
  ull loop{0};
  double matchcount{0}, apprxbiaslist[continuitycount], biaslimit{0.00004};

  FORWARD fwd;
  BACKWARD bwd;

  while (loop < LIMIT)
  {
    auto loopstart = chrono::high_resolution_clock::now();

    inisialize(x0);
    init_key.Key256(key);
    insertkey(x0, key);

    CopyState(strdx0, x0);
    CopyState(dx0, x0);

    // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
    // u16 index{0};
    // for (const u16 &i : IDword)
    // {
    //   InputDifference(dx0, i, IDbit[index]);
    //   index++;
    // }

    for (const auto &row : ID)
    {
      InputDifference(x0, row[0], row[1]);
    }
    // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

    CopyState(dstrdx0, dx0);

    for (int i{1}; i <= diffdetails.fwdround; ++i)
    {
      fwd.RoundFunction(x0, i);
      fwd.RoundFunction(dx0, i);
    }

    XORDifference(DiffState, WORD_COUNT, x0, dx0);

    fwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

    for (int i{diffdetails.fwdround + 1}; i <= basicdetails.totalround; ++i)
    {
      fwd.RoundFunction(x0, i);
      fwd.RoundFunction(dx0, i);
    }
    // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

    // modular addition
    AddStates(x0, strdx0);
    AddStates(dx0, dstrdx0);

    PNBdetails.x = strdx0;
    PNBdetails.x1 = dstrdx0;

    // randomise the PNBs
    valuePNB(PNBdetails);

    // modular subtraction of states
    SubtractStates(x0, strdx0);
    SubtractStates(dx0, dstrdx0);

    // ---------------------------BW ROUND STARTS--------------------------------------------------------------------

    for (int i{basicdetails.totalround}; i > diffdetails.fwdround; i--)
    {
      bwd.RoundFunction(x0, i);
      bwd.RoundFunction(dx0, i);
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

      double varepsilon = 2 * (matchcount / (double)loop) - 1.0;

      closeflag = false;
      if (fabs(varepsilon - BIAS) < biaslimit)
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

      cout << "|" << setw(10) << ++loopcounter << setw(10) << "|"
           << (varepsilon >= 0 ? (setw(10)) : setw(10)) << fixed << setprecision(6) << varepsilon << setw(3) << " ~ "
           << setw(5) << setprecision(5) << fabs(varepsilon) << setw(6)
           << setprecision(2) << " ~ 2^{" << setw(6) << log2(fabs(varepsilon)) << setw(1) << "}" << setw(3)
           << "|" << setw(15) << setprecision(2)
           << (chrono::duration<double, std::micro>(loopend - loopstart).count()) << setw(15) << "| ";

      cout << ((closeflag) ? "✅" : "❌") << ((closeflag) ? apprxbiascounter : 0) << "\n";

      if (apprxbiascounter >= continuitycount)
      {
        if (file.is_open())
        {
          file << "Median Bias ~ " << fixed << setprecision(5) << CalculateMedian(apprxbiaslist, sizeof(apprxbiaslist) / sizeof(apprxbiaslist[0])) << "\n";
          file << "The latest " << continuitycount << " biases are as follows:\n";
          for (const auto &i : apprxbiaslist)
            file << i << "\n";

          file << "Number of loops: " << loopcounter << "\n";
        }
        break;
      }
        }
    if (loop == sampledetails.samplesperLoop * sampledetails.totalLoop)
    {
      cout << "The bias does not converge\n";
      file << "The bias does not converge\n";
    }
  }
  auto mainend = chrono::high_resolution_clock::now();
  auto dura = std::chrono::duration<double, std::micro>(mainend - mainstart).count();
  cout << "Total exec. time: " << (dura / 1000000.0) << "seconds ~ " << dura / 60000000 << " minutes ~ " << fixed << setprecision(3) << dura / 3600000000 << " hours\n";
  file << "Total exec. time: " << (dura / 1000000.0) << "seconds ~ " << dura / 60000000 << " minutes ~ " << fixed << setprecision(3) << dura / 3600000000 << " hours\n";

  time(&t);           // Gets the current time since epoch (Jan 1, 1970)
  lt = localtime(&t); // Converts the time stored in t to a local time representation and stores it in the tm structure pointed to by lt.
  // long timeZoneOffset = lt->tm_gmtoff;
  // time_t localTime = t + timeZoneOffset;
  // lt = localtime(&localTime);

  cout << "######## Execution ended on: "
       << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
       << lt->tm_hour + 8 << ':' << lt->tm_min + 23 << ':' << lt->tm_sec
       << " ########\n";
  file << "######## Execution ended on: "
       << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
       << lt->tm_hour + 8 << ':' << lt->tm_min + 23 << ':' << lt->tm_sec
       << " ########\n";
  file.close();

  return 0;
}
