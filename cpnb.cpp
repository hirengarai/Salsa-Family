/*
 * REFERENCE IMPLEMENTATION OF PNB searching programe.
 *
 * Filename: cpnb.cpp
 *
 * created: 30/12/23
 * updated: 30/12/23
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the PNB searching programe for the stream cipher Salsa/ ChaCha.
 * running command: g++ cpnb.cpp -pthread && ./a.out
 */

#include <algorithm>     // sorting purpose
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

// DIFFFERENTIAL ATTACK PARAMETERS
// Change these parameters

// Aumasson et al.
char cipher[8] = "ChaCha";
u16 totalround = 7;
// u16 IDword[] = {13}, IDbit[] = {13}, ODword[] = {11}, ODbit[] = {0}, fwdround{3}, totalround{6}; // Aumasson et al.

// int ODword[] = { 2,6,6,10,14 }, ODbit[] = { 0,7,19,12,0};
// int ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
// eurocrypt
u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0}, fwdround{4}, IDsize{1}, ODsize{3};
// eurocrypt

u32 ITERATION = pow(2, 20); // the measure threshold that the PNB will not affect the output of cipher
double THRESHOLD = pow(2.0, -1) * pow(2.0, -1);

unordered_map<u16, double> findPNB(u16 KWord, promise<unordered_map<u16, double>> &&PNBList);

int main()
{
    // ---------------------------FILE CREATION------------------------------------------------------------------------------
    char filename[25];
    sprintf(filename, "%s%ipnb%i_%i.txt", cipher, totalround, IDword[0], IDbit[0]);
    ofstream file(filename);

    cout << "            PNB Determining Programe\n";
    file << "            PNB Determining Programe\n";

    // sleep(1.5);
    cout << "            Cipher Name: " << cipher << ", # of rounds:" << (unsigned)totalround << ", # of fwd rounds: " << fwdround << "\n";
    file << "            Cipher Name: " << cipher << ", # of rounds:" << (unsigned)totalround << ", # of fwd rounds: " << fwdround << "\n";
    cout << "            Threshold = " << THRESHOLD << "\n";
    file << "            Threshold = " << THRESHOLD << "\n";
    cout << "            ID = ";
    file << "            ID = ";
    for (int i{0}, n = sizeof(IDword) / sizeof(IDword[0]); i < n; ++i)
    {
        if (i == n - 1)
        {
            cout << "(" << IDword[i] << ", " << IDbit[i] << ")\n";
            file << "(" << IDword[i] << ", " << IDbit[i] << ")\n";
        }
        else
        {
            cout << "(" << IDword[i] << ", " << IDbit[i] << ")⊕";
            file << "(" << IDword[i] << ", " << IDbit[i] << ")⊕";
        }
    }
    // sleep(1);
    cout << "            OD = ";
    file << "            OD = ";
    for (int i{0}, n = sizeof(ODword) / sizeof(ODword[0]); i < n; ++i)
    {
        if (i == n - 1)
        {
            cout << "(" << ODword[i] << ", " << ODbit[i] << ")\n";
            file << "(" << ODword[i] << ", " << ODbit[i] << ")\n";
        }
        else
        {
            cout << "(" << ODword[i] << ", " << ODbit[i] << ")⊕";
            file << "(" << ODword[i] << ", " << ODbit[i] << ")⊕";
        }
    }
    cout << "            loop size : 2^" << log2(ITERATION) << "\n";
    file << "            loop size : 2^" << log2(ITERATION) << "\n";
    cout << "*************************************************\n";
    file << "*************************************************\n";

    // time calculation starts :
    auto start = chrono::high_resolution_clock::now();

    // thread portion started
    const int processor_count = std::thread::hardware_concurrency();
    int max_num_threads = processor_count;
    cout << "# of CPU cores : " << processor_count << "\n";
    // sleep(1.5);
    cout << "# of threads = " << max_num_threads << "\n";
    // sleep(1.5);
    cout << "PNB Calculation Started . . . (Shh! It's a multi-threaded programme !) \n";
    // sleep(1);
    cout << "Have Patience or a cup of tea . . . \n";

    cout << "----------------------------------------------------------------------------------------------------------------\n";
    vector<thread> threadPool(max_num_threads);
    vector<future<unordered_map<u16, double>>> futuResults(max_num_threads);
    vector<unordered_map<u16, double>> threadPNBlist(max_num_threads);
    vector<u16> pnbList;
    vector<double> biasList;
    vector<u16> nonPNBlist;

    // threads are called
    for (u32 i{0}; i < max_num_threads; ++i)
    {
        promise<unordered_map<u16, double>> prms;

        futuResults.at(i) = prms.get_future();
        threadPool.at(i) = thread(&findPNB, i, move(prms));
        // cout << "Thread id at " << i << " : " << threadPool.at(i).get_id() << endl;
    }

    for (u32 i{0}; i < max_num_threads; ++i)
    {
        threadPool.at(i).join();
        // cout << "Thread id : " << threadPool.at(i).get_id() << endl;
    }

    for (u32 i{0}; i < max_num_threads; ++i)
    {
        threadPNBlist.at(i) = futuResults.at(i).get();
        // cout << "No. of PNBs in " << i << "-th keyWord = " << threadPNBlist.at(i).size() << endl;
        file << "No. of PNBs in " << i << "-th keyWord = " << threadPNBlist.at(i).size() << "\n";
        for (auto &l : threadPNBlist.at(i))
        {
            pnbList.push_back(l.first);
            biasList.push_back(l.second);
            file << "bias of " << (l.first) << " = " << (l.second) << endl;
        }
    }
    // bias_sort(biasList, pnbList);
    sort(pnbList.begin(), pnbList.end());

    u16 t{0};
    cout << "\n{";
    file << "\n{";
    for (const int &n : pnbList)
    {
        t++;
        if (t == pnbList.size())
        {
            cout << n;
            file << n;
        }
        else
        {
            cout << n << ", ";
            file << n << ", ";
        }
    }
    cout << "}";
    file << "}";

    cout << "\n+--------------------------+\n";
    cout << "+     🔴#PNB =  " << pnbList.size() << "          +\n";
    cout << "+--------------------------+\n";

    file << "\n+--------------------------+\n";
    file << "+     🔴#PNB =  " << pnbList.size() << "          +\n";
    file << "+--------------------------+\n";

    cout << "----------------------------------------------------------------------------------------------------------\n";
    file << "----------------------------------------------------------------------------------------------------------\n";
    bool flag;
    for (int i{0}; i < 256; ++i)
    {
        flag = false;
        for (auto &j : pnbList)
        {
            if (j == i)
            {
                flag = true;
                break;
            }
        }
        if (flag == false)
            nonPNBlist.push_back(i);
    }
    t = 0;
    cout << "{";
    file << "{";
    for (const int &n : nonPNBlist)
    {
        t++;
        if (t == nonPNBlist.size())
        {
            cout << n;
            file << n;
        }
        else
        {
            cout << n << ", ";
            file << n << ", ";
        }
    }
    cout << "}";
    file << "}";

    cout << "\n+-----------------------+\n";
    cout << "+   🟢#nonPNB = " << nonPNBlist.size() << "     +\n";
    cout << "+-----------------------+\n";

    file << "\n+-----------------------+\n";
    file << "+   🟢#nonPNB = " << nonPNBlist.size() << "     +\n";
    file << "+-----------------------+\n";

    auto end = chrono::high_resolution_clock::now();
    cout << "Time taken to execute is " << chrono::duration<double, std::milli>(end - start).count() << " msec"
         << " ~ " << chrono::duration_cast<chrono::seconds>(end - start).count() << " seconds"
         << " ~ " << chrono::duration_cast<chrono::minutes>(end - start).count() << " minutes.\n";

    file << "Time taken to execute is " << chrono::duration<double, std::milli>(end - start).count() << " msec"
         << " ~ " << chrono::duration_cast<chrono::seconds>(end - start).count() << " seconds"
         << " ~ " << chrono::duration_cast<chrono::minutes>(end - start).count() << " minutes.\n";
}

unordered_map<u16, double> findPNB(u16 keyWord, promise<unordered_map<u16, double>> &&pnbList)
{
    u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16], loop{0};
    u16 fwBit{0}, bwBit{0};
    unordered_map<u16, double> pnb;

    double count;

    for (int keyBit{0}; keyBit < 32; ++keyBit)
    {
        count = 0.0;
        loop = 0;

        FORWARD fwd;
        BACKWARD bwd;
        INIT_IV init_iv;
        INIT_KEY init_key;
        INSERT_KEY insert_key;

        while (loop < ITERATION)
        {
            init_iv.chacha(x0);
            init_key.Key256(key);
            insert_key.chacha(x0, key);

            CopyState(strdx0, x0, 16);
            CopyState(dx0, x0, 16);
            // ----------------------------------DIFF.INJECTION--------------------------------------------------------------

            // InputDifference(dx0, IDword[0], IDbit[0]);
            u16 s{0};
            for (const u16 &t : IDword)
            {
                InputDifference(dx0, t, IDbit[s]);
                s++;
            }

            CopyState(dstrdx0, dx0, 16);
            // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

            for (int i{1}; i <= fwdround; ++i)
            {
                fwd.RoundFunction(x0, i);
                fwd.RoundFunction(dx0, i);
            }

            XORDifference(x0, dx0, DiffState, 16);
            fwBit = (DiffState[2] ^ (DiffState[7] >> 7) ^ DiffState[8]) & 1;

            //--------------------------------------------+
            // i = 0;                                     -
            // for (u16 &j : ODword)                      -
            // {                                          -
            //     temp ^= (DiffState[j] >> ODbit[i]);    -
            //     i++;                                   -
            // }                                          -
            // fwBit = temp;                              -
            //--------------------------------------------+

            // fwBit = DiffState[2] ^ (DiffState[7] >> 7) ^ DiffState[8]; // Eurocrypt (4 rounds fw)
            // fwBit = DiffState[2] ^ (DiffState[6] >> 7) ^ (DiffState[6] >> 19) ^ (DiffState[10] >> 12) ^ DiffState[14]; // new dist. (5 rounds fw)

            for (int i{fwdround + 1}; i <= totalround; ++i)
            {
                fwd.RoundFunction(x0, i);
                fwd.RoundFunction(dx0, i);
            }
            // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

            // modular addition
            AddStates(x0, strdx0);
            AddStates(dx0, dstrdx0);

            // altering key bit
            InputDifference(key, keyWord, keyBit);

            // making new X and X' with altered keybits
            insert_key.chacha(strdx0, key);
            insert_key.chacha(dstrdx0, key);

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
            // s = 0;                                     -
            // for (&t : ODword)                          -
            // {                                          -
            //     temp1 ^= (DiffState[t] >> ODbit[s]);   -
            //     s++;                                   -
            // }                                          -
            // bwBit = temp1;                             -
            //--------------------------------------------+

            bwBit = (DiffState[2] ^ (DiffState[7] >> 7) ^ DiffState[8]) & 1;
            // bwBit = DiffState[2] ^ (DiffState[7] >> 7) ^ DiffState[8]; // Eurocrypt (4 rounds fw)
            // bwBit = DiffState[2] ^ (DiffState[6] >> 7) ^ (DiffState[6] >> 19) ^ (DiffState[10] >> 12) ^ DiffState[14]; // new dist. (5 rounds fw)

            if (fwBit == bwBit)
                count++;
            loop++;
        }

        if ((double)(2.0 * count) / (double)ITERATION - 1.0 >= THRESHOLD)
            pnb[(keyWord * 32) + keyBit] = (double)(2.0 * count) / (double)ITERATION - 1.0;
    }
    pnbList.set_value(pnb);
    return pnb;
}