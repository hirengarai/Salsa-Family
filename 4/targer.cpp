#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <thread>

using namespace std;
#define ul unsigned int
#define ull unsigned long long

#define rotateleft(x, n) (((x) << (n)) ^ ((x) >> (32 - n)))
#define rotateright(x, n) (((x) >> (n)) ^ ((x) << (32 - n)))

#define update(a, b, n) ((rotateleft((a) ^ (b), (n))))

const ull mod = 4294967296;
#define MoD(x) ((x + mod) % mod)

// #define myrand32 ((ul)(4294967296.0 * drand48()))

ull LOOP, arrayofbias[256];

int l1 = 1, l2 = 0, l3 = 0, l = 0;

int setOfPNB[80]; // = {
//     219,
//     220,
//     221,
//     222,
//     223,
//     255,
//     77,
//     78,
//     79,
//     66,
//     67,
//     80,
//     68,
//     81,
//     102,
//     69,
//     82,
//     103,
//     70,
//     104,
//     83,
//     105,
//     71,
//     84,
//     106,
//     123,
//     124,
//     72,
//     85,
//     107,
//     125,
//     244,
//     191,
//     126,
//     127,
//     155,
//     2,
//     156,
//     3,
//     157,
//     224,
//     245,
//     108,
//     4,
//     158,
//     159,
//     73};

ul GetRandomNumber32()
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

// void initializeR(ul *x)
// {
//     int i;
//     for (i = 12; i < 16; ++i)
//     {
//         x[i] = GetRandomNumber32(); //IV
//     }
//     x[0] = 0x61707865;
//     x[1] = 0x3320646e;
//     x[2] = 0x79622d32;
//     x[3] = 0x6b206574;
// }
void copystate(ul *newState, ul *oldState)
{
    int i;
    for (i = 0; i < 16; i++)
        newState[i] = oldState[i];
}
void GetInitialState(ul *zeroState, ul *copyzeroState, ul *differencedState, ul *copydifferencedState)
{

    int i;
    for (int i = 0; i < 16; ++i)
    {
        zeroState[i] = GetRandomNumber32();
        copyzeroState[i] = zeroState[i];
        copyzeroState[0] = 0x61707865;
        copyzeroState[1] = 0x3320646e;
        copyzeroState[2] = 0x79622d32;
        copyzeroState[3] = 0x6b206574;
        differencedState[i] = zeroState[i];
        // x01[i] = x1[i];
    }
    // input difference
    ul pattern = 0x00000001 << 6;
    differencedState[13] = zeroState[13] ^ pattern;
    copystate(copydifferencedState, differencedState);
    copydifferencedState[0] = 0x61707865;
    copydifferencedState[1] = 0x3320646e;
    copydifferencedState[2] = 0x79622d32;
    copydifferencedState[3] = 0x6b206574;
    differencedState[13] = zeroState[13] ^ pattern;
    // ouput difference
    pattern = 0x00000001 << 2;
    differencedState[1] = zeroState[1] ^ pattern;
    pattern = 0x00000001 << 5;
    differencedState[5] = zeroState[5] ^ pattern;
    pattern = 0x00000001 << 9;
    differencedState[5] = zeroState[5] ^ pattern;
    pattern = 0x00000001 << 17;
    differencedState[5] = zeroState[5] ^ pattern;
    pattern = 0x00000001 << 29;
    differencedState[5] = zeroState[5] ^ pattern;
    pattern = 0x00000001 << 10;
    differencedState[9] = zeroState[9] ^ pattern;
    pattern = 0x00000001 << 22;
    differencedState[9] = zeroState[9] ^ pattern;
    pattern = 0x00000001 << 30;
    differencedState[9] = zeroState[9] ^ pattern;
    pattern = 0x00000001 << 30;
    differencedState[13] = zeroState[13] ^ pattern;
    pattern = 0x00000001 << 10;
    differencedState[13] = zeroState[13] ^ pattern;
}

void diffini(ul *x, int w, int b)
{
    int i;
    int pattern = 0x00000001 << b;
    x[w] = x[w] ^ pattern;
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

void firsthalfqr(ul *x0, ul *x1, ul *x2, ul *x3)
{
    ul b0, b1, b2, b3;
    b0 = MoD(*x0 + *x1);
    b3 = update(*x3, b0, 16);
    b2 = MoD(*x2 + b3);
    b1 = update(*x1, b2, 12);
    *x0 = b0, *x1 = b1, *x2 = b2, *x3 = b3;
}

void firstrevhalfqr(ul *z0, ul *z1, ul *z2, ul *z3)
{
    ul b0, b1, b2, b3;
    *z1 = rotateright(*z1, 7);
    b1 = *z1 ^ *z2;
    b2 = MoD(*z2 - *z3);

    *z3 = rotateright(*z3, 8);
    b3 = *z3 ^ *z0;
    b0 = MoD(*z0 - b1);
    *z0 = b0, *z1 = b1, *z2 = b2, *z3 = b3;
}

void secondhalfqr(ul *x0, ul *x1, ul *x2, ul *x3)
{
    ul z0, z1, z2, z3;
    z0 = MoD(*x0 + *x1);
    z3 = update(*x3, z0, 8);
    z2 = MoD(*x2 + z3);
    z1 = update(*x1, z2, 7);
    *x0 = z0, *x1 = z1, *x2 = z2, *x3 = z3;
}

void secondrevhalfqr(ul *z0, ul *z1, ul *z2, ul *z3)
{

    ul x0, x1, x2, x3;

    *z1 = rotateright(*z1, 12);
    x1 = *z1 ^ *z2;
    x2 = MoD(*z2 - *z3);

    *z3 = rotateright(*z3, 16);
    x3 = *z3 ^ *z0;
    x0 = MoD(*z0 - x1);

    *z0 = x0;
    *z1 = x1, *z2 = x2, *z3 = x3;
}

void f_evenrounds(ul *x)
{
    firsthalfqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
    firsthalfqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    firsthalfqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    firsthalfqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
}
void s_evenrounds(ul *x)
{
    secondhalfqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
    secondhalfqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    secondhalfqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    secondhalfqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
}

void f_oddrounds(ul *x)
{
    firsthalfqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
    firsthalfqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    firsthalfqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    firsthalfqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
}
void s_oddrounds(ul *x)
{
    secondhalfqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
    secondhalfqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    secondhalfqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    secondhalfqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
}

// reverse round
void f_revevenrounds(ul *x)
{
    firstrevhalfqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
    firstrevhalfqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    firstrevhalfqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    firstrevhalfqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
}
void s_revevenrounds(ul *x)
{
    secondrevhalfqr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
    secondrevhalfqr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
    secondrevhalfqr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
    secondrevhalfqr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
}

void f_revoddrounds(ul *x)
{
    firstrevhalfqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
    firstrevhalfqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    firstrevhalfqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    firstrevhalfqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
}
void s_revoddrounds(ul *x)
{
    secondrevhalfqr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
    secondrevhalfqr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
    secondrevhalfqr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
    secondrevhalfqr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
}

ul *xordiff(ul *x, ul *y, ul *z)
{
    for (int j = 0; j < 16; ++j)
    {
        z[j] = x[j] ^ y[j];
    }
    return z;
}

void add(ul *x, ul *x1)
{
    for (int j = 0; j < 16; ++j)
    {
        x[j] = (x[j] + x1[j]) % mod;
    }
}

void sub(ul *x, ul *x1)
{
    for (int j = 0; j < 16; ++j)
    {
        x[j] = MoD(x[j] - x1[j]);
    }
}

void chacha()
{
    ul x[16], x0[16], x1[16], x01[16], y0, z[16], z1[16], z2[16], r[16], d[16], key[8], diff, pattern, pt, i1, i2, i3, z11[16], z22[16], y1, y11;
    int i, j, k, j1, A[] = {4, 5, 6, 7, 8, 9, 10, 11}, m, n, o, p, w = 13, b = 6, count1, count2;
    ull loop{0};
    for (i = 0; i < 256; ++i)
    {
        arrayofbias[i] = 0;
    }
    while (loop < LOOP)
    {
        for (i = 1; i < 4; i++)
        {
            GetInitialState(x, x0, x1, x01);
            if ((i % 2) == 0)
            {
                f_oddrounds(x);
                s_oddrounds(x);

                f_oddrounds(x1);
                s_oddrounds(x1);
            }
            else
            {
                f_evenrounds(x);
                s_evenrounds(x);

                f_evenrounds(x1);
                s_evenrounds(x1);
            }
        }

        xordiff(x, x1, r);
        diff = r[2] ^ r[8] ^ (r[7] >> 7);
        // pattern = 0x00000001;

        for (i = 4; i < 7; i++)
        {
            if ((i % 2) == 0)
            {
                f_oddrounds(x);
                s_oddrounds(x);

                f_oddrounds(x1);
                s_oddrounds(x1);
            }
            else
            {
                f_evenrounds(x);
                s_evenrounds(x);

                f_evenrounds(x1);
                s_evenrounds(x1);
            }
        }

        add(x, x0);
        add(x1, x01);

        copystate(z11, x);
        copystate(z22, x1);

        // randomize the pnb
        for (i1 = 0; i1 < 256; i1++)
            for (i = 0; i < l; i++)    // l is basically the number of pnb's
                if (i1 == setOfPNB[i]) // C[i] is the list of pnb's
                {
                    if (drand48() < 0.5)
                    {
                        pt = 0x00000001;
                        i2 = A[i1 / 32]; // keyword
                        i3 = i1 % 32;    // keybit of the keyword
                        // printf("HERE %d  %d  %d\n", i2,i3,i1);
                        // pt = rotateleft(pt, i3);
                        pt = pt << i3;
                        // pt=0;

                        x0[i2] = x0[i2] ^ pt; // x0 is basically the initiall state
                        // here we are flipping the bits of the initial state

                        x01[i2] = x0[i2]; // copy
                    }

                } //*/
                  // basically this for loop checks if the pnb is in the list or not.
        for (i1 = 0; i1 < 256; i1++)
        {

            pt = 0;
            for (i = 0; i < l; i++)
                if (i1 == setOfPNB[i])
                {
                    pt = 1;
                    // printf("HERE\n");
                }
            // 👇🏾 means if the bit is pnb, then leave it
            if (pt == 1)
                continue;

            copystate(z1, z11);
            copystate(z2, z22);

            pattern = 0x00000001;

            i2 = A[i1 / 32]; // keyword
            i3 = i1 % 32;    // keybit
            pattern = pattern << i3;
            y0 = 0;
            if (drand48() <= 0.5)
            {
                x0[i2] = x0[i2] ^ pattern;
                x01[i2] = x0[i2];
                y0 = 1;
            }

            sub(z1, x0);
            sub(z2, x01);
            // undoing the bit flip 👇🏾
            if (y0 == 1)
            {
                x0[i2] = x0[i2] ^ pattern;
                x01[i2] = x0[i2];
            }
            // reverse round
            for (i = 7; i > 4; i--)
            {
                if ((i % 2) == 1)
                {
                    f_revoddrounds(z1);
                    s_revoddrounds(z1);

                    f_revoddrounds(z2);
                    s_revoddrounds(z2);
                }
                else
                {
                    f_revevenrounds(z1);
                    s_revevenrounds(z1);

                    f_revevenrounds(z2);
                    s_revevenrounds(z2);
                }
            }

            // difference
            pattern = 0x000000001;
            xordiff(z1, z2, d);
            z[11] = (d[2]) ^ (d[8]) ^ (d[7] >> 7) ^ diff;

            if ((z[11] & pattern) == 0)
            {
                arrayofbias[i1] += 1;
            }
        }

        loop++;
    }
}

// void *chacha()
// {

//     ul x[16], x0[16], x1[16], x01[16], y0, z[16], z1[16], z2[16], r[16], d[16], key[8], diff, pattern, pt, i1, i2, i3, z11[16], z22[16], y1, y11;
//     int i, j, k, j1, A[] = {4, 5, 6, 7, 8, 9, 10, 11}, m, n, o, p, w = 13, b = 6, count1, count2;
//     ull loop{0};

// }

int main()
{
    std::ofstream myfile;
    myfile.open("75propnbset.txt");

    srand48(time(NULL));

    int i, i0;
    int A[] = {4, 5, 6, 7, 8, 9, 10, 11}; // keywords position in the original state matrix
    double rr;

    for (l = 0; l < 75; ++l)
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

        chacha();

        rr = 0;

        for (i = 0; i < 256; ++i)
            if ((double)arrayofbias[i] / LOOP > rr)
            {
                rr = (double)arrayofbias[i] / LOOP;
                i0 = i;
            }
        printf("\n------------------------------\n");
        myfile << "\n------------------------------\n";
        printf("cell-> %d || bit -> %d || bias -> %0.10f", A[i0 / 32], i0 % 32, (double)arrayofbias[i0] / LOOP);
        myfile << "cell->" << A[i0 / 32] << "|| bit->" << i0 % 32 << " || bias->" << (double)arrayofbias[i0] / LOOP;
        printf("\n------------------------------\n");
        myfile << "\n------------------------------\n";
        setOfPNB[l] = i0;

        printf("The PNB set is \n");
        myfile << "The PNB set is \n";
        printf("{");
        myfile << "{";
        for (i = 0; i <= l; ++i)
            printf("%d,", setOfPNB[i]);
        myfile << setOfPNB[i];
        printf("}");
        myfile << "}";
        printf("\nNumber of the PNB's = %d", l + 1);
        myfile << "\n"
               << "Number of the PNB's" << l + 1 << "\n";
        printf("\n******************************\n");
        myfile << "\n"
               << "******************************"
               << "\n";
    }
    myfile.close();
}