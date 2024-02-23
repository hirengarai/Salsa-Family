/*
 * REFERENCE IMPLEMENTATION OF PNB searching programe.
 *
 * Filename: aumpnb.cpp
 *
 * created: 30/12/23
 * updated: 11/02/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the PNB searching programe for the stream cipher Salsa/ ChaCha.
 * running command: g++ aumpnb.cpp && ./a.out
 */

#include "chacha.h"      // chacha round functions
#include <cmath>         // pow function
#include <cstring>       // string
#include <ctime>         // time
#include <fstream>       // storing output in a file
#include <future>        // multithreading
#include <iomanip>       // decimal numbers upto certain places
#include <thread>        // multithreading
#include <unordered_map> // multithreading
#include <vector>        // multithreading

using namespace std;
using namespace ChaCha;
using namespace Parameters;
using namespace Operation;

// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}, fwdround{3}, totalround{6}; // Aumasson et al.
// int ODword[] = { 2,6,6,10,14 }, ODbit[] = { 0,7,19,12,0};
// int ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};

// eurocrypt ID-OD
// u16 ID[][2] = {{13, 6}}, OD[][2] = {{2, 0}, {7, 7}, {8, 0}};

// aumasson ID-OD
u16 ID[][2] = {{13, 13}}, OD[][2] = {{11, 0}};

double findPNB(u16 keyWord, u16 keyBit, promise<double> &&list);

int main(int argc, char *argv[])
{
    time_t t = time(nullptr);
    tm *lt = localtime(&t); // Declares a pointer lt to a structure of type tm. The tm structure holds information about date and time

    cout << "######## Execution started on: "
         << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
         << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
         << " ########\n";

    basicdetails.cipher = "ChaCha-256";
    basicdetails.programtype = "Backward Bias Determiantion";
    basicdetails.totalround = 7;

    diffdetails.fwdround = 3;

    sampledetails.samplesperThread = pow(2, 17);
    u16 max_num_threads = pow(2, 3);
    sampledetails.samplesperLoop = sampledetails.samplesperThread * max_num_threads;

    diffdetails.ID = ID;
    diffdetails.OD = OD;
    diffdetails.IDsize = sizeof(ID) / sizeof(ID[0]);
    diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);

    PNBdetails.neutralitymeasure = pow(2, -1);

    PrintBasicDetails(basicdetails, cout);
    PrintDiffDetails(diffdetails, cout);
    PrintPNBDetails(PNBdetails, cout);
    PrintSampleDetails(sampledetails, cout);

    // ---------------------------FILE CREATION------------------------------------------------------------------------------
    char filename[90], dfilename[100];

    // Format the date and time in YYYYMMDD_HHMMSS format
    strftime(filename, sizeof(filename), "%M%H%S_%d%m%Y", lt);
    sprintf(filename + strlen(filename), "_auma_%.2lf_pnbs_%s_%i_id_%i_%i.txt", PNBdetails.neutralitymeasure, basicdetails.cipher.c_str(), basicdetails.totalround, ID[0][0], ID[0][1]);

    sprintf(dfilename, "detailed_%s", filename); // Add your desired extra character

    ofstream dfile(dfilename);
    if (dfile.is_open())
    {
        dfile << "######## Execution started on: "
              << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
              << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
              << " ########\n";
        dfile << "The file contains the detailed PNBs from the file " << filename << "\n\n";
    }

    // time calculation starts :
    auto start = chrono::high_resolution_clock::now();

    // thread portion started
    // u16 max_num_threads = KEY_COUNT;
    cout << "# of threads: " << max_num_threads << "\n";
    cout << "PNB Calculation Started . . . (Shh! It's a multi-threaded programme and will take some time !) \n";
    cout << "Have patience with a cup of tea . . . \n";
    cout << "+------------------------------------------------------------------------------------+\n";

    thread threadPool[max_num_threads];
    future<double> futureCounts[max_num_threads];
    double SUM, nm;
    unordered_map<u16, double> detailedPNBlist;
    unordered_map<u16, double> detailednonPNBlist;
    vector<u16> PNBlist;
    vector<u16> nonPNBlist;
    // vector<u16> detailedPNBlist;
    // vector<double> biasList;
    // vector<u16> nonPNBlist;
    // vector<double> nonPNBbiaslist;
    for (u16 keyWord{0}; keyWord < KEY_COUNT; ++keyWord)
    {
        for (u16 keyBit{0}; keyBit < WORD_SIZE; ++keyBit)
        {
            SUM = 0;
            for (int i{0}; i < max_num_threads; ++i)
            {
                promise<double> prms;

                futureCounts[i] = prms.get_future();
                threadPool[i] = thread(&findPNB, keyWord, keyBit, move(prms));
                // cout << "Thread id at " << i << " : " << threadPool.at(i).get_id() <<"\n";
            }
            for (int i{0}; i < max_num_threads; ++i)
                threadPool[i].join();

            for (int i{0}; i < max_num_threads; ++i)
                SUM += futureCounts[i].get();
            nm = 2 * SUM / (sampledetails.samplesperLoop) - 1.0;
            if (nm >= PNBdetails.neutralitymeasure)
                detailedPNBlist.insert({(keyWord * 32) + keyBit, nm});
            else
                detailednonPNBlist.insert({(keyWord * 32) + keyBit, nm});
        }
        dfile << "****************************** Keyword: " << keyWord << " *********************************************\n";
        dfile << "*************** PNBs (" << detailedPNBlist.size() << ") ***************\n";
        for (auto &l : detailedPNBlist)
        {
            PNBlist.push_back(l.first);
            dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
        }

        dfile << "*************** Non-PNBs (" << detailednonPNBlist.size() << ") ***************\n";
        for (auto &l : detailednonPNBlist)
        {
            nonPNBlist.push_back(l.first);
            dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
        }
        dfile << "*------------------------------------------------------------------------------------*\n";
        detailedPNBlist.clear();
        detailednonPNBlist.clear();
    }



    // for (int i{0}; i < max_num_threads; ++i)
    // {
    //     perThreadList.at(i) = futuResults[i].get();
    //     dfile << "****************************** Keyword: " << i << " *********************************************\n";
    //     dfile << "*************** PNBs (" << perThreadList.at(i).first.size() << ") ***************\n";
    //     for (auto &l : perThreadList.at(i).first)
    //     {
    //         detailedPNBlist.push_back(l.first);
    //         biasList.push_back(l.second);
    //         dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
    //     }
    //     dfile << "*************** Non-PNBs (" << perThreadList.at(i).second.size() << ") ***************\n";
    //     for (auto &l : perThreadList.at(i).second)
    //     {
    //         nonPNBlist.push_back(l.first);
    //         nonPNBbiaslist.push_back(l.second);
    //         dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
    //     }
    //     dfile << "*------------------------------------------------------------------------------------*\n";
    // }
    dfile << "Total non-PNBs: " << nonPNBlist.size() << "\n";
    dfile << "Total PNBs: " << PNBlist.size() << "\n\n";

    dfile << "The " << PNBlist.size() << " PNBs are as follows:\n";
    ofstream file(filename);
    sort(PNBlist.begin(), PNBlist.end());

    if (file.is_open() && dfile.is_open())
    {
        bool flag = true;
        dfile << "{";
        for (const int &n : PNBlist)
        {
            file << n << " ";
            if (!flag)
                dfile << ", ";
            dfile << n;
            flag = false;
        }
        dfile << "}\n";
        file << PNBlist.size();
    }

    // BiasSrotedPrint(biasList, pnbList, dfile);

    file.close();

    auto end = chrono::high_resolution_clock::now();
    cout << "Finished.\n";

    time(&t);           // Gets the current time since epoch (Jan 1, 1970)
    lt = localtime(&t); // Converts the time stored in t to a local time representation and stores it in the tm structure pointed to by lt.
    cout << "######## Execution ended on: "
         << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
         << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
         << " ########\n";

    cout << "Total execution time: " << chrono::duration<double, std::milli>(end - start).count() << " milliseconds"
         << " ~ " << chrono::duration_cast<chrono::seconds>(end - start).count() << " seconds"
         << " ~ " << chrono::duration_cast<chrono::minutes>(end - start).count() << " minutes.\n";
    if (dfile.is_open())
    {
        dfile << "######## Execution ended on: "
              << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
              << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
              << " ########\n";

        dfile << "Total execution time: " << chrono::duration<double, std::milli>(end - start).count() << " milliseconds"
              << " ~ " << chrono::duration_cast<chrono::seconds>(end - start).count() << " seconds"
              << " ~ " << chrono::duration_cast<chrono::minutes>(end - start).count() << " minutes.\n";
    }
    dfile.close();
}

double findPNB(u16 keyWord, u16 keyBit, promise<double> &&count)
{
    u32 x0[WORD_COUNT], strdx0[WORD_COUNT], key[KEY_COUNT], dx0[WORD_COUNT], dstrdx0[WORD_COUNT], DiffState[WORD_COUNT], threadloop{0};
    u16 fwdBit, bwdBit;

    double thread_match_count{0};

    while (threadloop < sampledetails.samplesperThread)
    {
        inisialize(x0);
        init_key.Key256(key);
        insertkey(x0, key);

        CopyState(strdx0, x0);
        CopyState(dx0, x0);
        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
        for (const auto &row : ID)
            InputDifference(dx0, row[0], row[1]);
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

        // modular addition of states
        AddStates(x0, strdx0);
        AddStates(dx0, dstrdx0);

        // altering key bit
        ToggleBit(key[keyWord], keyBit);

        // making new X and X' with altered keybits
        insertkey(strdx0, key);
        insertkey(dstrdx0, key);

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

        XORDifference(DiffState, 16, x0, dx0);

        bwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

        if (fwdBit == bwdBit)
            thread_match_count++;
        ++threadloop;
    }

    // Use std::move to transfer ownership of the pair to the promise
    count.set_value(move(thread_match_count));

    // Return the pair
    return thread_match_count;
}