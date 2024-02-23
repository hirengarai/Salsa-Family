/*
 * REFERENCE IMPLEMENTATION OF PNB searching programe.
 *
 * Filename: forroaumpnbthreaded.cpp
 *
 * created: 19/2/24
 * updated: 20/2/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the PNB searching programe for the stream cipher Salsa/ ChaCha.
 * running command: g++ forroaumpnbthreaded.cpp && ./a.out
 */

#include "forro.h"       // forro round functions
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
using namespace Forro;
using namespace Parameters;
using namespace Operation;

u16 ID[][2] = {{5, 17}};

pair<unordered_map<u16, double>, unordered_map<u16, double>> findPNB(u16 KWord, promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> &&list);

int main(int argc, char *argv[])
{
    time_t t = time(nullptr);
    tm *lt = localtime(&t); // Declares a pointer lt to a structure of type tm. The tm structure holds information about date and time

    cout << "######## Execution started on: "
         << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
         << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
         << " ########\n";

    basicdetails.cipher = "Forro";
    basicdetails.programtype = "PNB Determiantion";
    basicdetails.totalround = 6;

    diffdetails.fwdround = 2;

    sampledetails.samplesperLoop = pow(2, 18);

    diffdetails.ID = ID;
    // diffdetails.OD = OD;
    diffdetails.IDsize = sizeof(ID) / sizeof(ID[0]);
    // diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);
    diffdetails.halfroundflag = true;

    PNBdetails.neutralitymeasure = 0.2;

    PrintBasicDetails(basicdetails, cout);
    PrintDiffDetails(diffdetails, cout);
    PrintPNBDetails(PNBdetails, cout);
    PrintSampleDetails(sampledetails, cout);

    char filename[90], dfilename[100];
    // if (argc > 1)
    // {
    // ---------------------------FILE CREATION------------------------------------------------------------------------------

    // Format the date and time in YYYYMMDD_HHMMSS format
    // strftime(filename, sizeof(filename), "%d%m%Y_%H%M%S", lt);
    // sprintf(filename + strlen(filename), "%s_aum_%.2lf_%i_od_%i_%i_id_%i_%i.txt", basicdetails.cipher.c_str(), PNBdetails.neutralitymeasure, basicdetails.totalround, ID[0][0], ID[0][1]);

    // sprintf(dfilename, "detailed_%s", filename); // Add your desired extra character
    // }
    // ofstream dfile(dfilename);
    // if (dfile.is_open())
    // {
    //     dfile << "######## Execution started on: "
    //           << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
    //           << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
    //           << " ########\n";
    //     dfile << "The file contains the detailed PNBs from the file " << filename << "\n\n";
    // }

    // time calculation starts :
    auto start = chrono::high_resolution_clock::now();

    // thread portion started
    u16 max_num_threads = KEY_COUNT;
    cout << "# of threads: " << max_num_threads << "\n";
    cout << "PNB Calculation Started . . . (Shh! It's a multi-threaded programme and will take some time !) \n";
    cout << "Have patience with a cup of tea . . . \n";
    cout << "+------------------------------------------------------------------------------------+\n";

    thread threadPool[max_num_threads];
    vector<future<pair<unordered_map<u16, double>, unordered_map<u16, double>>>> futuResults(max_num_threads);
    vector<pair<unordered_map<u16, double>, unordered_map<u16, double>>> perThreadList(max_num_threads);
    vector<u16> pnbList;
    vector<double> biasList;
    vector<u16> nonPNBlist;
    vector<double> nonPNBbiaslist;

    for (u16 word{0}; word < WORD_COUNT; ++word)
    {
        pnbList.clear();

        u16 OD[][2] = {{word, 0}};
        diffdetails.OD = OD;
        diffdetails.ODsize = sizeof(OD) / sizeof(OD[0]);
        arrayprint.doubleprint(OD, diffdetails.ODsize, 2, "");
        // threads are called
        for (int i{0}; i < max_num_threads; ++i)
        {
            promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> prms;

            futuResults[i] = prms.get_future();
            threadPool[i] = thread(&findPNB, i, move(prms));
            // cout << "Thread id at " << i << " : " << threadPool.at(i).get_id() <<"\n";
        }

        for (int i{0}; i < max_num_threads; ++i)
            threadPool[i].join();
        // cout << "Thread id : " << threadPool.at(i).get_id() <<"\n";

        for (int i{0}; i < max_num_threads; ++i)
        {
            perThreadList.at(i) = futuResults[i].get();
            // if (dfile.is_open())
            // {
            //     dfile << "****************************** Keyword: " << i << " *********************************************\n";
            //     dfile << "*************** PNBs (" << perThreadList.at(i).first.size() << ") ***************\n";
            for (auto &l : perThreadList.at(i).first)
            {
                pnbList.push_back(l.first);
                biasList.push_back(l.second);
                // dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
            }
            //     dfile << "*************** Non-PNBs (" << perThreadList.at(i).second.size() << ") ***************\n";
            for (auto &l : perThreadList.at(i).second)
            {
                nonPNBlist.push_back(l.first);
                nonPNBbiaslist.push_back(l.second);
                // dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
            }
            //     dfile << "*------------------------------------------------------------------------------------*\n";
            // }
        }
        // if (dfile.is_open())
        // {
        //     dfile << "Total non-PNBs: " << nonPNBlist.size() << "\n";
        cout << "Total PNBs: " << pnbList.size() << "\n";
        cout << "------------------------------------------------------------------------------------\n";

        //     dfile << "The " << pnbList.size() << " PNBs are as follows:\n";
        // }
        // ofstream file(filename);
        // sort(pnbList.begin(), pnbList.end());

        // if (file.is_open() && dfile.is_open())
        // {
        //     bool flag = true;
        //     dfile << "{";
        //     for (const int &n : pnbList)
        //     {
        //         file << n << " ";
        //         if (!flag)
        //             dfile << ", ";
        //         dfile << n;
        //         flag = false;
        //     }
        //     dfile << "}\n";
        //     file << pnbList.size();
        // }

        // BiasSrotedPrint(biasList, pnbList, dfile);

        // file.close();
    }

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
    // if (dfile.is_open())
    // {
    //     dfile << "######## Execution ended on: "
    //           << lt->tm_mday << '/' << lt->tm_mon + 1 << '/' << lt->tm_year + 1900 << " at "
    //           << lt->tm_hour << ':' << lt->tm_min << ':' << lt->tm_sec
    //           << " ########\n";

    //     dfile << "Total execution time: " << chrono::duration<double, std::milli>(end - start).count() << " milliseconds"
    //           << " ~ " << chrono::duration_cast<chrono::seconds>(end - start).count() << " seconds"
    //           << " ~ " << chrono::duration_cast<chrono::minutes>(end - start).count() << " minutes.\n";
    // }
    // dfile.close();
}

pair<unordered_map<u16, double>, unordered_map<u16, double>> findPNB(u16 keyWord, promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> &&list)
{
    u32 x0[WORD_COUNT], strdx0[WORD_COUNT], key[KEY_COUNT], dx0[WORD_COUNT], dstrdx0[WORD_COUNT], DiffState[WORD_COUNT], loop;
    u16 fwdBit, bwdBit;

    unordered_map<u16, double> pnb;
    unordered_map<u16, double> nonpnb;

    double matchcount;

    for (int keyBit{0}; keyBit < WORD_SIZE; ++keyBit)
    {
        matchcount = 0.0;
        loop = 0;

        while (loop < sampledetails.samplesperLoop)
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
            // for (int i{1}; i <= diffdetails.fwdround; ++i)
            // {
            //     frward.RoundFunction(x0, i);
            //     frward.RoundFunction(dx0, i);
            // }

            // 1
            frward.ODDQ1(x0);
            frward.ODDQ1(dx0);

            frward.ODDQ2(x0);
            frward.ODDQ2(dx0);

            frward.ODDQ3(x0);
            frward.ODDQ3(dx0);

            frward.ODDQ4(x0);
            frward.ODDQ4(dx0);

            // 2
            frward.EVENQ1(x0);
            frward.EVENQ1(dx0);

            frward.EVENQ2(x0);
            frward.EVENQ2(dx0);

            frward.EVENQ3(x0);
            frward.EVENQ3(dx0);

            frward.EVENQ4(x0);
            frward.EVENQ4(dx0);

            XORDifference(DiffState, WORD_COUNT, x0, dx0);

            fwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

            // 2.5
            frward.ODDQ1(x0);
            frward.ODDQ1(dx0);

            frward.ODDQ2(x0);
            frward.ODDQ2(dx0);

            // 3
            frward.ODDQ3(x0);
            frward.ODDQ3(dx0);

            frward.ODDQ4(x0);
            frward.ODDQ4(dx0);

            // 4
            frward.EVENQ1(x0);
            frward.EVENQ1(dx0);

            frward.EVENQ2(x0);
            frward.EVENQ2(dx0);

            frward.EVENQ3(x0);
            frward.EVENQ3(dx0);

            frward.EVENQ4(x0);
            frward.EVENQ4(dx0);

            // 5
            frward.ODDQ1(x0);
            frward.ODDQ1(dx0);

            frward.ODDQ2(x0);
            frward.ODDQ2(dx0);

            frward.ODDQ3(x0);
            frward.ODDQ3(dx0);

            frward.ODDQ4(x0);
            frward.ODDQ4(dx0);

            // 6
            frward.EVENQ1(x0);
            frward.EVENQ1(dx0);

            frward.EVENQ2(x0);
            frward.EVENQ2(dx0);

            frward.EVENQ3(x0);
            frward.EVENQ3(dx0);

            frward.EVENQ4(x0);
            frward.EVENQ4(dx0);

            // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

            // modular addition of states
            AddStates(x0, strdx0);
            AddStates(dx0, dstrdx0);

            // altering key bit
            InputDifference(key, keyWord, keyBit);

            // making new X and X' with altered keybits
            insertkey(strdx0, key);
            insertkey(dstrdx0, key);

            // modular subtraction of states
            SubtractStates(x0, strdx0);
            SubtractStates(dx0, dstrdx0);

            // ---------------------------BW ROUND STARTS--------------------------------------------------------------------
            // for (int i{basicdetails.totalround}; i > diffdetails.fwdround + 1; i--)
            // {
            //     bckward.RoundFunction(x0, i);
            //     bckward.RoundFunction(dx0, i);
            // }
           
            // 6
            bckward.EVENQ1(x0);
            bckward.EVENQ1(dx0);

            bckward.EVENQ2(x0);
            bckward.EVENQ2(dx0);

            bckward.EVENQ3(x0);
            bckward.EVENQ3(dx0);

            bckward.EVENQ4(x0);
            bckward.EVENQ4(dx0);

            // 5
            bckward.ODDQ1(x0);
            bckward.ODDQ1(dx0);

            bckward.ODDQ2(x0);
            bckward.ODDQ2(dx0);

            bckward.ODDQ3(x0);
            bckward.ODDQ3(dx0);

            bckward.ODDQ4(x0);
            bckward.ODDQ4(dx0);

            // 4
            bckward.EVENQ1(x0);
            bckward.EVENQ1(dx0);

            bckward.EVENQ2(x0);
            bckward.EVENQ2(dx0);

            bckward.EVENQ3(x0);
            bckward.EVENQ3(dx0);

            bckward.EVENQ4(x0);
            bckward.EVENQ4(dx0);

            // 3
            bckward.ODDQ1(x0);
            bckward.ODDQ1(dx0);

            bckward.ODDQ2(x0);
            bckward.ODDQ2(dx0);

            bckward.ODDQ3(x0);
            bckward.ODDQ3(dx0);

            bckward.ODDQ4(x0);
            bckward.ODDQ4(dx0);

            // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

            XORDifference(DiffState, WORD_COUNT, x0, dx0);

            bwdBit = DifferenceBit(DiffState, diffdetails.OD, diffdetails.ODsize);

            if (fwdBit == bwdBit)
                matchcount++;
            loop++;
        }
        double temp = (2.0 * matchcount / sampledetails.samplesperLoop) - 1.0;
        if (temp >= PNBdetails.neutralitymeasure)
            pnb[(keyWord * 32) + keyBit] = temp;
        else
            nonpnb[(keyWord * 32) + keyBit] = temp;
    }
    // Use std::move to transfer ownership of the pair to the promise
    list.set_value(make_pair(move(pnb), move(nonpnb)));

    // Return the pair
    return make_pair(move(pnb), move(nonpnb));
}