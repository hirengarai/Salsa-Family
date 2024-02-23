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
#define XORUPDATE(a, b, n) (ROTATE_LEFT((a) ^ (b), (n)))

// ---------------------------FW QR-----------------------------------
#define FWDQR(a, b, c, d, e, X) (            \
    d += e, c ^= d, b = ADDUPDATE(b, c, 10), \
    a += b, e ^= a, d = ADDUPDATE(d, e, 27), \
    c += d, b ^= c, a = ADDUPDATE(a, b, 8))

#define XFWDQR(a, b, c, d, e) (            \
    d ^= e, c ^= d, b = XORUPDATE(b, c, 10), \
    a ^= b, e ^= a, d = XORUPDATE(d, e, 27), \
    c ^= d, b ^= c, a = XORUPDATE(a, b, 8))

// ---------------------------BW QR-----------------------------------
#define BWDQR(a, b, c, d, e, x) (                \
    a = ROTATE_RIGHT(a, 8) - b, b ^= c, c -= d,  \
    d = ROTATE_RIGHT(d, 27) - e, e ^= a, a -= b, \
    b = ROTATE_RIGHT(b, 10) - c, c ^= d, d -= e)

// -------------------------------------- RoundFunctionDefinition ------------------------------------------------
class FORWARD
{
public:
    void ODDQ1(u32 *x)
    {
        FWDQR((x[0]), (x[4]), (x[8]), (x[12]), (x[3]), false);
    }
    void ODDQ2(u32 *x)
    {
        FWDQR((x[1]), (x[5]), (x[9]), (x[13]), (x[0]), false);
    }
    void ODDQ3(u32 *x)
    {
        FWDQR((x[2]), (x[6]), (x[10]), (x[14]), (x[1]), false);
    }
    void ODDQ4(u32 *x)
    {
        FWDQR((x[3]), (x[7]), (x[11]), (x[15]), (x[2]), false);
    }

    void EVENQ1(u32 *x)
    {
        FWDQR((x[0]), (x[5]), (x[10]), (x[15]), (x[3]), false);
    }
    void EVENQ2(u32 *x)
    {
        FWDQR((x[1]), (x[6]), (x[11]), (x[12]), (x[0]), false);
    }
    void EVENQ3(u32 *x)
    {
        FWDQR((x[2]), (x[7]), (x[8]), (x[13]), (x[1]), false);
    }
    void EVENQ4(u32 *x)
    {
        FWDQR((x[3]), (x[4]), (x[9]), (x[14]), (x[2]), false);
    }

    void Half_1_ODDRF(u32 *x)
    {
        ODDQ1(x);
        ODDQ2(x);
    }
    void Half_2_ODDRF(u32 *x)
    {
        ODDQ3(x);
        ODDQ4(x);
    }

    void Half_1_EVENRF(u32 *x)
    {
        EVENQ1(x);
        EVENQ2(x);
    }
    void Half_2_EVENRF(u32 *x)
    {
        EVENQ3(x);
        EVENQ4(x);
    }

    void RoundFunction(u32 *x, u32 round)
    {
        if (round & 1)
        {
            FWDQR((x[0]), (x[4]), (x[8]), (x[12]), (x[3]), false);
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
    void ODDQ1(u32 *x)
    {
        BWDQR((x[3]), (x[7]), (x[11]), (x[15]), (x[2]), false);
    }
    void ODDQ2(u32 *x)
    {
        BWDQR((x[2]), (x[6]), (x[10]), (x[14]), (x[1]), false);
    }
    void ODDQ3(u32 *x)
    {
        BWDQR((x[1]), (x[5]), (x[9]), (x[13]), (x[0]), false);
    }
    void ODDQ4(u32 *x)
    {
        BWDQR((x[0]), (x[4]), (x[8]), (x[12]), (x[3]), false);
    }

    void EVENQ1(u32 *x)
    {
        BWDQR((x[3]), (x[4]), (x[9]), (x[14]), (x[2]), false);
    }
    void EVENQ2(u32 *x)
    {
        BWDQR((x[2]), (x[7]), (x[8]), (x[13]), (x[1]), false);
    }
    void EVENQ3(u32 *x)
    {
        BWDQR((x[1]), (x[6]), (x[11]), (x[12]), (x[0]), false);
    }
    void EVENQ4(u32 *x)
    {
        BWDQR((x[0]), (x[5]), (x[10]), (x[15]), (x[3]), false);
    }

    void Half_1_ODDRF(u32 *x)
    {
        ODDQ1(x);
        ODDQ2(x);
    }
    void Half_2_ODDRF(u32 *x)
    {
        ODDQ3(x);
        ODDQ4(x);
    }

    void Half_1_EVENRF(u32 *x)
    {
        EVENQ1(x);
        EVENQ2(x);
    }
    void Half_2_EVENRF(u32 *x)
    {
        EVENQ3(x);
        EVENQ4(x);
    }
    void RoundFunction(u32 *x, u32 round)
    {
        if (round & 1)
        {
            Half_1_ODDRF(x);
            Half_2_ODDRF(x);
        }
        else
        {
            Half_1_EVENRF(x);
            Half_2_EVENRF(x);
        }
    }
} bckward;