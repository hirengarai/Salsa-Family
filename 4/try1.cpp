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
using namespace ChaCha;
using namespace Parameters;
using namespace Operation;

u16 IDword[] = {13}, IDbit[] = {6}, ODword[] = {2, 7, 8}, ODbit[] = {0, 7, 0};

ull threadCOUNTER(u16 *PNB, u16 keyBit, ull loopSize, promise<double> &&COUNT)
{
    u32 x0[16], strdx0[16], key[8], dx0[16], dstrdx0[16], DiffState[16];
    u16 fwdBit, bwdBit;
    double count{0.0};
    ull loop{0};

    FORWARD fwd;
    BACKWARD bwd;
    while (loop < loopSize)
    {
        inisialize(x0);
        init_key.Key256(key);
        insertkey(x0, key);

        CopyState(strdx0, x0, 16);
        CopyState(dx0, x0, 16);
        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
        u16 index{0};
        for (const u16 &i : IDword)
        {
            InputDifference(dx0, i, IDbit[index]);
            index++;
        }
        CopyState(dstrdx0, dx0, 16);

        // ---------------------------FW ROUND STARTS--------------------------------------------------------------------

        for (int i{1}; i <= dparams.fwdround; ++i)
        {
            fwd.RoundFunction(x0, i);
            fwd.RoundFunction(dx0, i);
        }

        XORDifference(DiffState, 16, x0, dx0);

        fwdBit = DifferenceBit(DiffState, dparams.ODword, dparams.ODbit, dparams.ODsize);

        for (int i{dparams.fwdround + 1}; i <= dparams.totalround; ++i)
        {
            fwd.RoundFunction(x0, i);
            fwd.RoundFunction(dx0, i);
        }

        // ---------------------------FW ROUND ENDs----------------------------------------------------------------------

        // modular addition of states
        AddStates(x0, strdx0);
        AddStates(dx0, dstrdx0);

        // randomise the existing PNBs
        for (int i{0}; i < sizeof(PNB) / sizeof(PNB[0]); ++i)
        {
            int WORD = (PNB[i] / WORD_SIZE) + 4;
            int BIT = PNB[i] % WORD_SIZE;

            if (generateRandomBoolean)
            {
                ToggleBit(strdx0[WORD], BIT);
                ToggleBit(dstrdx0[WORD], BIT);
            }
        }

        // altering the key bit
        ToggleBit(strdx0[keyBit / WORD_SIZE + 4], keyBit);
        ToggleBit(dstrdx0[keyBit / WORD_SIZE + 4], keyBit);

        // modular subtraction of states
        SubtractStates(x0, strdx0);
        SubtractStates(dx0, dstrdx0);

        // ---------------------------BW ROUND STARTS--------------------------------------------------------------------

        for (int i{dparams.totalround}; i > dparams.fwdround; i--)
        {
            bwd.RoundFunction(x0, i);
            bwd.RoundFunction(dx0, i);
        }

        // ---------------------------BW ROUND ENDS----------------------------------------------------------------------

        XORDifference(DiffState, 16, x0, dx0);

        bwdBit = DifferenceBit(DiffState, dparams.ODword, dparams.ODbit, dparams.ODsize);

        if (fwdBit == bwdBit)
            count++;
        loop++;
    }
    COUNT.set_value(count);
    return count;
}

int main()
{
    u16 PNBCount, PNBCountLimit = 80;

    for (PNBCount = 0; PNBCount < PNBCountLimit; ++PNBCount)
    {
        for (u16 keyBit{0}; keyBit < 256; ++keyBit)
        {
            
        }
    }
}