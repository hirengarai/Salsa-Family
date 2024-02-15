/*
 *  REFERENCE IMPLEMENTATION OF Salsa256 cipher in XOR round functions
 *
 * Filename: xorsalsa.cpp
 *
 * created: 23/09/23
 * updated: 08/10/2023
 *
 * Synopsis:
 *  This file contains the 256-bit version of the Salsa cipher. This is solely for research purpose.
 *
 * always include the helper files: salsa.h
 * running command: g++ xorsalsa.cpp && ./a.out
 */

#include <ctime>   // time
#include <iomanip> // decimal numbers upto certain places
#include "salsa.h" // fwd, bwd

using namespace std;

u16 IDword[] = {7}, IDbit[] = {31};

double ITERATION{pow(2, 0)};

int main()
{
    srand48(time(NULL));
    u32 x0[16], dx0[16], DiffState[16], key[8], temp;

    double diffcount{0}, count[7]{0};
    // forward and backward class defintion
    FORWARD fwd;
    INITIALISE_IV init_iv;
    INIT_KEY init_key;
    INSERT_KEY insert_key;

    for (int i{0}; i < ITERATION; ++i)
    {
        init_iv.salsa(x0);
        init_key.Key256(key);
        insert_key.salsa(x0, key);

        CopyState(dx0, x0, 16);

        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------

        // InputDifference(dx0, IDword[0], IDbit[0]);
        u16 s{0};
        for (u16 &t : IDword)
        {
            InputDifference(dx0, t, IDbit[s]);
            s++;
        }

        // fwd.RoundFunction(x0, 1);
        fwd.Half_1_OddRF(x0);
        fwd.Half_1_OddRF(dx0);

        fwd.Half_2_OddRF(x0);
        fwd.Half_2_OddRF(dx0);
        // UPDATE(x0[3], x0[15], x0[11],7);
        // UPDATE(dx0[3], dx0[15], dx0[11],7);

        // UPDATE(x0[7], x0[3], x0[15], 9);
        // UPDATE(dx0[7], dx0[3], dx0[15], 9);

    //     x0[11] = UPDATE(x0[11], x0[7], x0[3], 13);
    //     dx0[11] = UPDATE(dx0[11], dx0[7], dx0[3], 13);

    //    x0[15]= UPDATE(x0[15], x0[11], x0[7], 18);
    //    dx0[15]= UPDATE(dx0[15], dx0[11], dx0[7], 18);

        XORDifference(x0, dx0, DiffState, 16);
        // BinShowOnScreen(DiffState, 16);
        temp = NumberOfDifferences(DiffState, 3, "column");
        // cout << temp << "\n";
        if (temp == 4)
        {
            count[0]++;
        }
        // else{
            BinShowOnScreen(DiffState, 16);
            i = ITERATION;
        // }
        // else if (temp == 5)
        // {
        //     count[1]++;
        // }
        // else if (temp == 6)
        // {
        //     count[2]++;
        // }
        // else if (temp == 7)
        // {
        //     count[3]++;
        // }
        // else if (temp == 8)
        // {
        //     count[4]++;
        // }
        // else if (temp == 9)
        // {
        //     count[5]++;
        // }
        // else
        // {
        //     count[6]++;
        // }
        // diffcount += temp;
        // if (number == 4)
        //     count++;

        // cout << "# of difference after one round: " << number << "\n";
        // ShowOnScreen(DiffState, 16);
    }
    cout << fixed << setprecision(2) << " diff = 4: " << count[0] * 100 / ITERATION;
    // << "%%\n diff = 5:" << count[1] * 100 / ITERATION << "%%\n diff = 6:" << count[2] * 100 / ITERATION << "%%\n diff = 7:" << count[3] * 100 / ITERATION << "%% \n diff = 8:" << count[4] * 100 / ITERATION << "%%\n diff = 9:" << count[5] * 100 / ITERATION << "\n diff > 10:" << count[6] * 100 / ITERATION;

    // cout
    //     << "\n Average number of differences: " << round((diffcount) / (double)ITERATION) << "\n";
}
