/*
 * REFERENCE IMPLEMENTATION OF forro.h header file
 *
 * Filename: forro.h
 *
 * created: 17/02/24
 * updated: 12/02/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains functions that implement the bare minimum of Forro cipher
 */
#include "commonfiles2.h"

// QR helping functions
#define ROTATE_RIGHT(x, n) (((x) >> (n)) ^ ((x) << (WORD_SIZE - n)))
#define UPDATE(a, b, n) (ROTATE_LEFT((a) ^ (b), (n)))
#define ADDUPDATE(a, b, n) (ROTATE_LEFT((a) + (b), (n)))

// ---------------------------FW QR-----------------------------------
#define FWDQR(a, b, c, d, e, X) \
    d += e;                     \
    c ^= d;                     \
    b = ADDUPDATE(b, c, 10);    \
    a += b;                     \
    e ^= a;                     \
    d = ADDUPDATE(d, e, 27);    \
    c += d;                     \
    b ^= c;                     \
    a = ADDUPDATE(a, b, 8);

// ---------------------------BW QR-----------------------------------
#define BWDQR(a, b, c, d, e, x)  \
    a = ROTATE_RIGHT(a, 8) - b;  \
    b ^= c;                      \
    c -= d;                      \
    d = ROTATE_RIGHT(d, 27) - e; \
    e ^= a;                      \
    a -= b;                      \
    b = ROTATE_RIGHT(b, 10) - c; \
    c ^= d;                      \
    d -= e;

// -------------------------------------- RoundFunctionDefinition ------------------------------------------------
class FORWARD
{
public:
    void RoundFunction(u32 *x, u32 round)
    {
        if (round & 1)
        {
            FWDQR((x[0]), (x[4]), (x[8]), (x[12]), (x[3]),false);
            FWDQR((x[1]), (x[5]), (x[9]), (x[13]), (x[0]), false);
            FWDQR((x[2]), (x[6]), (x[10]), (x[14]), (x[1]), false);
            FWDQR((x[3]), (x[7]), (x[11]), (x[15]), (x[2]), false);
        }
        else
        {
            FWDQR((x[0]), (x[5]), (x[10]), (x[15]), (x[3]), false);
            FWDQR((x[1]), (x[6]), (x[11]), (x[12]), (x[0]), false);
            FWDQR((x[2]), (x[7]), (x[8]), (x[13]), (x[1]), false);
            FWDQR((x[3]), (x[4]), (x[9]), (x[14]), (x[2]), false);
        }
    }
} frward;

class BACKWARD
{
public:
    void RoundFunction(u32 *x, u32 round)
    {
        if (round & 1)
        {
            BWDQR((x[3]), (x[7]), (x[11]), (x[15]), (x[2]), false);
            BWDQR((x[2]), (x[6]), (x[10]), (x[14]), (x[1]), false);
            BWDQR((x[1]), (x[5]), (x[9]), (x[13]), (x[0]), false);
            BWDQR((x[0]), (x[4]), (x[8]), (x[12]), (x[3]), false);
        }
        else
        {
            BWDQR((x[3]), (x[4]), (x[9]), (x[14]), (x[2]), false);
            BWDQR((x[2]), (x[7]), (x[8]), (x[13]), (x[1]), false);
            BWDQR((x[1]), (x[6]), (x[11]), (x[12]), (x[0]), false);
            BWDQR((x[0]), (x[5]), (x[10]), (x[15]), (x[3]), false);
        }
    }
} bckward;