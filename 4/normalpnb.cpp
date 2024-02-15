#include "chacha1.h"     // chacha round functions
#include <cmath>         // pow function
#include <cstring>       // string
#include <ctime>         //  time
#include <fstream>       // storing output in a file
#include <future>        // multithreading
#include <iomanip>       // decimal numbers upto certain places
#include <thread>        // multithreading
#include <unistd.h>      // sleep
#include <unordered_map> // multithreading
#include <vector>

bool syncoflag = false;
u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};
int main(int argc, char *argv[])
{
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime(&t);
    printf("######## Execution Started on: %d-%d-%d %d:%d:%d ########\n\n\n", lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900, lt->tm_hour, lt->tm_min, lt->tm_sec);

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
    Details(dparams, std::cout);
    // ---------------------------FILE CREATION------------------------------------------------------------------------------
    char filename[90], dfilename[100];

    // Get the current time
    time_t t1 = time(nullptr);
    struct tm *tm_info = localtime(&t1);

    // Format the date and time in YYYYMMDD_HHMMSS format
    strftime(filename, sizeof(filename), "%M%H%S_%d%m%Y", tm_info);
    if (syncoflag)
        sprintf(filename + strlen(filename), "_nor_synco_%.2lf_pnbs_%s_%i_id_%i_%i.txt", dparams.nm, dparams.cipher, dparams.totalround, IDword[0], IDbit[0]);
    else
        sprintf(filename + strlen(filename), "_nor_convt_%.2lf_pnbs_%s_%i_id_%i_%i.txt", dparams.nm, dparams.cipher, dparams.totalround, IDword[0], IDbit[0]);

    sprintf(dfilename, "detailed_%s", filename); // Add your desired extra character

    std::ofstream dfile(dfilename);
    if (dfile.is_open())
        Details(dparams, dfile);

    dfile << "########Execution Started on: " << lt->tm_mday << "-" << lt->tm_mon + 1 << "-" << lt->tm_year + 1900 << "-" << lt->tm_hour << "-" << lt->tm_min << "-" << lt->tm_sec << "########\n\n";
    dfile << "The file contains the detailed PNBs from the file " << filename << "\n\n";

    u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16], loop;
    u16 fwdBit, bwdBit;
    std::unordered_map<u16, double> pnb;
    std::unordered_map<u16, double> nonpnb;

    double count;

    FORWARD fwd;
    BACKWARD bwd;
    INITIALISE_IV init_iv;
    INITIALISE_KEY init_key;
    INSERT_KEY insert_key;
    STATES state;
    auto start = std::chrono::high_resolution_clock::now();
    for (int keyWord{0}; keyWord < 8; ++keyWord)
    {
        dfile << "****************************** Keyword: " << keyWord << " *********************************************\n";
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
            double temp = (2.0 * count / dparams.totalLoopCount) - 1.0;
            if (temp >= dparams.nm)
            {
                pnb[(keyWord * 32) + keyBit] = temp;
                dfile << "bias of " << keyBit << " ~ " << std::setw(4) << std::fixed << temp << "\n";
            }
            else
            {
                nonpnb[(keyWord * 32) + keyBit] = temp;
                dfile << "bias of " << keyBit << " ~ " << std::fixed << std::setw(4) << temp << "\n";
            }
        }
    }

    dfile << "*************** Non-PNBs (" << nonpnb.size() << ") ***************\n";

    std::ofstream file(filename);

    if (file.is_open() && dfile.is_open())
    {
        bool flag = true;
        dfile << "{";
        if (!pnb.empty())
        {
            auto it = pnb.begin();
            file << it->first << " ";
            dfile << it->first << ", ";
        }
        file << pnb.size() << "\n";
        dfile << "}\n";
    }
    dfile << "*************** PNBs (" << pnb.size() << ") ***************\n";
    auto end = std::chrono::high_resolution_clock::now();
    dfile << "Execution time: " << std::chrono::duration<double, std::milli>(end - start).count() << " millisec"
          << "\n";
    file.close();
    dfile.close();
}