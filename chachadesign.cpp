/*
 * REFERENCE IMPLEMENTATION OF  ChaCha256 cipher
 *
 * Filename: deschacha.cpp
 *
 * created: 23/09/23
 * updated: 31/12/23
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 * 
 * Synopsis:
 * This file contains the 256-bit version of the ChaCha cipher. This is solely for research purpose.
 *
 * always include the helper files: chacha.h
 * running command: g++ deschacha.cpp && ./a.out
 */
#include <ctime>    // time
#include "chacha.h" // fwd, bwd

using namespace std;

int main()
{
    srand48(time(NULL));
    u32 x0[16], strdx0[16], key[8];

    // forward and backward class defintion
    FORWARD fwd;
    BACKWARD bwd;
    INITIALISE_IV init_iv;
    INITIALISE_KEY init_key;
    INSERT_KEY insert_key;
    STATES States;

    init_iv.chacha(x0);
    init_key.Key256(key);
    insert_key.chacha(x0, key);

    x0[4] = 0x03020100;
    x0[5] = 0x07060504;
    x0[6] = 0x0b0a0908;
    x0[7] = 0x0f0e0d0c;
    x0[8] = 0x13121110;
    x0[9] = 0x17161514;
    x0[10] = 0x1b1a1918;
    x0[11] = 0x1f1e1d1c;
    x0[12] = 0x00000001;
    x0[13] = 0x09000000;
    x0[14] = 0x4a000000;
    x0[15] = 0x00000000;

    States.CopyState(strdx0, x0, 16);

    cout << "Initial State: \n";
    States.PrintState(x0, 16);

    // fwd.RoundFunction(x0, 1);
    // fwd.RoundFunction(x0, 2);
    // fwd.RoundFunction(x0, 3);
    // fwd.Half_1_EvenRF(x0);
    // cout << "State after 3.5 rounds: \n";
    // States.PrintState(x0, 16);
    // fwd.Half_2_EvenRF(x0);
    // fwd.RoundFunction(x0, 5);
    // fwd.RoundFunction(x0, 6);

    for (int i{1}; i < 21; ++i)
        fwd.RoundFunction(x0, i);

    cout << "State after 20 rounds: \n";
    States.PrintState(x0, 16);

    // SubtractStates(x0, strdx0);
    // bwd.Half_1_EvenRF(x0);
    // bwd.Half_2_EvenRF(x0);
    // bwd.RoundFunction(x0, 1);
    for (int i{0}; i < 20; ++i)
        bwd.RoundFunction(x0, i);

    // bwd.RoundFunction(x0, 6);
    // bwd.RoundFunction(x0, 5);
    // // bwd.RoundFunction(x0, 4);
    // bwd.Half_1_EvenRF(x0);
    cout << "State after 20 rev. rounds: \n";
    States.PrintState(x0, 16);
}



/*
Init. state:              61707865  3320646e  79622d32  6b206574
                          03020100  07060504  0b0a0908  0f0e0d0c
                          13121110  17161514  1b1a1918  1f1e1d1c
                          00000001  09000000  4a000000  00000000
state after 20 rounds:    837778ab  e238d763  a67ae21e  5950bb2f
                          c4f2d0c7  fc62bb2f  8fa018fc  3f5ec7b7
                          335271c2  f29489f3  eabda8fc  82e46ebd
                          d19c12b4  b04e16de  9e83d0cb  4e3c50a2
*/