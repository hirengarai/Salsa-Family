/*
 * REFERENCE IMPLEMENTATION OF chacha.h header file
 *
 * Filename: chacha.h
 *
 * created: 23/09/23
 * updated: 01/02/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains functions that implement the bare minimum of ChaCha cipher
 */

#include "commonfiles2.h"

// QR helping functions
#define ROTATE_RIGHT(x, n) (((x) >> (n)) ^ ((x) << (WORD_SIZE - n)))
#define UPDATE(a, b, n) (ROTATE_LEFT((a) ^ (b), (n)))

// ---------------------------FW QR-----------------------------------
#define FWDARX16(a, b, c, d, X) \
  (X) ? (a ^= b) : (a += b);    \
  d = UPDATE(a, d, 16);

#define FWDARX12(a, b, c, d, X) \
  (X) ? (c ^= d) : (c += d);    \
  b = UPDATE(b, c, 12);

#define FWDARX8(a, b, c, d, X) \
  (X) ? (a ^= b) : (a += b);   \
  d = UPDATE(a, d, 8);

#define FWDARX7(a, b, c, d, X) \
  (X) ? (c ^= d) : (c += d);   \
  b = UPDATE(b, c, 7);

// #1st half
#define FWDARX_16_12(a, b, c, d, X) \
  FWDARX16(a, b, c, d, X);          \
  FWDARX12(a, b, c, d, X);

// #2nd half
#define FWDARX_8_7(a, b, c, d, X) \
  FWDARX8(a, b, c, d, X);         \
  FWDARX7(a, b, c, d, X);

// full QR
#define FWDQR_16_12_8_7(a, b, c, d, X) \
  FWDARX_16_12(a, b, c, d, X);         \
  FWDARX_8_7(a, b, c, d, X);

// ---------------------------BW QR-----------------------------------
#define BWDARX7(a, b, c, d, X) \
  b = ROTATE_RIGHT(b, 7) ^ c;  \
  (X) ? (c ^= d) : (c -= d);

#define BWDARX8(a, b, c, d, X) \
  d = ROTATE_RIGHT(d, 8) ^ a;  \
  (X) ? (a ^= b) : (a -= b);

#define BWDARX12(a, b, c, d, X) \
  b = ROTATE_RIGHT(b, 12) ^ c;  \
  (X) ? (c ^= d) : (c -= d);

#define BWDARX16(a, b, c, d, X) \
  d = ROTATE_RIGHT(d, 16) ^ a;  \
  (X) ? (a ^= b) : (a -= b);

// #1st half
#define BWQR_7_8(a, b, c, d, X) \
  BWDARX7(a, b, c, d, X);       \
  BWDARX8(a, b, c, d, X);

// #2nd half
#define BWQR_12_16(a, b, c, d, X) \
  BWDARX12(a, b, c, d, X);        \
  BWDARX16(a, b, c, d, X)

// full QR
#define BWQR_7_8_12_16(a, b, c, d, X) \
  BWQR_7_8(a, b, c, d, X);            \
  BWQR_12_16(a, b, c, d, X);

// -------------------------------------- RoundFunctionDefinition ------------------------------------------------
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
      for (int index{0}; index < 4; ++index)
      {
        int j{0};
        FWDQR_16_12_8_7(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], true);
      }
    }
    else
    {
      for (int index{0}; index < 4; ++index)
      {
        int j{0};
        FWDQR_16_12_8_7(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], true);
      }
    }
  }

  void EVENARX_16(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX16(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_16(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX16(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }

  void EVENARX_12(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX12(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_12(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX12(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }

  void EVENARX_8(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX8(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_8(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX8(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }

  void EVENARX_7(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX7(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_7(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      FWDARX7(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }
  void Half_1_EvenRF(u32 *x)
  {
    EVENARX_16(x);
    EVENARX_12(x);
  }
  void Half_1_OddRF(u32 *x)
  {
    ODDARX_16(x);
    ODDARX_12(x);
  }

  void Half_2_EvenRF(u32 *x)
  {
    EVENARX_8(x);
    EVENARX_7(x);
  }

  void Half_2_OddRF(u32 *x)
  {
    ODDARX_8(x);
    ODDARX_7(x);
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
} frward;

/* bw rounds
7
8
12
16
*/

class BACKWARD
{
public:
  void EVENARX_7(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX7(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_7(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX7(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }

  void EVENARX_8(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX8(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_8(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX8(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }

  void EVENARX_12(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX12(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_12(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX12(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }

  void EVENARX_16(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX16(x[ChaCha::diagonal[index][j]], x[ChaCha::diagonal[index][j + 1]], x[ChaCha::diagonal[index][j + 2]], x[ChaCha::diagonal[index][j + 3]], false);
    }
  }

  void ODDARX_16(u32 *x)
  {
    for (int index{0}; index < 4; ++index)
    {
      int j{0};
      BWDARX16(x[ChaCha::column[index][j]], x[ChaCha::column[index][j + 1]], x[ChaCha::column[index][j + 2]], x[ChaCha::column[index][j + 3]], false);
    }
  }
  void Half_1_EvenRF(u32 *x)
  {
    EVENARX_7(x);
    EVENARX_8(x);
  }
  void Half_1_OddRF(u32 *x)
  {
    ODDARX_7(x);
    ODDARX_8(x);
  }

  void Half_2_EvenRF(u32 *x)
  {
    EVENARX_12(x);
    EVENARX_16(x);
  }

  void Half_2_OddRF(u32 *x)
  {
    ODDARX_12(x);
    ODDARX_16(x);
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
  // void Half_1_EvenRF(u32 *x)
  // {
  //   BWQR_7_8((x[0]), (x[5]), (x[10]), (x[15]), false); // diagonal 1
  //   BWQR_7_8((x[1]), (x[6]), (x[11]), (x[12]), false); // diagonal 2
  //   BWQR_7_8((x[2]), (x[7]), (x[8]), (x[13]), false);  // diagonal 3
  //   BWQR_7_8((x[3]), (x[4]), (x[9]), (x[14]), false);  // diagonal 4
  // }
  // // half 12-16
  // void Half_2_EvenRF(u32 *x)
  // {
  //   BWQR_12_16((x[0]), (x[5]), (x[10]), (x[15]), false); // diagonal 1
  //   BWQR_12_16((x[1]), (x[6]), (x[11]), (x[12]), false); // diagonal 2
  //   BWQR_12_16((x[2]), (x[7]), (x[8]), (x[13]), false);  // diagonal 3
  //   BWQR_12_16((x[3]), (x[4]), (x[9]), (x[14]), false);  // diagonal 4
  // }
  // // half 7-8
  // void Half_1_OddRF(u32 *x)
  // {
  //   BWQR_7_8((x[0]), (x[4]), (x[8]), (x[12]), false);  // column 1
  //   BWQR_7_8((x[1]), (x[5]), (x[9]), (x[13]), false);  // column 2
  //   BWQR_7_8((x[2]), (x[6]), (x[10]), (x[14]), false); // column 3
  //   BWQR_7_8((x[3]), (x[7]), (x[11]), (x[15]), false); // column 4
  // }
  // // half 12-16
  // void Half_2_OddRF(u32 *x)
  // {
  //   BWQR_12_16((x[0]), (x[4]), (x[8]), (x[12]), false);  // column 1
  //   BWQR_12_16((x[1]), (x[5]), (x[9]), (x[13]), false);  // column 2
  //   BWQR_12_16((x[2]), (x[6]), (x[10]), (x[14]), false); // column 3
  //   BWQR_12_16((x[3]), (x[7]), (x[11]), (x[15]), false); // column 4
  // }

  // // round numbering means even or odd round
  // // full round
  // void RoundFunction(u32 *x, u32 round)
  // {
  //   if (round & 1)
  //   {
  //     Half_1_OddRF(x);
  //     Half_2_OddRF(x);
  //   }
  //   else
  //   {
  //     Half_1_EvenRF(x);
  //     Half_2_EvenRF(x);
  //   }
  // }
} backward;
