/*
 * REFERENCE IMPLEMENTATION OF ChaCha256 cipher in XOR round functions
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
double ITERATION{pow(2, 25)};

int main()
{
    srand48(0);
    u32 x0[16], dx0[16], DiffState[16], key[8];

    // forward and backward class defintion
    FORWARD fwd;
    INITIALISE_IV init_iv;
    INITIALISE_KEY init_key;
    INSERT_KEY insert_key;
    STATES State;

    // cout << "IDbit: " << IDbit[0] << "\n";

    init_iv.chacha(x0);
    init_key.Key256(key);
    insert_key.chacha(x0, key);

    // State.CopyState(dx0, x0, 16);

    // State.InputDifference(dx0, 13, 15);

    fwd.RoundFunction(x0, 1);


    // fwd.RoundFunction(dx0, 1);

    State.XORDifference(x0,dx0, DiffState, 16);

    PrintStateParams psparams;
    // psparams.binaryform = true;
    psparams.x = DiffState;
    psparams.size = 16;

    State.PrintState(psparams);

    psparams.x = x0;
    State.PrintState(psparams);

    // ----------------------------------DIFF.INJECTION--------------------------------------------------------------
}
