#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define ul unsigned int
#define ull unsigned long long

#define rotateleft(x, n) (((x) << (n)) ^ ((x) >> (32 - n)))
#define rotateright(x, n) (((x) >> (n)) | ((x) << (32 - n)))

#define update(a, b, n) ((rotateleft((a) ^ (b), (n))))

#define BIASMAXLIMIT 0.51
#define BIASMINLIMIT 0.49

ull LOOP, val[256], val1[256];

ull MOD = 4294967296;

int POS[50], BIT[50];
ull Bias[256];
ull onex[16];

int l1 = 1, l2 = 0, l3 = 0, l = 0;
int i2, i3;
int C[80];

ul myrand32()
{
    ull temp;
    int i;
    if (drand48() < 0.5)
        temp = 0;
    else
        temp = 1;

    for (i = 1; i < 32; i++)
    {
        if (drand48() < 0.5)
            temp = temp << 1;
        else
            temp = (temp << 1) + 1;
    }
    return temp;
}

void initializeR(ul *x)
{
    int i;
    for (i = 0; i < 16; i++)
        x[i] = myrand32();

    x[0] = 0x61707865;
    x[1] = 0x3320646e;
    x[2] = 0x79622d32;
    x[3] = 0x6b206574;
}

void copystate(ul *x1, ul *x)
{
    int i;
    for (i = 0; i < 16; i++)
        x1[i] = x[i];
}

void print(ul *x)
{
    int i;
    for (i = 0; i < 16; i++)
    {
        printf("%8x ", x[i]);
        if (i > 0 && i % 4 == 3)
            printf("\n");
    }
    printf("\n");
}

void qr(ul *x0, ul *x1, ul *x2, ul *x3)
{

    ul z0, z1, z2, z3;
    z0 = *x0 + *x1;
    z3 = update(*x3, z0, 16);
    z2 = *x2 + z3;
    z1 = update(*x1, z2, 12);
    z0 = z0 + z1;
    z3 = update(z3, z0, 8);
    z2 = z2 + z3;
    z1 = update(z1, z2, 7);

    *x0 = z0;
    *x1 = z1, *x2 = z2, *x3 = z3;
}

void halfqr(ul *x0, ul *x1, ul *x2, ul *x3)
{
    ul z0, z1, z2, z3;
    z0 = *x0 + *x1;
    z3 = update(*x3, z0, 16);
    z2 = *x2 + z3;
    z1 = update(*x1, z2, 12);

    *x0 = z0;
    *x1 = z1, *x2 = z2, *x3 = z3;
}

void roundodd(ul *x)
{

    qr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
    qr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    qr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    qr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
    // transpose(x);
}

void halfroundodd(ul *x)
{

    halfqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
    halfqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    halfqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    halfqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
    // transpose(x);
}
void roundeven(ul *x)
{

    qr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
    qr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    qr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    qr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
    // transpose(x);
}
void halfroundeven(ul *x)
{

    halfqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
    halfqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    halfqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    halfqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
    // transpose(x);
}

void inhalfqr(ul *x0, ul *x1, ul *x2, ul *x3)
{
    ul p, q, r, s;

    p = *x0;
    q = *x1;
    r = *x2;
    s = *x3;

    // q = rotateright(q,7); q ^= r; r -= s;
    // s = rotateright(s,8); s ^= p; p -= q;

    q = rotateright(q, 12);
    q ^= r;
    r -= s;
    s = rotateright(s, 16);
    s ^= p;
    p -= q;
    // p += q; s ^= p; s = rotateleft(s,16);
    // r += s; q ^= r; q = rotateleft(q,12);
    // p += q; s ^= p; s = rotateleft(s,8);
    // r += s; q ^= r; q = rotateleft(q,7);

    *x0 = p;
    *x1 = q;
    *x2 = r;
    *x3 = s;
}

void inhalfqr_NEW(ul *x0, ul *x1, ul *x2, ul *x3)
{
    ul p, q, r, s;

    p = *x0;
    q = *x1;
    r = *x2;
    s = *x3;

    q = rotateright(q, 7);
    q ^= r;
    r -= s;
    s = rotateright(s, 8);
    s ^= p;
    p -= q;

    // q = rotateright(q,12); q ^= r; r -= s;
    // s = rotateright(s,16); s ^= p; p -= q;
    // p += q; s ^= p; s = rotateleft(s,16);
    // r += s; q ^= r; q = rotateleft(q,12);
    // p += q; s ^= p; s = rotateleft(s,8);
    // r += s; q ^= r; q = rotateleft(q,7);

    *x0 = p;
    *x1 = q;
    *x2 = r;
    *x3 = s;
}

void inhalfroundeven(ul *x)
{

    inhalfqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
    inhalfqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    inhalfqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    inhalfqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));

    // transpose(x);
}

void inhalfroundeven_NEW(ul *x)
{

    inhalfqr_NEW(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
    inhalfqr_NEW(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    inhalfqr_NEW(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    inhalfqr_NEW(&(x[0]), &(x[5]), &(x[10]), &(x[15]));

    // transpose(x);
}

void inqr(ul *x0, ul *x1, ul *x2, ul *x3)
{
    ul p, q, r, s;

    p = *x0;
    q = *x1;
    r = *x2;
    s = *x3;

    q = rotateright(q, 7);
    q ^= r;
    r -= s;
    s = rotateright(s, 8);
    s ^= p;
    p -= q;
    q = rotateright(q, 12);
    q ^= r;
    r -= s;
    s = rotateright(s, 16);
    s ^= p;
    p -= q;
    // p += q; s ^= p; s = rotateleft(s,16);
    // r += s; q ^= r; q = rotateleft(q,12);
    // p += q; s ^= p; s = rotateleft(s,8);
    // r += s; q ^= r; q = rotateleft(q,7);

    *x0 = p;
    *x1 = q;
    *x2 = r;
    *x3 = s;
}

void inroundo(ul *x)
{

    inqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
    inqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    inqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    inqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
}

void inrounde(ul *x)
{
    inqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
    inqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    inqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    inqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
}

void inrounds(ul *x)
{

    inqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
    inqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    inqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    inqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
    inqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
    inqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    inqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    inqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
}

void ChaCha()
{

    ul x[16], x0[16], x1[16], x01[16], y0, z[16], z1[16], z2[16], diff, pattern, pt, i1, i2, i3, z11[16], z22[16], y1, y11;
    int i, j, k, j1, A[] = {4, 5, 6, 7, 8, 9, 10, 11}, m, n, o, p;
    ull loop = 0;

    for (i = 0; i < 256; i++)
    {
        val[i] = 0;
        val1[i] = 0;
    }

    ull MOD = 4294967296;
    loop = 0;
    while (loop < LOOP)
    {
        while (1)
        {
            initializeR(x);

            copystate(x1, x);

            pt = 0x00000001 << 6;

            x1[13] = x1[13] ^ pt;

            copystate(x0, x);
            copystate(x01, x1);

            for (i = 0; i < 1; i++)
            {
                if ((i % 2) == 0)
                {
                    roundodd(x);
                    roundodd(x1);
                }
                else
                {
                    roundeven(x);
                    roundeven(x1);
                }
            }

            j1 = 0;

            for (k = 0; k < 16; k = k + 1)
            {
                pattern = 0x80000000;
                for (j = 31; j >= 0; j--)
                {
                    if (((x[k] ^ x1[k]) & pattern) != 0)
                        j1 = j1 + 1;
                    pattern = pattern >> 1;
                }
            }

            if (j1 > 10)
                continue;
            break;
        }

        copystate(x, x0);
        copystate(x1, x01);
        // printf("%u\n", x[1]);

        for (i = 0; i < 4; i++)
        {
            if ((i % 2) == 0)
            {
                roundodd(x);
                roundodd(x1);
            }
            else
            {
                roundeven(x);
                roundeven(x1);
            }
        }
        diff = x[2] ^ x1[2] ^ x[8] ^ x1[8] ^ (x[7] >> 7) ^ (x1[7] >> 7);
        for (i = 4; i < 7; i++)
        {
            if ((i % 2) == 0)
            {
                roundodd(x);
                roundodd(x1);
            }
            else
            {
                roundeven(x);
                roundeven(x1);
            }
        }

        for (i = 0; i < 16; i++)
        {
            z1[i] = (x[i] + x0[i]);
            z2[i] = (x1[i] + x01[i]);
        }

        copystate(z11, z1);
        copystate(z22, z2);

        // copystate(y0,x0);
        // copystate(y01,x01);

        for (i1 = 0; i1 < 256; i1++)
            for (i = 0; i < l; i++)
                if (i1 == C[i])
                {
                    if (drand48() < 0.5)
                    {
                        pt = 0x00000001;
                        i2 = A[i1 / 32];
                        i3 = i1 % 32;
                        // printf("HERE %d  %d  %d\n", i2,i3,i1);
                        pt = rotateleft(pt, i3);
                        // pt=0;

                        x0[i2] = x0[i2] ^ pt;

                        x01[i2] = x0[i2];
                    }

                } //*/

        for (i1 = 0; i1 < 256; i1++)
        {
            pt = 0;
            for (i = 0; i < l; i++)
                if (i1 == C[i])
                {
                    pt = 1;
                    // printf("HERE\n");
                }
            if (pt == 1)
                continue;

            copystate(z1, z11);
            copystate(z2, z22);

            pt = 0x00000001;

            // i2=POS[i];
            // i3=BIT[i];
            i2 = A[i1 / 32];
            i3 = i1 % 32;
            // printf("%d  %d  %d\n", i2,i3,i1);
            pt = rotateleft(pt, i3);
            // pt=0;
            y0 = 0;
            if (drand48() <= 0.5)
            {
                x0[i2] = x0[i2] ^ pt;
                // }

                x01[i2] = x0[i2];
                y0 = 1;
            }
            //}

            for (i = 0; i < 16; i++)
            {
                z1[i] = z1[i] + (MOD - (x0[i]));
                z2[i] = z2[i] + (MOD - (x01[i]));
            }

            if (y0 == 1)
            {
                x0[i2] = x0[i2] ^ pt;

                x01[i2] = x0[i2];
            }

            for (i = 7; i > 4; i--)
            {
                if ((i % 2) == 1)
                {
                    inroundo(z1);
                    inroundo(z2);
                }
                else
                {
                    inrounde(z1);
                    inrounde(z2);
                }
            }

            // z[1]= z[1] ^ z1[1] ^ z2[1];

            y1 = z1[2] ^ z1[8] ^ (z1[7] >> 7);
            y11 = z2[2] ^ z2[8] ^ (z2[7] >> 7);

            z[11] = (y1 ^ y11 ^ diff);

            // z[2]= rotateright(z[2],0);

            pattern = 0x000000001;
            if ((z[11] & pattern) == 0)
            {
                val[i1] = val[i1] + 1;
                // pattern = 0x80000000;
                z[11] = (y1 ^ y11 ^ diff);
                z[11] = rotateright(z[11], 1);
                if ((z[11] & pattern) == 0)
                {

                    val1[i1] = val1[i1] + 1;
                }
            }
        }

        loop++;
    }
}

int main()
{

    srand48(time(NULL));

    int i, i0;
    int A[] = {4, 5, 6, 7, 8, 9, 10, 11};
    double rr;

    for (l = 0; l < 80; l++)
    {
        if (l < 35)
            LOOP = 75000; // pow(2,17)
        if (l >= 35)
            LOOP = 600000; // pow(2,20)
        if (l >= 45)
            LOOP = 6000000; // pow(2,23)
        if (l >= 60)
            LOOP = 10000000; // pow(2,24)
        if (l >= 68)
            LOOP = 30000000; // pow(2,25)

        ChaCha();
        rr = 0;
        // for(i=0;i<256;i++)
        //   if((double)val1[i]/val[i]>0.51)
        //   printf("%0.20f  %d  %d\n", (double)val1[i]/val[i],  A[i/32], i%32);
        // printf("\n Bias %0.20f %d %d %d  \n", (double)Bias/LOOP,l1,l2,l3);
        //}

        for (i = 0; i < 256; i++)
            if ((double)val1[i] / val[i] > rr)
            {
                rr = (double)val1[i] / val[i];
                i0 = i;
            }
        printf("\n------------------------------\n");
        printf("%0.20f  %d  %d   %0.20f\n", (double)val1[i0] / val[i0], A[i0 / 32], i0 % 32, (double)val[i0] / LOOP);

        printf("\n------------------------------\n");

        C[l] = i0;

        for (i = 0; i <= l; i++)
            printf("%d, ", C[i]);
        printf("   %d", l + 1);
        printf("\n------------------------------\n");
    }
}
