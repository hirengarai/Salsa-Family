/*
 * REFERENCE IMPLEMENTATION OF  ChaCha256 cipher
 *
 * Filename: deschacha.cpp
 *
 * created: 15/09/23
 * updated: 16/01/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains the bare minimum implementation of the 256-bit key version of ChaCha cipher. This is solely for research purpose.
 *
 * always include the helper files: chacha.h
 * running command: g++ deschacha.cpp && ./a.out
 */
#include <ctime>   // time
#include "final.h" // fwd, bwd

using namespace std;
using namespace ChaCha;
using namespace Operation;
using namespace Parameters;

int main()
{
    srand48(time(NULL));
    u32 x0[WORD_COUNT], key[KEY_COUNT];

    inisialize(x0);
    initkey.Key256(key);
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

    cout << "Initial State: \n";
    prntst.x = x0;
    prntst.binaryform = true;
    operate.printState(prntst);

    for (int i{1}; i <= 20; ++i)
    {
        cout << "State after " << i << "th forward round: \n";
        fwd.RoundFunction(x0, i);

        prntst.x = x0;
        operate.printState(prntst);
    }
    cout << "\n -------------------- Forward Rounds Ends Here -------------------- \n";
    for (int i{20}; i >= 1; i--)
    {
        bwd.RoundFunction(x0, i);
        cout << "State after " << i << "-th reverse round: \n";
        operate.printState(prntst);
    }
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