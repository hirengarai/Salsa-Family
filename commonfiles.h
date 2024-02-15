/*
 * REFERENCE IMPLEMENTATION OF some common functions that is used in both Salsa and ChaCha cipher
 *
 * Filename: commonfiles.h
 *
 * created: 23/09/23
 * updated: 20/12/23
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains common functions that is used in both Salsa and ChaCha scheme.
 */
#include <algorithm>
#include <bitset>   // binary representation
#include <cmath>    // pow function
#include <fstream>  // files
#include <iostream> // cin cout
#include <cstring>  // string
#include <iomanip>  // decimal numbers upto certain places, setw function
#include <vector>   // vector

#define WORD_SIZE 32
#define MOD 4294967296 // pow(2, WORD_SIZE)
#define GET_BIT(a, p) (((a) >> (p)) & 0x1) // p-th bit of word

typedef uint16_t u16;           // positive integer of 16 bits
typedef uint32_t u32;           // positive integer of 32 bits
typedef unsigned long long ull; // 32 - 64 bits memory

// random number of 32 bits
static inline u32 drandom()
{
    return MOD * drand48();
}

#define ROTATE_LEFT(x, n) (((x) << (n)) ^ ((x) >> (WORD_SIZE - n)))

// u32 mtrandom(const int min, const int max)
// {
//     thread_local std::mt19937_64 engine(std::random_device{}());
//     std::uniform_int_distribution<int> dist(min, max);
//     return dist(engine);
// }

// double unitrandom()
// {
//     thread_local std::mt19937 engine(std::random_device{}());
//     std::uniform_int_distribution<> dist(0.0, 1.0);
//     return dist(engine);
// }

class INIT_IV
{
public:
    void salsa(u32 *x)
    {
        x[0] = 0x61707865;
        x[5] = 0x3120646e;
        x[10] = 0x79622d36;
        x[15] = 0x6b206574;
        for (int i{6}; i < 10; ++i)
        {
            x[i] = drandom(); // IV
        }
    }

    void chacha(u32 *x)
    {
        x[0] = 0x61707865;
        x[1] = 0x3320646e;
        x[2] = 0x79622d32;
        x[3] = 0x6b206574;

        // x[0] = 0;
        // x[1] = 0;
        // x[2] = 0;
        // x[3] = 0;
        for (int i{12}; i < 16; ++i)
        {
            x[i] = drandom(); // IV
                              //    x[i] = 0; // IV
        }
    }
};

class INIT_KEY
{
public:
    void Key256(u32 *k)
    {
        for (int i{0}; i < 8; ++i)
        {
            // k[i] = 0;
            k[i] = drandom();
        }
    }
    void Key128(u32 *k)
    {
        for (int i{0}; i < 4; ++i)
        {
            // k[i] = 0;
            k[i] = drandom();
            k[i + 4] = k[i];
        }
    }
};

class INSERT_KEY
{
public:
    // fitting the key k into the state matrix x
    void salsa(u32 *x, u32 *k)
    {
        for (int i{1}; i < 5; ++i)
            x[i] = k[i - 1];
        for (int i{11}; i < 15; ++i)
            x[i] = k[i - 7];
    }
    // fitting the key k into the state matrix x
    void chacha(u32 *x, u32 *k)
    {
        for (int i{4}; i < 12; ++i)
        {
            x[i] = k[i - 4];
        }
    }
};

class PUT_VALUE
{
public:
    int WORD, BIT;
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, count is the total element number in y
    void salsa(u32 *x, u32 *x1, u16 *y, int count, const char *str)
    {
        if (!(strcasecmp(str, "random")))
        {
            for (int i{0}; i < count; ++i)
            {
                if ((y[i] / 32) > 3)
                {
                    WORD = (y[i] / 32) + 7;
                }
                else
                {
                    WORD = (y[i] / 32) + 1;
                }
                BIT = y[i] % 32;

                if (drand48() > 0.5)
                {
                    x[WORD] = x[WORD] ^ (0x1 << BIT);
                    x1[WORD] = x1[WORD] ^ (0x1 << BIT);
                }
            }
        }
    }
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, count is the total element number in y
    void chacha(u32 *x, u32 *x1, u16 *y, int count, const char *str)
    {
        if (!(strcasecmp(str, "random")))
        {
            for (int i{0}; i < count; ++i)
            {
                WORD = (y[i] / 32) + 4;
                BIT = y[i] % 32;

                if (drand48() > 0.5)
                {
                    x[WORD] ^= (0x1 << BIT);
                    x1[WORD] ^= (0x1 << BIT);
                }
            }
        }
        else if (!(strcasecmp(str, "zero")))
        {
            for (int i{0}; i < count; ++i)
            {
                WORD = (y[i] / 32) + 4;
                BIT = y[i] % 32;

                x[WORD] &= ~(0x1 << BIT);
                x1[WORD] &= ~(0x1 << BIT);
            }
        }
    }
};

void BlockValue(u32 *x, u32 *x1, u16 *list1, u16 *list2, u16 list1_size, u16 list2_size)
{
    int WORD, BIT;
    for (int i{0}; i < list1_size; ++i)
    {
        WORD = (list1[i] / 32) + 4;
        BIT = list1[i] % 32;

        if ((i % 5) == 4)
        {
            x[WORD] |= (0x1 << BIT);
            x1[WORD] |= (0x1 << BIT);
        }
        else
        {
            x[WORD] &= ~(0x1 << BIT);
            x1[WORD] &= ~(0x1 << BIT);
        }
    }
    for (int i{0}; i < list2_size; ++i)
    {
        WORD = (list2[i] / 32) + 4;
        BIT = list2[i] % 32;

        if (drand48() > 0.5)
        {
            x[WORD] ^= (0x1 << BIT);
            x1[WORD] ^= (0x1 << BIT);
        }
    }
}

// sum of x and x1 is stored in x
void AddStates(u32 *x, u32 *x1)
{
    for (int i{0}; i < 16; ++i)
        x[i] += x1[i];
}

// x1 state is copied in x
void CopyState(u32 *x, u32 *x1, u32 n)
{
    for (int i{0}; i < n; ++i)
        x[i] = x1[i];
}

void InputDifference(u32 *x, int word, int bit)
{
    int pattern = 1 << bit;
    x[word] ^= pattern;
}

// function to store the no. of set bits in x state. str = column means columnwise
// check digit = 0,1,2/3 represents column number
int NumberOfDifferences(u32 *x, int check_digit, const char *str)
{
    int temp = 0;
    if (!strcmp(str, "column"))
    {
        for (int i{check_digit}; i < 16; i += 4)
        {
            for (int j{0}; j < 32; ++j)
            {
                int pattern = 0x1 << j;
                if (x[i] & pattern)
                    temp++;
            }
        }
        return temp;
    }

    else if (!strcmp(str, "diag1"))
    {
        for (int i{0}; i < 16; i += 5)
        {
            for (int j{0}; j < 32; ++j)
            {
                int pattern = 0x1 << j;
                if (x[i] & pattern)
                    temp++;
            }
        }
        return temp;
    }
    else
    {
        for (int i{0}; i < 16; ++i)
        {
            for (int j{0}; j < 32; ++j)
            {
                int pattern = 0x1 << j;
                if (x[i] & pattern)
                    temp++;
            }
        }
        return temp;
    }
}

// all the values of x are unset
void ResetState(u32 *x, u32 size)
{
    for (int i{0}; i < size; ++i)
    {
        x[i] = 0x0;
    }
}

// 4 cross 4 matrix form print
void PRINT_STATE(u32 *x, int size)
{
    for (int i{0}; i < size; ++i)
    {
        printf("%8x ", x[i]);
        if (i > 0 && i % 4 == 3)
            printf("\n");
    }
    printf("\n");
}

void PRINT(u16 *x, int size)
{
    for (int i{0}; i < size; ++i)
    {
        printf("%u ", x[i]);
    }
    printf("\n");
}

void BINARY_PRINT_STATE(u32 *x, int size)
{
    for (int i{0}; i < size; ++i)
    {
        std::bitset<32> temp(x[i]);
        std::cout << temp << "  ";
        if (i > 0 && i % 4 == 3)
            printf("\n");
    }
    printf("\n");
}

// subtraction of x1 from x is stored in x
void SubtractStates(u32 *x, u32 *x1)
{
    for (int i{0}; i < 16; ++i)
        x[i] -= x1[i];
}

// XOR of the states x and x1 is stored in y
void XORDifference(u32 *x, u32 *x1, u32 *y, int n)
{
    for (int i{0}; i < n; ++i)
        y[i] = x[i] ^ x1[i];
}

// randomise the PNBs
// int WORD, BIT;
// for (int j = 0; j < PNBcount; ++j)
// {
//   if ((PNB[j] / 32) > 3)
//   {
//     WORD = (PNB[j] / 32) + 7;
//   }
//   else
//   {
//     WORD = (PNB[j] / 32) + 1;
//   }
//   BIT = PNB[j] % 32;

//   if (drand48() < 0.5)
//   {
//     strdx0[WORD] = strdx0[WORD] ^ (0x1 << BIT);
//     dstrdx0[WORD] = dstrdx0[WORD] ^ (0x1 << BIT);
//   }
// }

void Block(u16 *list, u16 *listBlocks, u16 *rest, u16 BLOCKlen, u16 listsize, u16 *len)
{

    std::vector<u16> block;
    if (BLOCKlen > listsize)
        exit(EXIT_FAILURE);
    else
    {
        int i{0};
        len[0] = 0, len[1] = 0;
        while (i < listsize)
        {
            int counter{1};
            block.push_back(list[i]);
            i++;

            while ((i < listsize) && (list[i] - list[i - 1] == 1) && (counter < BLOCKlen))
            {
                block.push_back(list[i]);
                counter++;
                i++;
            }

            if (counter == BLOCKlen)
            {
                for (int j{0}; j < BLOCKlen; ++j)
                {
                    listBlocks[len[0]] = block.at(j);
                    len[0]++;
                    // len[0]++;
                }
            }
        }
        size_t listBlocks_size = sizeof(listBlocks) / sizeof(u16);
        u16 *end = listBlocks + listBlocks_size;
        for (int i{0}; i < listsize; ++i)
        {
            u16 *result = std::find(listBlocks, end, list[i]);
            if (result == end)
            {
                rest[len[1]] = list[i];
                len[1]++;
            }
        }
    }
}

void Details(const char *cipher, u16 totalround, u16 fwdround, u16 *IDword, u16 *IDbit, u16 IDsize, u16 *ODword, u16 *ODbit, u16 ODsize, size_t PNBcount, const char *mode, ull loopLim, ull ITERATION)
{
    std::cout << std::setw(4)<<"+------------------------------------------------------------------------------------+\n";
    std::cout << std::setw(4)<<"|            Backward bias determining programe                                 "<<std::setw(7)<<"|\n";
    std::cout << std::setw(4)<<"|            Cipher Name: " << cipher << ", # of total rounds: " << (unsigned)totalround << ", # of forward rounds: " << fwdround<< "  "<<std::setw(7)<<"|\n";
    std::cout << std::setw(4) << "|            ID = ";
    for (int i{0}; i < IDsize; ++i)
    {
        if (i == IDsize - 1)
            std::cout << "(" << IDword[i] << ", " << IDbit[i] << "), ";
        else
        {
            std::cout << "(" << IDword[i] << ", " << IDbit[i] << ")⊕";
        }
    }
    std::cout << "OD = ";
    for (int i{0}; i < ODsize; ++i)
    {
        if (i == ODsize - 1)
            std::cout << "(" << ODword[i] << ", " << ODbit[i] << ")                            "<<std::setw(7)<<"|\n";
        else
        {
            std::cout << "(" << ODword[i] << ", " << ODbit[i] << ")⊕";
        }
    }

    std::cout << std::setw(5) << "|            Values of PNBs are in " << mode << " mode                                  " << std::setw(7) <<"|\n";
    std::cout << std::setw(4)<<"|            Per loop size : 2^{" << log2(loopLim) << "}                                             "<<std::setw(7)<<"|\n";
    std::cout << std::setw(4)<<"|            Total loop count : 2^{" << log2(ITERATION / loopLim) << "}                                          "<<std::setw(7)<<"|\n";
}

u16 *OpenFile(const char *file, size_t *PNBcount)
{
    std::ifstream pnb(file);

    if (!pnb.is_open())
    {
        std::cout << "|          ⚠ Unable to open the file ⚠                                          "<<std::setw(7)<<"|\n";
        exit(1);
    }
    else
    {
        u16 n;
        while (pnb >> n)
        {
            *PNBcount = n;
        }
    }
    pnb.close();
    pnb.open(file);

    u16 *arr = new u16[*PNBcount], index{0};

    while (!pnb.eof())
    {
        pnb >> arr[index++];
    }
    pnb.close();

    return arr;
}

u16 **OpenFile(const char *file, size_t *blockCount, size_t *restCount)
{
    std::ifstream pnb(file);

    if (!pnb.is_open())
    {
        std::cout << "|          ⚠ Unable to open the file ⚠                                         "<<std::setw(7)<<"|\n";
        exit(1);
    }

    u16 last, second_last, current;
    while (pnb >> current)
    {
        second_last = last;
        last = current;
    }

    *blockCount = second_last;
    *restCount = last;

    u16 **arr = new u16 *[2];

    arr[0] = new u16[*blockCount];
    arr[1] = new u16[*restCount];

    pnb.close();
    pnb.open(file);

    for (int i{0}; i < *blockCount; ++i)
        pnb >> arr[0][i];
    for (int j{0}; j < *restCount; ++j)
        pnb >> arr[1][j];

    pnb.close();

    return arr;
}
