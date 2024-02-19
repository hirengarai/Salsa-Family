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
