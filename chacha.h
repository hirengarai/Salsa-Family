/*
 *  REFERENCE IMPLEMENTATION OF chacha.h header file
 *
 * Filename: chacha.h
 *
 * created: 23/09/23
 * updated: 23/09/23
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 * 
 * Synopsis:
 *  This file contains functions that implement the ChaCha round functon
 */

#include "commonfiles.h"

// QR helping functions
#define ROTATE_RIGHT(x, n) (((x) >> (n)) ^ ((x) << (WORD_SIZE - n)))
#define UPDATE(a, b, n) (ROTATE_LEFT((a) ^ (b), (n)))
// ---------------------------FW QR-----------------------------------
#define FWQR16(a, b, c, d) \
  a = a + b;               \
  d = UPDATE(a, d, 16);

#define XFWQR16(a, b, c, d) \
  a = a ^ b;                \
  d = UPDATE(a, d, 16);

#define FWQR12(a, b, c, d) \
  c = c + d;               \
  b = UPDATE(b, c, 12);

#define XFWQR12(a, b, c, d) \
  c = c ^ d;                \
  b = UPDATE(b, c, 12);

#define FWQR8(a, b, c, d) \
  a = a + b;              \
  d = UPDATE(a, d, 8);

#define XFWQR8(a, b, c, d) \
  a = a ^ b;               \
  d = UPDATE(a, d, 8);

#define FWQR7(a, b, c, d) \
  c = c + d;              \
  b = UPDATE(b, c, 7);

#define XFWQR7(a, b, c, d) \
  c = c ^ d;               \
  b = UPDATE(b, c, 7);

// #1st half
#define FWQR_16_12(a, b, c, d) \
  FWQR16(a, b, c, d);          \
  FWQR12(a, b, c, d);

#define XFWQR_16_12(a, b, c, d) \
  XFWQR16(a, b, c, d);          \
  XFWQR12(a, b, c, d);

// #2nd half
#define FWQR_8_7(a, b, c, d) \
  FWQR8(a, b, c, d);         \
  FWQR7(a, b, c, d);

#define XFWQR_8_7(a, b, c, d) \
  XFWQR8(a, b, c, d);         \
  XFWQR7(a, b, c, d);

// full QR
#define FWfullQR(a, b, c, d) \
  FWQR_16_12(a, b, c, d);    \
  FWQR_8_7(a, b, c, d);

#define X_QR(a, b, c, d)   \
  XFWQR_16_12(a, b, c, d); \
  XFWQR_8_7(a, b, c, d);

// ---------------------------BW QR-----------------------------------
#define BWQR7(a, b, c, d)     \
  b = ROTATE_RIGHT(b, 7) ^ c; \
  c = c - d;

#define BWQR8(a, b, c, d)     \
  d = ROTATE_RIGHT(d, 8) ^ a; \
  a = a - b;

#define BWQR12(a, b, c, d)     \
  b = ROTATE_RIGHT(b, 12) ^ c; \
  c = c - d;

#define BWQR16(a, b, c, d)     \
  d = ROTATE_RIGHT(d, 16) ^ a; \
  a = a - b;

// #1st half
#define BWQR_7_8(a, b, c, d) \
  BWQR7(a, b, c, d);         \
  BWQR8(a, b, c, d);

// #2nd half
#define BWQR_12_16(a, b, c, d) \
  BWQR12(a, b, c, d);          \
  BWQR16(a, b, c, d)

// full QR
#define BWfullQR(a, b, c, d) \
  BWQR_7_8(a, b, c, d);      \
  BWQR_12_16(a, b, c, d);

// --------------------------------------RoundFunctionDefinition------------------------------------------------

/*
fw rounds
16
12
8
7
*/
class FORWARD
{
public:
  // XOR version of full round functions, round means even or odd round
  void XRoundFunction(u32 *x, u32 round)
  {
    if (round & 1)
    {
      X_QR((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
      X_QR((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
      X_QR((x[2]), (x[6]), (x[10]), (x[14])); // column 3
      X_QR((x[3]), (x[7]), (x[11]), (x[15])); // column 4
    }
    else
    {
      X_QR((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
      X_QR((x[1]), (x[6]), (x[11]), (x[12])); // diagnoal 2
      X_QR((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
      X_QR((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
    }
  }

  // half round functions
  // half 16-12
  // void Half_1_EvenRF(u32 *x)
  // {
  //   FWQR_16_12((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
  //   FWQR_16_12((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
  //   FWQR_16_12((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
  //   FWQR_16_12((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  // }
  // // half 8-7
  // void Half_2_EvenRF(u32 *x)
  // {
  //   FWQR_8_7((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
  //   FWQR_8_7((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
  //   FWQR_8_7((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
  //   FWQR_8_7((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  // }
  // // half 16-12
  // void Half_1_OddRF(u32 *x)
  // {
  //   FWQR_16_12((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
  //   FWQR_16_12((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
  //   FWQR_16_12((x[2]), (x[6]), (x[10]), (x[14])); // column 3
  //   FWQR_16_12((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  // }
  // // half 8-7
  // void Half_2_OddRF(u32 *x)
  // {
  //   FWQR_8_7((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
  //   FWQR_8_7((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
  //   FWQR_8_7((x[2]), (x[6]), (x[10]), (x[14])); // column 3
  //   FWQR_8_7((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  // }

  void Quarter_1_EvenRF(u32 *x)
  {
    FWQR16((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    FWQR16((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    FWQR16((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    FWQR16((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  }

  void Quarter_1_OddRF(u32 *x)
  {
    FWQR16((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    FWQR16((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    FWQR16((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    FWQR16((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  }

  void Quarter_2_EvenRF(u32 *x)
  {
    FWQR12((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    FWQR12((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    FWQR12((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    FWQR12((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  }

  void Quarter_2_OddRF(u32 *x)
  {
    FWQR12((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    FWQR12((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    FWQR12((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    FWQR12((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  }

  void Quarter_3_EvenRF(u32 *x)
  {
    FWQR8((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    FWQR8((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    FWQR8((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    FWQR8((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  }

  void Quarter_3_OddRF(u32 *x)
  {
    FWQR8((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    FWQR8((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    FWQR8((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    FWQR8((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  }

  void Quarter_4_EvenRF(u32 *x)
  {
    FWQR7((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    FWQR7((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    FWQR7((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    FWQR7((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  }

  void Quarter_4_OddRF(u32 *x)
  {
    FWQR7((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    FWQR7((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    FWQR7((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    FWQR7((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  }
  void Half_1_EvenRF(u32 *x)
  {
    Quarter_1_EvenRF(x);
    Quarter_2_EvenRF(x);
  }
  void Half_1_OddRF(u32 *x)
  {
    Quarter_1_OddRF(x);
    Quarter_2_OddRF(x);
  }

  void Half_2_EvenRF(u32 *x)
  {
    Quarter_3_EvenRF(x);
    Quarter_4_EvenRF(x);
  }

  void Half_2_OddRF(u32 *x)
  {
    Quarter_3_OddRF(x);
    Quarter_4_OddRF(x);
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
7
8
12
16
*/

class BACKWARD
{
public:
  // // XOR function
  // void XFWRound(u32 *x, int round)
  // { // round numbering means even or odd round
  //   if (round & 1)
  //   {
  //     XFWfullQR((x[0]), (x[4]), (x[8]), (x[12]));
  //     XFWfullQR((x[1]), (x[5]), (x[9]), (x[13]));
  //     XFWfullQR((x[2]), (x[6]), (x[10]), (x[14]));
  //     XFWfullQR((x[3]), (x[7]), (x[11]), (x[15]));
  //   }
  //   else
  //   {
  //     XFWfullQR((x[0]), (x[5]), (x[10]), (x[15]));
  //     XFWfullQR((x[1]), (x[6]), (x[11]), (x[12]));
  //     XFWfullQR((x[2]), (x[7]), (x[8]), (x[13]));
  //     XFWfullQR((x[3]), (x[4]), (x[9]), (x[14]));
  //   }
  // }

  // half round functions
  // half 7-8
  void Half_1_EvenRF(u32 *x)
  {
    BWQR_7_8((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    BWQR_7_8((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    BWQR_7_8((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    BWQR_7_8((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  }
  // half 12-16
  void Half_2_EvenRF(u32 *x)
  {
    BWQR_12_16((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    BWQR_12_16((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    BWQR_12_16((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    BWQR_12_16((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
  }
  // half 7-8
  void Half_1_OddRF(u32 *x)
  {
    BWQR_7_8((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    BWQR_7_8((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    BWQR_7_8((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    BWQR_7_8((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  }
  // half 12-16
  void Half_2_OddRF(u32 *x)
  {
    BWQR_12_16((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    BWQR_12_16((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    BWQR_12_16((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    BWQR_12_16((x[3]), (x[7]), (x[11]), (x[15])); // column 4
  }

  // round numbering means even or odd round
  // full round
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
