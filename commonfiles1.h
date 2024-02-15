/*
 * REFERENCE IMPLEMENTATION OF some common functions that is used in both Salsa and ChaCha cipher
 *
 * Filename: commonfiles.h
 *
 * created: 23/09/23
 * updated: 31/12/23
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains some common functions that is used both in Salsa and ChaCha scheme.
 */
#include <algorithm>
#include <bitset>   // binary representation
#include <cmath>    // pow function
#include <cstring>  // string
#include <fstream>  // files
#include <iostream> // cin cout
#include <iomanip>  // decimal numbers upto certain places, setw function
#include <vector>   // vector

#define WORD_SIZE 32
#define MOD 4294967296                     // pow(2, WORD_SIZE)
#define GET_BIT(a, p) (((a) >> (p)) & 0x1) // p-th bit of word a

typedef uint16_t u16;           // positive integer of 16 bits
typedef uint32_t u32;           // positive integer of 32 bits
typedef unsigned long long ull; // 32 - 64 bits memory

// random number of 32 bits
static inline u32 drandom()
{
    return MOD * drand48();
}

#define ROTATE_LEFT(x, n) (((x) << (n)) ^ ((x) >> (WORD_SIZE - n)))

// IV inisialisation
class INITIALISE_IV
{
public:
    void salsa(u32 *x = nullptr)
    {
        if (x == nullptr)
        {
            for (int i{0}; i < 16; ++i)
                default_values[i] = 0;
            x = default_values;
        }
        else
        {
            x[0] = 0x61707865;
            x[5] = 0x3120646e;
            x[10] = 0x79622d36;
            x[15] = 0x6b206574;
            for (int i{6}; i < 10; ++i)
                x[i] = drandom(); // IV
        }
    }

    void chacha(u32 *x = nullptr)
    {
        if (x == nullptr)
        {
            for (int i{0}; i < 16; ++i)
                default_values[i] = 0;
            x = default_values;
        }
        else
        {
            x[0] = 0x61707865;
            x[1] = 0x3320646e;
            x[2] = 0x79622d32;
            x[3] = 0x6b206574;
            for (int i{12}; i < 16; ++i)
                x[i] = drandom(); // IV
        }
    }
    u32 default_values[16];
};

// Inisialise key
class INITIALISE_KEY
{
public:
    void Key256(u32 *k = nullptr)
    {
        if (k == nullptr)
        {
            for (int i{0}; i < 8; ++i)
                default_values[i] = 0;
            k = default_values;
        }
        else
        {
            for (int i{0}; i < 8; ++i)
                k[i] = drandom();
        }
    }
    void Key128(u32 *k = nullptr)
    {
        if (k == nullptr)
        {
            for (int i{0}; i < 4; ++i)
            {
                default_values[i] = 0;
                default_values[i + 4] = default_values[i];
            }
            k = default_values;
        }
        else
        {
            for (int i{0}; i < 4; ++i)
            {
                k[i] = drandom();
                k[i + 4] = k[i];
            }
        }
    }
    u32 default_values[8];
};

// Insering key into the state matrix
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
            x[i] = k[i - 4];
    }
};

// Values for PNBs
class PUT_VALUE
{
public:
    int WORD, BIT;
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, count is the total element number in y
    void salsa(u32 *x, u32 *x1, u16 *y = nullptr, size_t count = -1, const char *valuemode = nullptr)
    {
        if (y)
        {
            if (!(strcasecmp(valuemode, "random")))
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
    }
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, count is the total element number in y
    void chacha(u32 *x, u32 *x1 = nullptr, u16 *y = nullptr, u16 *list1 = nullptr, u16 *list2 = nullptr, size_t ysize = -1, size_t list1_size = -1, size_t list2_size = -1, const char *valuemode = nullptr)
    {
        if (y)
        {
            if (!(strcasecmp(valuemode, "random")))
            {
                for (int i{0}; i < ysize; ++i)
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
            else if (!(strcasecmp(valuemode, "zero")))
            {
                for (int i{0}; i < ysize; ++i)
                {
                    WORD = (y[i] / 32) + 4;
                    BIT = y[i] % 32;

                    x[WORD] &= ~(0x1 << BIT);
                    x1[WORD] &= ~(0x1 << BIT);
                }
            }
        }
        else
        {
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
    }
};

class STATES
{
public:
    // sum of x and x1 is stored in x
    void AddStates(u32 *x, u32 *x1)
    {
        for (int i{0}; i < 16; ++i)
            x[i] += x1[i];
    }
    void CopyState(u32 *x, u32 *x1, u32 n)
    {
        for (int i{0}; i < n; ++i)
            x[i] = x1[i];
    }
    // 4 cross 4 matrix form print, by default set to matrix form printing, size set to 0, set flag to false to not print in matrix form
    // bin is set to false, to print binary set it to true
    void PrintState(u32 *x, size_t size = 16, bool matrixflag = true, bool bin = false)
    {
        if (bin)
        {
            if (matrixflag)
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
            else
            {
                for (int i{0}; i < size; ++i)
                {
                    std::bitset<32> temp(x[i]);
                    std::cout << temp << "  ";
                }
                printf("\n");
            }
        }
        else if (!matrixflag)
        {
            for (int i{0}; i < size; ++i)
                printf("%u ", x[i]);
            printf("\n");
        }
        else
        {
            for (int i{0}; i < size; ++i)
            {
                printf("%8x ", x[i]);
                if (i > 0 && i % 4 == 3)
                    printf("\n");
            }
            printf("\n");
        }
    }
    // all the values of x are set to value, by defualt it is set to 0
    void
    ResetState(u32 *x, u32 size, u32 value = -1)
    {
        if (value == -1)
        {
            for (int i{0}; i < size; ++i)
                x[i] = 0x0;
        }
        else
        {
            for (int i{0}; i < size; ++i)
                x[i] = value;
        }
    }
    void SubtractStates(u32 *x, u32 *x1)
    {
        for (int i{0}; i < 16; ++i)
            x[i] -= x1[i];
    }

    // XOR of the states x and x1 is stored in y
    void XORDifference(u32 *x, u32 *x1, u32 *y, size_t n)
    {
        for (int i{0}; i < n; ++i)
            y[i] = x[i] ^ x1[i];
    }

    // function to count the number of SET bits along a column or diagonal By default the 1st coloumn is chosen
    // by default str is set to calculate the total number of set bits in 512 positions
    int SetBitsCount(u32 *x, const char *str = nullptr, int diagonal_no = 0, int column_no = 0)
    {
        int temp = 0;

        if (!strcmp(str, "column"))
        {
            switch (column_no)
            {
            case 2:
            {
                int tempar2[4] = {5, 9, 13, 1};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar2[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            case 3:
            {
                int tempar3[4] = {10, 14, 2, 6};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar3[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            case 4:
            {
                int tempar4[4] = {15, 3, 7, 11};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar4[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            default:
            {
                int tempar[4] = {0, 4, 8, 12};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            }
            return temp;
        }

        else if (!strcmp(str, "diagonal"))
        {
            switch (diagonal_no)
            {
            case 2:
            {
                int tempar1[] = {1, 6, 11, 12};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar1[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            case 3:
            {
                int tempar2[] = {2, 7, 8, 13};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar2[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            case 4:
            {
                int tempar3[] = {3, 4, 9, 14};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar3[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            default:
            {
                int tempar[] = {0, 5, 10, 15};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (x[tempar[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
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
};

void InputDifference(u32 *x, int word, int bit)
{
    int pattern = 1 << bit;
    x[word] ^= pattern;
}

// cipher name, total round, number of forward rounds, IDword array, IDbit array, total number of IDs, ODword array, ODbit array, total number of ODs,
void Details(const char *cipher, const char *progtype = nullptr, u16 totalround = 0, u16 fwdround = 0, u16 *IDword = nullptr, u16 *IDbit = nullptr, size_t IDsize = -1, u16 *ODword = nullptr, u16 *ODbit = nullptr, size_t ODsize = -1, size_t PNBcount = 0, const char *mode = nullptr, ull loopLim = 0, ull ITERATION = 0)
{
    std::cout << std::setw(4) << "+------------------------------------------------------------------------------------+\n";
    if (progtype)
        std::cout << std::setw(4) << "|            " << progtype << "programe                                 " << std::setw(7) << "|\n";
    std::cout << std::setw(4) << "|            Cipher Name: " << cipher << ", # of total rounds: " << (unsigned)totalround << ", # of forward rounds: " << fwdround << "  " << std::setw(7) << "|\n";
    if (IDword)
    {
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
                std::cout << "(" << ODword[i] << ", " << ODbit[i] << ")                            " << std::setw(7) << "|\n";
            else
            {
                std::cout << "(" << ODword[i] << ", " << ODbit[i] << ")⊕";
            }
        }
    }
    if (PNBcount)
        std::cout << std::setw(5) << "                Number of PNBs: " << PNBcount << std::setw(7) << "|\n";
    if (mode)
        std::cout << std::setw(5) << "|            Values of PNBs are in " << mode << " mode                                  " << std::setw(7) << "|\n";

    if (loopLim)
        std::cout << std::setw(4) << "|            Per loop size : 2^{" << log2(loopLim) << "}                                             " << std::setw(7) << "|\n";
    if (ITERATION)
        std::cout << std::setw(4) << "|            Total samples : 2^{" << log2(ITERATION / loopLim) << "}                                          " << std::setw(7) << "|\n";
}

u16 *OpenFile(const char *file, size_t *PNBcount)
{
    std::ifstream pnb(file);

    if (!pnb.is_open())
    {
        std::cout << "|          ⚠ Unable to open the file ⚠                                          " << std::setw(7) << "|\n";
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
        std::cout << "|          ⚠ Unable to open the file ⚠                                         " << std::setw(7) << "|\n";
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
