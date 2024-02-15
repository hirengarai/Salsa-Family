/*
 * REFERENCE IMPLEMENTATION OF PNB searching programe.
 *
 * Filename: cpnb.cpp
 *
 * created: 30/12/23
 * updated: 05/01/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the PNB searching programe for the stream cipher Salsa/ ChaCha.
 * running command: g++ cpnb.cpp -pthread && ./a.out
 */

#include "chacha.h"      // chacha round functions
#include <cmath>         // pow function
#include <cstring>       // string
#include <ctime>         //  time
#include <fstream>       // storing output in a file
#include <future>        // multithreading
#include <iomanip>       // decimal numbers upto certain places
#include <thread>        // multithreading
#include <unistd.h>      // sleep
#include <unordered_map> // multithreading
#include <vector>        // multithreading

using namespace std;

// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}, fwdround{3}, totalround{6}; // Aumasson et al.
// int ODword[] = { 2,6,6,10,14 }, ODbit[] = { 0,7,19,12,0};
// int ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
// eurocrypt
u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
size_t KEY_WORD_COUNT = 8;
pair<unordered_map<u16, double>, unordered_map<u16, double>> findPNB(u16 KWord, const DetailsParams &dparams, promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> &&list);
pair<unordered_map<u16, double>, unordered_map<u16, double>> SyncofindPNB(u16 keyWord, const DetailsParams &dparams, promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> &&list);
// unordered_map<u16, double> SyncofindPNB(u16 keyWord, const DetailsParams &dparams, promise<unordered_map<u16, double>> &&list);
// void animateDots()
// {
//     int animationDelay = 500; // in milliseconds

//     while (true)
//     {
//         std::cout << ". " << std::flush;
//         std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));
//         std::cout << "\b\b  \b\b" << std::flush; // Move the cursor back to erase the dots
//         std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));
//     }
// }
bool syncoflag = false;
int main(int argc, char *argv[])
{
    if (argc > 1)
        syncoflag = true; // set true for conventional search, false for syncopated technique
    srand48(time(NULL));

    DetailsParams dparams;
    dparams.cipher = "ChaCha";
    if (syncoflag)
        dparams.progtype = "PNB search in syncopation way";
    else
        dparams.progtype = "PNB search in conventional way";
    dparams.totalround = 7;
    dparams.fwdround = 4;
    dparams.IDbit = IDbit;
    dparams.IDword = IDword;
    dparams.IDsize = sizeof(IDword) / sizeof(IDword[0]);
    dparams.ODbit = ODbit;
    dparams.ODword = ODword;
    dparams.ODsize = sizeof(ODword) / sizeof(ODword[0]);
    dparams.totalLoopCount = pow(2, 21);
    dparams.nm = pow(2.0, -2); // the measure threshold that the PNB will not affect the output of cipher
    Details(dparams, cout);
    // ---------------------------FILE CREATION------------------------------------------------------------------------------
    char filename[90], dfilename[100];

    // Get the current time
    time_t t = time(nullptr);
    tm *tm_info = localtime(&t);

    // Format the date and time in YYYYMMDD_HHMMSS format
    strftime(filename, sizeof(filename), "%M%H%S_%d%m%Y", tm_info);
    if (syncoflag)
        sprintf(filename + strlen(filename), "_synco_%.2lf_pnbs_%s_%i_id_%i_%i.txt", dparams.nm, dparams.cipher, dparams.totalround, IDword[0], IDbit[0]);
    else
        sprintf(filename + strlen(filename), "_convt_%.2lf_pnbs_%s_%i_id_%i_%i.txt", dparams.nm, dparams.cipher, dparams.totalround, IDword[0], IDbit[0]);

    sprintf(dfilename, "detailed_%s", filename); // Add your desired extra character

    ofstream dfile(dfilename);
    if (dfile.is_open())
        Details(dparams, dfile);
    dfile << "The file contains the detailed PNBs from the file " << filename << "\n\n";

    // time calculation starts :
    auto start = chrono::high_resolution_clock::now();

    // thread portion started
    const int processor_count = std::thread::hardware_concurrency();
    u16 max_num_threads = KEY_WORD_COUNT;
    cout << "# of CPU cores: " << processor_count << "\n";
    if (processor_count > KEY_WORD_COUNT)
        max_num_threads = KEY_WORD_COUNT;
    cout << "# of threads: " << max_num_threads << "\n";
    cout << "PNB Calculation Started . . . (Shh! It's a multi-threaded programme and will take some time !) \n";
    cout << "Have patience with a cup of tea . . . \n";
    cout << "**************************************************************************************\n";
    // animateDots();

    vector<thread> threadPool(max_num_threads);
    vector<future<pair<unordered_map<u16, double>, unordered_map<u16, double>>>> futuResults(max_num_threads);
    vector<pair<unordered_map<u16, double>, unordered_map<u16, double>>> perThreadList(max_num_threads);
    vector<u16> pnbList;
    vector<double> biasList;
    vector<u16> nonPNBlist;
    vector<double> nonPNBbiaslist;

    // threads are called
    for (u16 i{0}; i < max_num_threads; ++i)
    {
        promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> prms;

        futuResults.at(i) = prms.get_future();
        if (syncoflag)
            threadPool.at(i) = thread(&SyncofindPNB, i, ref(dparams), move(prms));
        else
            threadPool.at(i) = thread(&findPNB, i, ref(dparams), move(prms));
        // cout << "Thread id at " << i << " : " << threadPool.at(i).get_id() <<"\n";
    }

    for (u16 i{0}; i < max_num_threads; ++i)
        threadPool.at(i).join();
    // cout << "Thread id : " << threadPool.at(i).get_id() <<"\n";

    for (u16 i{0}; i < max_num_threads; ++i)
    {
        perThreadList.at(i) = futuResults.at(i).get();
        dfile << "****************************** Keyword: " << i << " *********************************************\n";
        dfile << "*************** PNBs (" << perThreadList.at(i).first.size() << ") ***************\n";
        for (auto &l : perThreadList.at(i).first)
        {
            pnbList.push_back(l.first);
            biasList.push_back(l.second);
            dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
        }
        dfile << "*************** Non-PNBs (" << perThreadList.at(i).second.size() << ") ***************\n";
        for (auto &l : perThreadList.at(i).second)
        {
            nonPNBlist.push_back(l.first);
            nonPNBbiaslist.push_back(l.second);
            dfile << "bias of " << (l.first) << " ~ " << setw(4) << fixed << (l.second) << "\n";
        }
        dfile << "*------------------------------------------------------------------------------------*\n";
    }
    dfile << "Total non-PNBs: " << nonPNBlist.size() << "\n";
    dfile << "Total PNBs: " << pnbList.size() << "\n\n";

    dfile << "The " << pnbList.size() << " PNBs are as follows:\n";
    ofstream file(filename);
    sort(pnbList.begin(), pnbList.end());

    if (file.is_open() && dfile.is_open())
    {
        bool flag = true;
        dfile << "{";
        for (const int &n : pnbList)
        {
            file << n << " ";
            if (!flag)
                dfile << ", ";
            dfile << n;
            flag = false;
        }
        dfile << "}\n";
        file << pnbList.size();
    }

    BiasSrotedPrint(biasList, pnbList, dfile);

    file.close();
    dfile.close();

    auto end = chrono::high_resolution_clock::now();
    cout << "Finished.\n";
    cout << "Time taken to search the PNBs is " << chrono::duration<double, std::milli>(end - start).count() << " msec"
         << " ~ " << chrono::duration_cast<chrono::seconds>(end - start).count() << " seconds"
         << " ~ " << chrono::duration_cast<chrono::minutes>(end - start).count() << " minutes.\n";
}

pair<unordered_map<u16, double>, unordered_map<u16, double>> findPNB(u16 keyWord, const DetailsParams &dparams, promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> &&list)
{
    u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16], loop{0};
    u16 fwdBit, bwdBit;
    unordered_map<u16, double> pnb;
    unordered_map<u16, double> nonpnb;

    double count;

    FORWARD fwd;
    BACKWARD bwd;
    INITIALISE_IV init_iv;
    INITIALISE_KEY init_key;
    INSERT_KEY insert_key;
    STATES state;

    for (int keyBit{0}; keyBit < 32; ++keyBit)
    {
        count = 0.0;
        loop = 0;

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

            state.CopyState(dstrdx0, dx0, 16);

            // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

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

            // modular addition of states
            state.AddStates(x0, strdx0);
            state.AddStates(dx0, dstrdx0);

            // altering key bit
            state.InputDifference(key, keyWord, keyBit);

            // making new X and X' with altered keybits
            insert_key.chacha(strdx0, key);
            insert_key.chacha(dstrdx0, key);

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

            if (fwdBit == bwdBit)
                count++;
            loop++;
        }

        if ((double)(2.0 * count) / (double)dparams.totalLoopCount - 1.0 >= dparams.nm)
            pnb[(keyWord * 32) + keyBit] = (double)(2.0 * count) / (double)dparams.totalLoopCount - 1.0;
        else
            nonpnb[(keyWord * 32) + keyBit] = (double)(2.0 * count) / (double)dparams.totalLoopCount - 1.0;
    }
    // Use std::move to transfer ownership of the pair to the promise
    list.set_value(make_pair(move(pnb), move(nonpnb)));

    // Return the pair
    return make_pair(move(pnb), move(nonpnb));
}

pair<unordered_map<u16, double>, unordered_map<u16, double>> SyncofindPNB(u16 keyWord, const DetailsParams &dparams, promise<pair<unordered_map<u16, double>, unordered_map<u16, double>>> &&list)
{
    u32 x0[16], strdx0[16], key[8], dx0[16], DiffState[16], loop{0};
    u16 fwdBit, bwdBit;
    unordered_map<u16, double> pnb;
    unordered_map<u16, double> nonpnb;

    double count;

    FORWARD fwd;
    BACKWARD bwd;
    INITIALISE_IV init_iv;
    INITIALISE_KEY init_key;
    INSERT_KEY insert_key;
    STATES state;

    for (int keyBit{0}; keyBit < 32; ++keyBit)
    {
        count = 0.0;
        loop = 0;

        while (loop < dparams.totalLoopCount)
        {
            init_iv.chacha(x0);
            init_key.Key256(key);
            insert_key.chacha(x0, key);
            state.CopyState(strdx0, x0, 16);

            // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

            for (int i{1}; i <= dparams.fwdround; ++i)
                fwd.RoundFunction(x0, i);

            fwdBit = state.DifferenceBit(x0, dparams.ODword, dparams.ODbit, dparams.ODsize);

            for (int i{dparams.fwdround + 1}; i <= dparams.totalround; ++i)
                fwd.RoundFunction(x0, i);

            // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

            // modular addition of states
            state.AddStates(x0, strdx0);

            // altering key bit
            state.InputDifference(key, keyWord, keyBit);

            // making new X and X' with altered keybits
            insert_key.chacha(strdx0, key);

            // modular subtraction of states
            state.SubtractStates(x0, strdx0);

            // ---------------------------BW ROUND STARTS--------------------------------------------------------------------
            for (int i{dparams.totalround}; i > dparams.fwdround; i--)
                bwd.RoundFunction(x0, i);

            // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

            // bwdBit = state.DifferenceBit(x0, dparams.ODword, dparams.ODbit, dparams.ODsize);
            state.XORDifference(DiffState, 16, x0, dx0);

            bwdBit = state.DifferenceBit(x0, dparams.ODword, dparams.ODbit, dparams.ODsize);

            if (fwdBit == bwdBit)
                count++;
            loop++;
        }

        if ((double)(2.0 * count) / (double)dparams.totalLoopCount - 1.0 >= dparams.nm)
            pnb[(keyWord * 32) + keyBit] = (double)(2.0 * count) / (double)dparams.totalLoopCount - 1.0;
        else
            nonpnb[(keyWord * 32) + keyBit] = (double)(2.0 * count) / (double)dparams.totalLoopCount - 1.0;
    }
    // Use std::move to transfer ownership of the pair to the promise
    list.set_value(make_pair(move(pnb), move(nonpnb)));

    // Return the pair
    return make_pair(move(pnb), move(nonpnb));
}