#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define ul unsigned int
#define ull unsigned long long

#define rotateleft(x, n) (((x) << (n)) ^ ((x) >> (32 - n)))
#define rotateright(x, n) (((x) >> (n)) | ((x) << (32 - n)))
#define updatexor(a, b, n) ((rotateleft((a) ^ (b), (n))))
#define update(a, b, n) ((rotateleft((a) + (b), (n))))
#define revupdate(a, b, n) ((rotateright(a, n)) - b)

ull LOOP, val[256], val1[256];

ull MOD = 4294967296;

int POS[50], BIT[50];
ull Bias[256];
ull onex[16];

int l1 = 1, l2 = 0, l3 = 0, l = 0;
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

    x[6] = 0x61707865;
    x[7] = 0x3320646e;
    x[14] = 0x79622d32;
    x[15] = 0x6b206574;
}

void copystate(ul *x1, ul *x)
{
    int i;
    for (i = 0; i < 16; i++)
        x1[i] = x[i];
}

void qr(ul *x0, ul *x1, ul *x2, ul *x3, ul *x4)
{

    ul a, b, c, d, e;
    a = *x0, b = *x1, c = *x2, d = *x3, e = *x4;
    d = d + e;
    c = c ^ d;
    b = update(b, c, 10);
    a = a + b;
    e = e ^ a;
    d = update(d, e, 27);
    c = c + d;
    b = b ^ c;
    a = update(a, b, 8);

    *x0 = a;
    *x1 = b, *x2 = c, *x3 = d, *x4 = e;
}

void qrxor(ul *a, ul *b, ul *c, ul *d, ul *e)
{

    ul a1, b1, c1, d1, a2, b2, c2, d2, e2;
    d1 = *d ^ *e;
    c1 = *c ^ d1;
    b1 = updatexor(*b, c1, 10);
    a1 = *a ^ b1;
    e2 = *e ^ a1;
    d2 = updatexor(d1, e2, 27);
    c2 = c1 ^ d2;
    b2 = b1 ^ c2;
    a2 = updatexor(a1, b2, 8);

    *a = a2;
    *b = b2, *c = c2, *d = d2, *e = e2;
}

void roundodd(ul *x)
{

    qr(&(x[0]), &(x[4]), &(x[8]), &(x[12]), &(x[3]));
    qr(&(x[1]), &(x[5]), &(x[9]), &(x[13]), &(x[0]));
    qr(&(x[2]), &(x[6]), &(x[10]), &(x[14]), &(x[1]));
    qr(&(x[3]), &(x[7]), &(x[11]), &(x[15]), &(x[2]));
}

void roundeven(ul *x)
{

    qr(&(x[0]), &(x[5]), &(x[10]), &(x[15]), &(x[3]));
    qr(&(x[1]), &(x[6]), &(x[11]), &(x[12]), &(x[0]));
    qr(&(x[2]), &(x[7]), &(x[8]), &(x[13]), &(x[1]));
    qr(&(x[3]), &(x[4]), &(x[9]), &(x[14]), &(x[2]));
}

void roundxorodd(ul *x)
{

    qr(&(x[0]), &(x[4]), &(x[8]), &(x[12]), &(x[3]));
    qrxor(&(x[1]), &(x[5]), &(x[9]), &(x[13]), &(x[0]));
    qr(&(x[2]), &(x[6]), &(x[10]), &(x[14]), &(x[1]));
    qr(&(x[3]), &(x[7]), &(x[11]), &(x[15]), &(x[2]));
}

void inqr(ul *x0, ul *x1, ul *x2, ul *x3, ul *x4)
{

    ul a, b, c, d, e;
    a = *x0, b = *x1, c = *x2, d = *x3, e = *x4;
    a = revupdate(a, b, 8);
    b = b ^ c;
    c = c - d;
    d = revupdate(d, e, 27);
    e = e ^ a;
    a = a - b;
    b = revupdate(b, c, 10);
    c = c ^ d;
    d = d - e;
    *x0 = a;
    *x1 = b, *x2 = c, *x3 = d, *x4 = e;
}

void inroundo(ul *x)
{

    inqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]), &(x[2]));
    inqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]), &(x[1]));
    inqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]), &(x[0]));
    inqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]), &(x[3]));
}

void inrounde(ul *x)
{
    inqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]), &(x[2]));
    inqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]), &(x[1]));
    inqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]), &(x[0]));
    inqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]), &(x[3]));
}

void fhalfroundodd(ul *x)
{

    qr(&(x[0]), &(x[4]), &(x[8]), &(x[12]), &(x[3]));
    qr(&(x[1]), &(x[5]), &(x[9]), &(x[13]), &(x[0]));
}

void shalfroundodd(ul *x)
{

    qr(&(x[2]), &(x[6]), &(x[10]), &(x[14]), &(x[1]));
    qr(&(x[3]), &(x[7]), &(x[11]), &(x[15]), &(x[2]));
}

void reshalfroundodd(ul *x)
{

    inqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]), &(x[0]));
    inqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]), &(x[3]));
}

void refhalfroundodd(ul *x)
{

    inqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]), &(x[2]));
    inqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]), &(x[1]));
}

void fhalfroundeven(ul *x)
{
    qr(&(x[0]), &(x[5]), &(x[10]), &(x[15]), &(x[3]));
    qr(&(x[1]), &(x[6]), &(x[11]), &(x[12]), &(x[0]));
}

void shalfrouneven(ul *x)
{
    qr(&(x[2]), &(x[7]), &(x[8]), &(x[13]), &(x[1]));
    qr(&(x[3]), &(x[4]), &(x[9]), &(x[14]), &(x[2]));
}

void refhalfroundeven(ul *x)
{
    inqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]), &(x[2]));
    inqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]), &(x[1]));
}
void reshalfroundeven(ul *x)
{
    inqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]), &(x[0]));
    inqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]), &(x[3]));
}

int main()
{

    ul x[16], x0[16], x1[16], x01[16], x_ori[16], x1_ori[16], y[16], y1[16], z[16], z1[16], z_ori[16], z1_ori[16], diff1, diff2, finaldiff, pattern, pt, i1, i2, i3, z11[16], z22[16], temp, temp2;
    int i, j, k, j1, A[] = {0, 1, 2, 3, 8, 9, 10, 11}, m, n, o, p;
    ull loop = 0;
    LOOP = 100000;

    double threshold = 0.22;

    srand48(time(NULL));
    for (i = 0; i < 256; i++)
    {
        val[i] = 0;
        val1[i] = 0;
    }

    ull MOD = 4294967296;

    while (loop < LOOP)
    {

        initializeR(x);

        copystate(x1, x);
        copystate(x_ori, x);

        pt = 0x00000001 << 11;

        x1[5] = x[5] ^ pt;

        copystate(x1_ori, x1);

        roundxorodd(x);
        roundxorodd(x1);
        roundeven(x);
        roundeven(x1);

        // for (i = 1; i < 2; i++){if ((i%2)==0) {roundodd(x); roundodd(x1);} else {roundeven(x); roundeven(x1);}}

        // fhalfroundodd(x); fhalfroundodd(x1);
        // shalfroundodd(x); shalfroundodd(x1);

        diff1 = (x[10] << 0) ^ (x1[10] << 0);
        temp = x[10];

        roundodd(x);
        roundodd(x1);
        roundeven(x);
        roundeven(x1);
        roundodd(x);
        roundodd(x1);

        fhalfroundeven(x);
        fhalfroundeven(x1);

        for (i = 0; i < 16; i++)
        {

            z[i] = (x[i] + x_ori[i]);
            z1[i] = (x1[i] + x1_ori[i]);
        }

        copystate(z_ori, z);
        copystate(z1_ori, z1);

        for (i1 = 0; i1 < 256; i1++)
        {

            copystate(z, z_ori);
            copystate(z1, z1_ori);
            copystate(x0, x_ori);
            copystate(x01, x1_ori);

            pt = 0x00000001;

            i2 = A[i1 / 32];
            i3 = i1 % 32;

            // printf("x0[%d]=%u, x01[%d]=%u \n   ",i2,x0[i2],i2,x01[i2]);

            pt = rotateleft(pt, i3);

            // if(drand48()<0){

            x0[i2] = x0[i2] ^ pt;
            x01[i2] = x01[i2] ^ pt;

            // printf("x0[%d]=%u, x01[%d]=%u\n",i2,x01[i2],i2,x01[i2]);
            // printf("cell no.=%d\n",i2);

            //  }

            for (i = 0; i < 16; i++)
            {

                z[i] = z[i] + (MOD - (x0[i]));
                z1[i] = z1[i] + (MOD - (x01[i]));
            }

            reshalfroundeven(z);
            reshalfroundeven(z1);

            inroundo(z);
            inroundo(z1);
            inrounde(z);
            inrounde(z1);
            inroundo(z);
            inroundo(z1);

            //   reshalfroundodd(z); reshalfroundodd(z1);

            // for (i = 5; i > 2; i--)    {if ((i%2)==1) {inroundo(z); inroundo(z1);} else {inrounde(z); inrounde(z1);}}

            // refhalfroundodd(z); refhalfroundodd(z1);
            // reshalfroundodd(z); reshalfroundodd(z1);

            // inroundo(z); inroundo(z1);

            diff2 = (z[10] >> 0) ^ (z1[10] >> 0);
            temp2 = z[10];

            // finaldiff  = (diff1^diff2);

            // printf("%u  \n", finaldiff );
            val[i1] = val[i1] + 1;

            pattern = 0x000000001;

            if (((diff1 ^ diff2) & pattern) == 0)
            {

                val1[i1] = val1[i1] + 1;
            }
        }

        loop++;
    }

    int PNBcount = 0;
    for (i1 = 0; i1 < 256; i1++)
        if ((2 * (((double)val1[i1] / val[i1]) - 0.5)) > threshold)

        {
            PNBcount++;

            printf("bit=%d,bias=%0.10f\n", i1, 2 * (((double)val1[i1] / val[i1]) - 0.5));
        }

    printf("count=%d\n", PNBcount);
}
