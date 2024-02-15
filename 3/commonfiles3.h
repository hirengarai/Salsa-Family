/*
 * REFERENCE IMPLEMENTATION OF some common functions that is used in both Salsa and ChaCha cipher
 *
 * Filename: commonfiles.h
 *
 * created: 23/09/23
 * updated: 05/01/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This file contains some common functions that is used both in Salsa and ChaCha scheme.
 */
#include <algorithm> // sorting purpose
#include <bitset>    // binary representation
#include <cmath>     // pow function
#include <cstring>   // string
#include <fstream>   // files
#include <iostream>  // cin cout
#include <iomanip>   // decimal numbers upto certain places, setw function
#include <vector>    // vector

#define GET_BIT(a, p) (((a) >> (p)) & 0x1) // p-th bit of word a

typedef uint16_t u16;           // positive integer of 16 bits
typedef uint32_t u32;           // positive integer of 32 bits
typedef unsigned long long ull; // 32 - 64 bits memory

constexpr int WORD_SIZE = std::numeric_limits<u32>::digits;
constexpr u32 MOD = 1ULL << WORD_SIZE; // pow(2, WORD_SIZE)

// random number of 32 bits
static inline u32 drandom()
{
    return MOD * drand48();
}

#define ROTATE_LEFT(x, n) (((x) << (n)) ^ ((x) >> (WORD_SIZE - (n))))

// IV inisialisation
class INITIALISE_IV
{
public:
    void salsa(u32 *x = nullptr)
    {
        if (x)
        {
            x[0] = 0x61707865;
            x[5] = 0x3120646e;
            x[10] = 0x79622d36;
            x[15] = 0x6b206574;
            for (size_t index{6}; index < 10; ++index)
                x[index] = drandom(); // IV
        }
        else
        {
            for (size_t index{0}; index < 16; ++index)
                default_values[index] = 0;
            x = default_values;
        }
    }

    void chacha(u32 *x = nullptr)
    {
        if (x)
        {
            x[0] = 0x61707865;
            x[1] = 0x3320646e;
            x[2] = 0x79622d32;
            x[3] = 0x6b206574;
            for (size_t index{12}; index < 16; ++index)
                x[index] = drandom(); // IV
        }
        else
        {
            for (size_t index{0}; index < 16; ++index)
                default_values[index] = 0;
            x = default_values;
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
        if (k)
        {
            for (int index{0}; index < 8; ++index)
                k[index] = drandom();
        }
        else
        {
            for (int index{0}; index < 8; ++index)
                default_values[index] = 0;
            k = default_values;
        }
    }
    void Key128(u32 *k = nullptr)
    {
        if (k)
        {
            for (int index{0}; index < 4; ++index)
            {
                k[index] = drandom();
                k[index + 4] = k[index];
            }
        }
        else
        {
            for (int index{0}; index < 4; ++index)
            {
                default_values[index] = 0;
                default_values[index + 4] = default_values[index];
            }
            k = default_values;
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

// mode: normal / block value assignment, // valuemode: random 0s and 1s / 0s, // flag: random change of PNBS / fixed change of PNBs
struct PutValueParams
{
    // by default mode: normal, otherwise block , by default valuemode: random, otherwise 0-value
    u32 *x, *dx;
    u16 *pnb, *pnbinblocks, *restpnbs;
    size_t pnbsize, pnbinblockssize, restpnbssize;
    bool pnbblockflag, pnbvaluechange_randomflag, fixed_pnbvaluechangeflag;

    // Constructor with default values
    PutValueParams(bool pnbrandomvaluechangeflag = true, bool pnbvaluerandomflag = true, bool pnbblockflag = false, u32 *x = nullptr, u32 *dx = nullptr, u16 *pnb = nullptr, size_t pnbsize = 0, u16 *pnbinblocks = nullptr, u16 *restpnbs = nullptr, size_t pnbinblockssize = 0, size_t restpnbssize = 0)
        : fixed_pnbvaluechangeflag(pnbrandomvaluechangeflag), pnbvaluechange_randomflag(pnbvaluerandomflag), pnbblockflag(pnbblockflag), x(x), dx(dx), pnb(pnb), pnbinblocks(pnbinblocks), restpnbs(restpnbs), pnbsize(pnbsize), pnbinblockssize(pnbinblockssize), restpnbssize(restpnbssize) {}
};

// mode: normal / block value assign, //valuemode: random / zero, // flag: random change / random change
class PUT_VALUE
{
public:
    int WORD, BIT;
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, ysize is the total element number in y
    void salsa(PutValueParams &params)
    {
        if (!(params.pnbblockflag))
        {
            if (!(params.valuemode)) // random
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    if ((params.y[i] / WORD_SIZE) > 3)
                        WORD = (params.y[i] / WORD_SIZE) + 7;
                    else
                        WORD = (params.y[i] / WORD_SIZE) + 1;
                    BIT = params.y[i] % WORD_SIZE;

                    if (params.flag) // flag:true means randomly change the PNBs
                    {
                        if (drand48() > 0.5)
                        {
                            params.x[WORD] ^= (0x1 << BIT);
                            if (params.x1)
                                params.x1[WORD] ^= (0x1 << BIT);
                        }
                    }
                    else
                    {
                        params.x[WORD] ^= (0x1 << BIT);
                        if (params.x1)
                            params.x1[WORD] ^= (0x1 << BIT);
                    }
                }
            }
            else // 0 values
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    if ((params.y[i] / WORD_SIZE) > 3)
                    {
                        WORD = (params.y[i] / WORD_SIZE) + 7;
                    }
                    else
                    {
                        WORD = (params.y[i] / WORD_SIZE) + 1;
                    }
                    BIT = params.y[i] % WORD_SIZE;

                    if (drand48() > 0.5)
                    {
                        params.x[WORD] &= ~(0x1 << BIT);
                        if (params.x1)
                            params.x1[WORD] &= ~(0x1 << BIT);
                    }
                }
            }
        }
    }
    void chacha(PutValueParams &params)
    {
        if (params.pnbblockflag)
        {
            for (size_t index{0}; index < params.pnbinblockssize; ++index)
            {
                WORD = (params.pnbinblocks[index] / WORD_SIZE) + 4;
                BIT = params.pnbinblocks[index] % WORD_SIZE;

                if ((index % 5) == 4)
                {
                    params.x[WORD] |= (0x1 << BIT);
                    if (params.dx)
                        params.dx[WORD] |= (0x1 << BIT);
                }
                else
                {
                    params.x[WORD] &= ~(0x1 << BIT);
                    if (params.dx)
                        params.dx[WORD] &= ~(0x1 << BIT);
                }
            }
            for (size_t i{0}; i < params.restpnbssize; ++i)
            {
                WORD = (params.restpnbs[i] / WORD_SIZE) + 4;
                BIT = params.restpnbs[i] % WORD_SIZE;

                if (drand48() > 0.5)
                {
                    params.x[WORD] ^= (0x1 << BIT);
                    if (params.dx)
                        params.dx[WORD] ^= (0x1 << BIT);
                }
            }
        }
        else
        {
            if (params.pnbvaluechange_randomflag) // random values
            {
                for (size_t index{0}; index < params.pnbsize; ++index)
                {
                    WORD = (params.pnb[index] / WORD_SIZE) + 4;
                    BIT = params.pnb[index] % WORD_SIZE;

                    if (params.fixed_pnbvaluechangeflag && (drand48() > 0.5))
                    {
                        params.x[WORD] ^= (0x1 << BIT);
                        if (params.dx)
                            params.dx[WORD] ^= (0x1 << BIT);
                    }
                    else if (!params.fixed_pnbvaluechangeflag)
                    {
                        params.x[WORD] ^= (0x1 << BIT);
                        if (params.dx)
                            params.dx[WORD] ^= (0x1 << BIT);
                    }
                }
            }
            else // 0 values
            {
                for (int i{0}; i < params.pnbsize; ++i)
                {
                    WORD = (params.pnb[i] / WORD_SIZE) + 4;
                    BIT = params.pnb[i] % WORD_SIZE;

                    params.x[WORD] &= ~(0x1 << BIT);
                    if (params.dx)
                        params.dx[WORD] &= ~(0x1 << BIT);
                }
            }
        }
    }
};

// x = state matrix, size = 16, matrixform, binary form
struct PrintStateParams
{
    u32 *x;
    size_t size;
    bool matrixform, binaryform;

    // Constructor with default values
    PrintStateParams(u32 *x = nullptr, size_t size = 16, bool matrixform = true, bool binaryform = false)
        : x(x), size(size), matrixform(matrixform), binaryform(binaryform) {}
};

// str = column/diagonal
struct SetBitsCountParams
{
    u32 *x;
    const char *str;
    int columnno, diagno;
    int word;

    // int x;
    // const char *str;
    // int y;
    // int z;

    // Constructor with default values
    SetBitsCountParams(u32 *x = nullptr, const char *str = nullptr, int columnno = 0, int diagno = 0, int word = 0)
        : x(x), str(str), columnno(columnno), diagno(diagno), word(word) {}
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
    void InputDifference(u32 *x, int word, int bit)
    {
        int pattern = 1 << bit;
        x[word] ^= pattern;
    }
    // 4 cross 4 matrix form print, by default set to matrix form printing, size set to 0, set flag to false to not print in matrix form
    // bin is set to false, to print binary set it to true
    void PrintState(const PrintStateParams &params)
    {
        if (params.binaryform)
        {
            if (params.matrixform)
            {
                for (int i{0}; i < params.size; ++i)
                {
                    std::bitset<32> temp(params.x[i]);
                    std::cout << temp << "  ";
                    if (i > 0 && i % 4 == 3)
                        printf("\n");
                }
                printf("\n");
            }
            else
            {
                for (int i{0}; i < params.size; ++i)
                {
                    std::bitset<32> temp(params.x[i]);
                    std::cout << temp << "  ";
                }
                printf("\n");
            }
        }
        else if (params.matrixform)
        {
            for (int i{0}; i < params.size; ++i)
            {
                printf("%8x ", params.x[i]);
                if (i > 0 && i % 4 == 3)
                    printf("\n");
            }
            printf("\n");
        }
        else
        {
            for (int i{0}; i < params.size; ++i)
                printf("%u ", params.x[i]);
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
    void XORDifference(u32 *y, size_t n, u32 *x, u32 *x1 = nullptr)
    {
        if (x1)
        {
            for (int i{0}; i < n; ++i)
                y[i] = x[i] ^ x1[i];
        }
        else
        {
            for (int i{0}; i < n; ++i)
                y[i] = x[i];
        }
    }

    u16 DifferenceBit(u32 *x, u16 *ODword, u16 *ODbit, size_t size)
    {
        u16 fwdBit{0};
        for (size_t index{0}; index < size; ++index)
            fwdBit ^= GET_BIT(x[ODword[index]], ODbit[index]);
        return fwdBit;
    }

    // function to count the number of SET bits along a column or diagonal By default the 1st coloumn is chosen
    // by default str is set to calculate the total number of set bits in 512 positions, // word means matrix word
    int SetBitsCount(const SetBitsCountParams &params)
    {
        int temp = 0;

        if (!params.str) // column
        {
            switch (params.columnno)
            {
            case 2:
            {
                int tempar2[4] = {5, 9, 13, 1};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (params.x[tempar2[i]] & pattern)
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
                        if (params.x[tempar3[i]] & pattern)
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
                        if (params.x[tempar4[i]] & pattern)
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
                        if (params.x[tempar[i]] & pattern)
                            temp++;
                    }
                }
            }
            break;
            }
            return temp;
        }

        else if (!strcmp(params.str, "diagonal"))
        {
            switch (params.diagno)
            {
            case 2:
            {
                int tempar1[] = {1, 6, 11, 12};
                for (int i{0}; i < 4; ++i)
                {
                    for (int j{0}; j < 32; ++j)
                    {
                        int pattern = 0x1 << j;
                        if (params.x[tempar1[i]] & pattern)
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
                        if (params.x[tempar2[i]] & pattern)
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
                        if (params.x[tempar3[i]] & pattern)
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
                        if (params.x[tempar[i]] & pattern)
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
            for (int j{0}; j < 32; ++j)
            {
                int pattern = 0x1 << j;
                if (params.x[params.word] & pattern)
                    temp++;
            }
            return temp;
        }
    }
};

struct DetailsParams
{
    const char *cipher, *progtype;
    u16 totalround, *IDword, *IDbit, *ODword, *ODbit, fwdround;
    size_t IDsize, ODsize;
    bool halfroundflag;
    const char *PNBmode, *PNBvaluemode;
    size_t PNBcount;
    ull totalLoopCount, samplesPerloop, samplesPerkey, totalSampleKey;
    double nm;

    // Constructor with default values
    DetailsParams(const char *cipher = nullptr, const char *progtype = nullptr,
                  u16 totalround = 0, u16 *IDword = nullptr, u16 *IDbit = nullptr, u16 *ODword = nullptr, u16 *ODbit = nullptr, size_t IDsize = 0, size_t ODsize = 0, u16 fwdround = 0,
                  const char *PNBmode = nullptr, const char *PNBvaluemode = nullptr,
                  ull totalSamples = 0, ull samplesPerkey = 0, ull samplesPerloop = 0, ull totalSampleKey = 0, bool halfroundflag = false, double nm = 0, size_t PNBcount = 0) : cipher(cipher), progtype(progtype), totalround(totalround), IDword(IDword), IDbit(IDbit), ODword(ODword), ODbit(ODbit), IDsize(IDsize), ODsize(ODsize), fwdround(fwdround), halfroundflag(halfroundflag), PNBmode(PNBmode), PNBvaluemode(PNBvaluemode), totalLoopCount(totalSamples), samplesPerkey(samplesPerkey), samplesPerloop(samplesPerloop), totalSampleKey(totalSampleKey), nm(nm), PNBcount(PNBcount) {}
};

// cipher name, total round, number of forward rounds, IDword array, IDbit array, total number of IDs, ODword array, ODbit array, total number of ODs,
void Details(const DetailsParams &params, std::ostream &output)
{
    output << std::setw(4) << "+------------------------------------------------------------------------------------+\n";
    if (params.cipher)
        output << std::setw(4) << "            Cipher Name: " << params.cipher << "\n";

    if (params.progtype)
        output << std::setw(4) << "            " << params.progtype << "                                     " << std::setw(7) << "\n";

    if (params.halfroundflag)
        output << " # of total rounds: " << (unsigned)params.totalround + 0.5 << ", # of forward rounds: " << params.fwdround << "  " << std::setw(7) << "\n";
    else
    {
        output << "            # of total rounds: " << (unsigned)params.totalround << "\n";
    }

    if (params.IDword)
    {
        output << "            # of forward rounds: " << params.fwdround << "  " << std::setw(7) << "\n";
        output << std::setw(4) << "            ID = ";

        for (int i{0}; i < params.IDsize; ++i)
        {
            if (i == params.IDsize - 1)
                output << "(" << params.IDword[i] << ", " << params.IDbit[i] << "), ";
            else
            {
                output << "(" << params.IDword[i] << ", " << params.IDbit[i] << ")⊕";
            }
        }
        output << "OD = ";
        for (int i{0}; i < params.ODsize; ++i)
        {
            if (i == params.ODsize - 1)
                output << "(" << params.ODword[i] << ", " << params.ODbit[i] << ")                            " << std::setw(7) << "\n";
            else
            {
                output << "(" << params.ODword[i] << ", " << params.ODbit[i] << ")⊕";
            }
        }
    }
    if (params.nm)
        output << std::setw(2) << "            Neutrality Measure: " << params.nm << std::setw(7) << "\n";
    if (params.PNBcount)
        output << std::setw(5) << "            Number of PNBs: " << params.PNBcount << std::setw(7) << "\n";
    if (params.PNBmode)
        output << std::setw(5) << "            Values of PNBs are in " << params.PNBmode << " mode                                  " << std::setw(7) << "\n";
    if (params.totalSampleKey)
        output << std::setw(2) << "            Total key samples : 2^{" << log2(params.totalSampleKey) << "}\n";

    if (params.samplesPerloop)
        output << std::setw(4) << "            Samples per loop : 2^{" << log2(params.samplesPerloop) << "}                                          " << std::setw(7) << "\n";
    if (params.samplesPerkey)
        output << std::setw(4) << "            Samples per key : 2^{" << log2(params.samplesPerkey) << "}                                          " << std::setw(7) << "\n";
    if (params.totalLoopCount)
        output
            << std::setw(2) << "            Total samples : 2^{" << log2(params.totalLoopCount) << "}                                             " << std::setw(7) << "\n";
    output << std::setw(4) << "+------------------------------------------------------------------------------------+\n";
}

void BiasSrotedPrint(std::vector<double> &PNB_BIAS, std::vector<u16> &PNB, std::ostream &output)
{
    int flag;
    size_t size = PNB.size();
    for (size_t l{size}; l >= 0; --l)
    {
        flag = 0;
        for (int j = 0; j < l; ++j)
        {
            if (PNB_BIAS[j] < PNB_BIAS[j + 1])
            {
                std::swap(PNB_BIAS.at(j), PNB_BIAS.at(j + 1));
                std::swap(PNB.at(j), PNB.at(j + 1));
                flag = 1;
            }
        }
        if (flag == 0)
            break;
    }
    output << "\nThe PNBs in descending order of BIAS is as follows:\n{";
    bool firstElement = true;
    for (auto const &i : PNB)
    {
        if (!firstElement)
            output << ", ";
        output << i;
        firstElement = false;
    }
    output << "}\n";
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
