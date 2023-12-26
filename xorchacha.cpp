/*
 *  REFERENCE IMPLEMENTATION OF ChaCha256 cipher in XOR round functions
 *
 * Filename: xorchacha.cpp
 *
 * created: 23/09/23
 * updated: 08/10/2023
 *
 * Synopsis:
 *  This file contains the 256-bit version of the ChaCha cipher. This is solely for research purpose.
 *
 * always include the helper files: salsa.h
 * running command: g++ xorchacha.cpp && ./a.out
 */

#include <ctime>    // time
#include <iomanip>  // decimal numbers upto certain places
#include "chacha.h" // fwd, bwd

using namespace std;

u16 IDword[] = {12}, IDbit[] = {15};
u16 con{15};
double ITERATION{pow(2, 25)};

int main()
{
    srand48(time(NULL));
    u32 x0[16], dx0[16], DiffState[16], key[8], temp, temp1{0};

    double diffcount{0}, count[7]{0};
    // forward and backward class defintion
    FORWARD fwd;
    INIT_IV init_iv;
    INIT_KEY init_key;
    INSERT_KEY insert_key;
    // IDbit[0] = 16 * drand48();
    cout << "IDbit: " << IDbit[0] << "\n";
    for (int i{0}; i < ITERATION; ++i)
    {
        init_iv.chacha(x0);
        init_key.Key256(key);
        insert_key.chacha(x0, key);

        CopyState(dx0, x0, 16);

        // ----------------------------------DIFF.INJECTION--------------------------------------------------------------

        // InputDifference(dx0, IDword[0], IDbit[0]);
        u16 s{0};
        for (u16 &t : IDword)
        {
            InputDifference(dx0, t, IDbit[s]);
            s++;
        }
        // XORDifference(x0, dx0, DiffState,16);
        // BinShowOnScreen(DiffState, 16);
        // fwd.RoundFunction(x0, 1);
        fwd.Quarter_1_OddRF(x0);
        fwd.Quarter_1_OddRF(dx0);

        fwd.Quarter_2_OddRF(x0);
        fwd.Quarter_2_OddRF(dx0);

        fwd.Quarter_3_OddRF(x0);
        fwd.Quarter_3_OddRF(dx0);

        // fwd.Half_2_OddRF(x0);
        // fwd.Half_2_OddRF(dx0);

        XORDifference(x0, dx0, DiffState, 16);

        temp = NumberOfDifferences(DiffState, 0, "column");
        // cout << temp << "\n";
        if (temp == 5)
            count[0]++;

        fwd.Quarter_4_OddRF(x0);
        fwd.Quarter_4_OddRF(dx0);

        XORDifference(x0, dx0, DiffState, 16);

        temp = NumberOfDifferences(DiffState, 0, "column");
        if (temp <= con)
            count[1]++;

        // fwd.Quarter_1_EvenRF(x0);
        // fwd.Quarter_1_EvenRF(dx0);

        // XORDifference(x0, dx0, DiffState, 16);
        // for()


        // temp1 += NumberOfDifferences(DiffState, 0, "diag1");
        // BinShowOnScreen(DiffState, 16);
        // temp += temp1;
        // if (temp1 == 50)
        // count[2]++;
        // else{
        // BinShowOnScreen(DiffState, 16);
        // i = ITERATION;
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
    // cout << fixed << setprecision(2) << " diff = 4: " << count[0] * 100 / ITERATION;
    // << "%%\n diff = 5:" << count[1] * 100 / ITERATION << "%%\n diff = 6:" << count[2] * 100 / ITERATION << "%%\n diff = 7:" << count[3] * 100 / ITERATION << "%% \n diff = 8:" << count[4] * 100 / ITERATION << "%%\n diff = 9:" << count[5] * 100 / ITERATION << "\n diff > 10:" << count[6] * 100 / ITERATION;

    cout << "\n 5 difference : " << ((count[0]) / (double)ITERATION) << "\n"
         << con << " difference : " << ((count[1]) / (double)ITERATION) << "\n"
         << 25 << " difference : " << (temp1) << "\n";
}
