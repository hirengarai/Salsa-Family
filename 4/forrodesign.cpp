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
#include "forro.h" // fwd, bwd

using namespace std;
using namespace Forro;
using namespace Parameters;
using namespace Operation;

int main()
{
    // DetailsParams dParams;
    // dParams.cipher = "ChaCha";
    // dParams.progtype = "Test Vector Testing";
    // dParams.totalround = 20;
    // Details(dParams, cout);

    srand48(time(NULL));
    u32 x0[16], strdx0[16], key[8];

    // forward and backward class defintion
    // FORWARD fwd;
    // BACKWARD bwd;
    // INITIALISE_IV init_iv;
    // INITIALISE_KEY init_key;
    // INSERT_KEY insert_key;
    // STATES States;

    // PrintStateParams psParams;

    inisialize(x0);
    init_key.Key256(key);
    insertkey(x0, key);

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
    CopyState(strdx0, x0);

    cout << "Initial State: \n";

    prntstate.x = x0;
    // prntstate.binaryform = true;
    PrintState(prntstate);

    for (int i{1}; i <= 20; ++i)
        frward.RoundFunction(x0, i);

    cout << "State after " << 20 << " rounds: \n";
    PrintState(prntstate);

    for (int i{20}; i > 0; i--)
        bckward.RoundFunction(x0, i);

    cout << "State after " << 20 << " rev. rounds: \n";
    PrintState(prntstate);
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

uint8_t expected[128] = {
    0xc5, 0xa9, 0x6c, 0x62, 0xf2, 0x93, 0x52, 0xaf, 0xf2, 0x62, 0x95, 0xb5, 0x8d, 0xa0, 0x59, 0x5c,
    0x62, 0x10, 0x82, 0x25, 0xf1, 0x4e, 0x33, 0x11, 0x16, 0xad, 0x3f, 0x7b, 0x4e, 0xa0, 0x00, 0xfe,
    0xc0, 0xf0, 0x36, 0x8e, 0x42, 0x11, 0x49, 0xb2, 0x6b, 0x0b, 0x43, 0x98, 0xdb, 0x7b, 0x3b, 0xbb,
    0x99, 0xe3, 0xf5, 0xd7, 0xa9, 0x1b, 0xf0, 0x28, 0x99, 0x6a, 0x8c, 0x46, 0x51, 0x70, 0x7e, 0xf1,
    0xdc, 0xbe, 0xe0, 0xc1, 0x27, 0x1a, 0x0c, 0xf7, 0xe0, 0x0e, 0xb1, 0xbc, 0x1e, 0x6f, 0xf8, 0x6e,
    0xf2, 0x3c, 0xac, 0xa9, 0x86, 0xa0, 0x03, 0x7e, 0x02, 0x92, 0x2b, 0xa5, 0xaa, 0x6a, 0x1d, 0x6d,
    0xf0, 0x9f, 0x5b, 0xd1, 0xc5, 0x40, 0xb0, 0xd9, 0xd1, 0xcc, 0x8b, 0x3e, 0xc3, 0x90, 0x66, 0x0a,
    0xe6, 0x8a, 0x88, 0x49, 0xfb, 0x57, 0xea, 0x3a, 0x71, 0xd8, 0x44, 0xe7, 0x20, 0xb4, 0x84, 0x70};