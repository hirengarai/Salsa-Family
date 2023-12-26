#include "commonfiles.h"

// QR helping functions
#define UPDATE(a, b, c, n) (a ^ (ROTATE_LEFT((b + c), n)))
// ---------------------------FW QR-----------------------------------
#define FW_HALF_1_QR(a, b, c, d) ( \
    b = UPDATE(b, a, d, 7),        \
    c = UPDATE(c, b, a, 9))
#define FW_HALF_2_QR(a, b, c, d) ( \
    d = UPDATE(d, c, b, 13),       \
    a = UPDATE(a, d, c, 18))
// #define FW_QR(a, b, c, d) (   \
//     FW_HALF_1_QR(a, b, c, d), \
//     FW_HALF_2_QR(a, b, c, d))
// XOR QR
#define X_QR(a, b, c, d) (       \
    b = a ^ (ROTATE_LEFT((a ^ d), 7)),  \
    c = c ^ (ROTATE_LEFT((b ^ a), 9)),  \
    d = d ^ (ROTATE_LEFT((b ^ c), 13)), \
    a = a ^ (ROTATE_LEFT((d ^ c), 18)))

// ---------------------------BW QR----------------------------------
#define BW_HALF_1_QR(a, b, c, d) ( \
    a = UPDATE(a, d, c, 18),       \
    d = UPDATE(d, c, b, 13))
#define BW_HALF_2_QR(a, b, c, d) ( \
    c = UPDATE(c, b, a, 9),        \
    b = UPDATE(b, a, d, 7))
// #define BW_QR(a, b, c, d) (   \
//     BW_HALF_1_QR(a, b, c, d), \
//     BW_HALF_2_QR(a, b, c, d))

// --------------------------------------RoundFunctionDefinition------------------------------------------------

/*
fw rounds
7
9
13
18
*/
class FORWARD
{
    public:
    // XOR version of full round functions, round means even or odd round
    void XRoundFunction(u32 *x, u32 round)
    {
        if (round & 1)
        {
            X_QR(x[0], x[4], x[8], x[12]);  // column 1
            X_QR(x[5], x[9], x[13], x[1]);  // column 2
            X_QR(x[10], x[14], x[2], x[6]); // column 3
            X_QR(x[15], x[3], x[7], x[11]); // column 4
        }
        else
        {
            X_QR(x[0], x[1], x[2], x[3]);     // row 1
            X_QR(x[5], x[6], x[7], x[4]);     // row 2
            X_QR(x[10], x[11], x[8], x[9]);   // row 3
            X_QR(x[15], x[12], x[13], x[14]); // row 4
        }
    }

    // half round funcitons
    // half 7-9
    void Half_1_EvenRF(u32 x[16])
    {
        FW_HALF_1_QR(x[0], x[1], x[2], x[3]);     // row 1
        FW_HALF_1_QR(x[5], x[6], x[7], x[4]);     // row 2
        FW_HALF_1_QR(x[10], x[11], x[8], x[9]);   // row 3
        FW_HALF_1_QR(x[15], x[12], x[13], x[14]); // row 4
    }
    // half 13-18
    void Half_2_EvenRF(u32 x[16])
    {
        FW_HALF_2_QR(x[0], x[1], x[2], x[3]);     // row 1
        FW_HALF_2_QR(x[5], x[6], x[7], x[4]);     // row 2
        FW_HALF_2_QR(x[10], x[11], x[8], x[9]);   // row 3
        FW_HALF_2_QR(x[15], x[12], x[13], x[14]); // row 4
    }
    // half 7-9
    void Half_1_OddRF(u32 x[16])
    {
        FW_HALF_1_QR(x[0], x[4], x[8], x[12]);  // column 1
        FW_HALF_1_QR(x[5], x[9], x[13], x[1]);  // column 2
        FW_HALF_1_QR(x[10], x[14], x[2], x[6]); // column 3
        FW_HALF_1_QR(x[15], x[3], x[7], x[11]); // column 4
    }
    // half  13-18
    void Half_2_OddRF(u32 x[16])
    {
        FW_HALF_2_QR(x[0], x[4], x[8], x[12]);  // column 1
        FW_HALF_2_QR(x[5], x[9], x[13], x[1]);  // column 2
        FW_HALF_2_QR(x[10], x[14], x[2], x[6]); // column 3
        FW_HALF_2_QR(x[15], x[3], x[7], x[11]); // column 4
    }

    // full round function, round means even or odd round
    void RoundFunction(u32 *x, u32 round)
    {
        if (round & 1)
        {
            Half_1_OddRF(x);
            Half_2_OddRF(x);
        }
        else
        {
            Half_1_EvenRF(x);
            Half_2_EvenRF(x);
        }
    }
};

/* bw rounds
18
13
9
7
*/

class BACKWARD
{
    public:
    // // XOR version of full round functions, round means even or odd round
    // void XRoundFunction(u32 *x, u32 round)
    // {
    //     if (round & 1)
    //     {
    //         X_QR(x[0], x[4], x[8], x[12]);  // column 1
    //         X_QR(x[5], x[9], x[13], x[1]);  // column 2
    //         X_QR(x[10], x[14], x[2], x[6]); // column 3
    //         X_QR(x[15], x[3], x[7], x[11]); // column 4
    //     }
    //     else
    //     {
    //         X_QR(x[0], x[1], x[2], x[3]);     // row 1
    //         X_QR(x[5], x[6], x[7], x[4]);     // row 2
    //         X_QR(x[10], x[11], x[8], x[9]);   // row 3
    //         X_QR(x[15], x[12], x[13], x[14]); // row 4
    //     }
    // }

    // half round funcitons
    // half 18-13
    void Half_1_EvenRF(u32 x[16])
    {
        BW_HALF_1_QR(x[0], x[1], x[2], x[3]);     // row 1
        BW_HALF_1_QR(x[5], x[6], x[7], x[4]);     // row 2
        BW_HALF_1_QR(x[10], x[11], x[8], x[9]);   // row 3
        BW_HALF_1_QR(x[15], x[12], x[13], x[14]); // row 4
    }
    // half 9-7
    void Half_2_EvenRF(u32 x[16])
    {
        BW_HALF_2_QR(x[0], x[1], x[2], x[3]);     // row 1
        BW_HALF_2_QR(x[5], x[6], x[7], x[4]);     // row 2
        BW_HALF_2_QR(x[10], x[11], x[8], x[9]);   // row 3
        BW_HALF_2_QR(x[15], x[12], x[13], x[14]); // row 4
    }
    // half 18-13
    void Half_1_OddRF(u32 x[16])
    {
        BW_HALF_1_QR(x[0], x[4], x[8], x[12]);  // column 1
        BW_HALF_1_QR(x[5], x[9], x[13], x[1]);  // column 2
        BW_HALF_1_QR(x[10], x[14], x[2], x[6]); // column 3
        BW_HALF_1_QR(x[15], x[3], x[7], x[11]); // column 4
    }
    // half 9-7
    void Half_2_OddRF(u32 x[16])
    {
        BW_HALF_2_QR(x[0], x[4], x[8], x[12]);  // column 1
        BW_HALF_2_QR(x[5], x[9], x[13], x[1]);  // column 2
        BW_HALF_2_QR(x[10], x[14], x[2], x[6]); // column 3
        BW_HALF_2_QR(x[15], x[3], x[7], x[11]); // column 4
    }

    // full round function, round means even or odd round
    void RoundFunction(u32 *x, u32 round)
    {
        if (round & 1)
        {
            Half_1_OddRF(x);
            Half_2_OddRF(x);
        }
        else
        {
            Half_1_EvenRF(x);
            Half_2_EvenRF(x);
        }
    }
};
