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
#include <random>
#include <vector> // vector

typedef uint16_t u16;           // positive integer of 16 bits
typedef uint32_t u32;           // positive integer of 32 bits
typedef unsigned long long ull; // 32 - 64 bits memory

constexpr int WORD_SIZE = std::numeric_limits<u32>::digits;
constexpr u32 MOD = std::numeric_limits<u32>::max(); // pow(2, WORD_SIZE)

constexpr size_t WORD_COUNT = 16; // state is formed by sixteen 32-bit words
constexpr size_t KEY_COUNT = 8;   // state is formed by eight 32-bit keyWords

constexpr size_t SALSA_IV_START = 6;
constexpr size_t SALSA_IV_END = 9;

constexpr size_t CHACHA_IV_START = 12;
constexpr size_t CHACHA_IV_END = 15;

constexpr size_t CHACHA_KEY_START = 4;
constexpr size_t CHACHA_KEY_END = 11;

// #define GetBit(a, p) (((a) >> (p)) & 0x1) // p-th bit of word a

#define GetBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word >> bit) & 0x1) : false)
#define SetBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word) |= (1u << bit)) : false)
#define UnsetBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word) &= ~(1u << bit)) : false)
#define ToggleBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word) ^= (1u << bit)) : false)

typedef uint16_t u16;           // positive integer of 16 bits
typedef uint32_t u32;           // positive integer of 32 bits
typedef unsigned long long ull; // 32 - 64 bits memory

/*
// constexpr u32 MOD = ((1ULL << WORD_SIZE-1)); // pow(2, WORD_SIZE)
*/

// random number of 32 bits
// static inline u32 drandom()
// {
//     return MOD * drand48();
// }
u32 drandom()
{
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dis(1, MOD);
    return dis(gen);
}

bool generateRandomBoolean()
{
    // static std::mutex mt_mutex;                 // Static ensures that the mutex is shared among all calls to the function
    // std::lock_guard<std::mutex> lock(mt_mutex); // Lock the mutex during random number generation

    // static std::mt19937 mt_engine(std::random_device{}());
    static std::mt19937 mt_engine(std::random_device{}());
    // std::mt19937 mt_engine(time(NULL));
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    return (distribution(mt_engine) > 0.5);
}

#define ROTATE_LEFT(x, n) (((x) << (n)) ^ ((x) >> (WORD_SIZE - n)))

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
            for (size_t i{6}; i < 10; ++i)
                x[i] = drandom(); // IV
        }
        else
        {
            for (size_t i{0}; i < 16; ++i)
                default_values[i] = 0;
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
            for (size_t index{CHACHA_IV_START}; index <= CHACHA_IV_END; ++index)
                x[index] = drandom(); // IV
        }
        else
        {
            for (size_t i{0}; i < 16; ++i)
                default_values[i] = 0;
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
            for (int index{0}; index < KEY_COUNT; ++index)
                k[index] = drandom();
        }
        else
        {
            for (int i{0}; i < 8; ++i)
                default_values[i] = 0;
            k = default_values;
        }
    }
    void Key128(u32 *k = nullptr)
    {
        if (k)
        {
            for (int i{0}; i < 4; ++i)
            {
                k[i] = drandom();
                k[i + 4] = k[i];
            }
        }
        else
        {
            for (int i{0}; i < 4; ++i)
            {
                default_values[i] = 0;
                default_values[i + 4] = default_values[i];
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
        for (size_t index{CHACHA_KEY_START}; index <= CHACHA_KEY_END; ++index)
            x[index] = k[index - 4];
    }
};

// mode: normal / block value assignment, // valuemode: random 0s and 1s / 0s, // flag: random change of PNBS / fixed change of PNBs
struct PutValueParams
{
    const char *mode, *valuemode; // by default mode: normal, otherwise block , by default valuemode: random, otherwise 0-value
    u32 *x, *x1;
    u16 *y, *list1, *list2;
    size_t ysize, list1size, list2size;
    bool flag;

    // Constructor with default values
    PutValueParams(bool flag = true, u32 *x = nullptr, u16 *y = nullptr, size_t ysize = 0, const char *mode = nullptr, const char *valuemode = nullptr, u32 *x1 = nullptr, u16 *list1 = nullptr, u16 *list2 = nullptr, size_t list1size = 0, size_t list2size = 0)
        : flag(flag), mode(mode), valuemode(valuemode), x(x), x1(x1), y(y), list1(list1), list2(list2), ysize(ysize), list1size(list1size), list2size(list2size) {}
};

// mode: normal / block value assign, //valuemode: random value  / zero values , // flag: random change of PNBs / definite change of PNBs
class PUT_VALUE
{
public:
    int WORD, BIT;
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, ysize is the total element number in y
    // void salsa(PutValueParams &params)
    // {
    //     if (!(params.mode))
    //     {
    //         if (!(params.valuemode)) // random
    //         {
    //             for (int i{0}; i < params.ysize; ++i)
    //             {
    //                 if ((params.y[i] / 32) > 3)
    //                 {
    //                     WORD = (params.y[i] / 32) + 7;
    //                 }
    //                 else
    //                 {
    //                     WORD = (params.y[i] / 32) + 1;
    //                 }
    //                 BIT = params.y[i] % 32;

    //                 if (params.flag) // flag:true means randomly change the PNBs
    //                 {
    //                     if (generateRandomBoolean)
    //                     {
    //                         // params.x[WORD] ^= (0x1 << BIT);
    //                         // if (params.x1)
    //                         //     params.x1[WORD] ^= (0x1 << BIT);
    //                         ToggleBit(params.x[WORD], BIT);
    //                         if (params.x1)
    //                             ToggleBit(params.x1[WORD], BIT);
    //                     }
    //                 }
    //                 else
    //                 {
    //                     // params.x[WORD] ^= (0x1 << BIT);
    //                     // if (params.x1)
    //                     //     params.x1[WORD] ^= (0x1 << BIT);
    //                     ToggleBit(params.x[WORD], BIT);
    //                     if (params.x1)
    //                         ToggleBit(params.x1[WORD], BIT);
    //                 }
    //             }
    //         }
    //         else // 0 values
    //         {
    //             for (int i{0}; i < params.ysize; ++i)
    //             {
    //                 if ((params.y[i] / 32) > 3)
    //                 {
    //                     WORD = (params.y[i] / 32) + 7;
    //                 }
    //                 else
    //                 {
    //                     WORD = (params.y[i] / 32) + 1;
    //                 }
    //                 BIT = params.y[i] % 32;

    //                 if (generateRandomBoolean)
    //                 {
    //                     params.x[WORD] &= ~(0x1 << BIT);
    //                     if (params.x1)
    //                         params.x1[WORD] &= ~(0x1 << BIT);
    //                 }
    //             }
    //         }
    //     }
    // }
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, ysize is the total element number in y
    void chacha(PutValueParams &params)
    {
        if (!(params.mode))
        {
            if (!(params.valuemode)) // random values
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    WORD = (params.y[i] / WORD_SIZE) + 4;
                    BIT = params.y[i] % WORD_SIZE;
                    if (params.flag) // flag:true means randomly change the PNBs
                    {
                        if (generateRandomBoolean())
                        {
                            // params.x[WORD] ^= (0x1 << BIT);
                            // if (params.x1)
                            //     params.x1[WORD] ^= (0x1 << BIT);
                            ToggleBit(params.x[WORD], BIT);
                            if (params.x1)
                                ToggleBit(params.x1[WORD], BIT);
                        }
                    }
                    else
                    {
                        // params.x[WORD] ^= (0x1 << BIT);
                        // if (params.x1)
                        //     params.x1[WORD] ^= (0x1 << BIT);
                        ToggleBit(params.x[WORD], BIT);
                        if (params.x1)
                            ToggleBit(params.x1[WORD], BIT);
                    }
                }
            }
            else // 0 values
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    WORD = (params.y[i] / 32) + 4;
                    BIT = params.y[i] % 32;

                    params.x[WORD] &= ~(0x1 << BIT);
                    if (params.x1)
                        params.x1[WORD] &= ~(0x1 << BIT);
                }
            }
        }
        else
        {
            for (int i{0}; i < params.list1size; ++i)
            {
                WORD = (params.list1[i] / 32) + 4;
                BIT = params.list1[i] % 32;

                if ((i % 5) == 4)
                {
                    params.x[WORD] |= (0x1 << BIT);
                    if (params.x1)
                        params.x1[WORD] |= (0x1 << BIT);
                }
                else
                {
                    params.x[WORD] &= ~(0x1 << BIT);
                    if (params.x1)
                        params.x1[WORD] &= ~(0x1 << BIT);
                }
            }
            for (int i{0}; i < params.list2size; ++i)
            {
                WORD = (params.list2[i] / 32) + 4;
                BIT = params.list2[i] % 32;

                if (generateRandomBoolean)
                {
                    params.x[WORD] ^= (0x1 << BIT);
                    if (params.x1)
                        params.x1[WORD] ^= (0x1 << BIT);
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
        for (int i{0}; i < WORD_COUNT; ++i)
            x[i] += x1[i];
    }
    void CopyState(u32 *x, u32 *x1, u32 n)
    {
        for (int i{0}; i < n; ++i)
            x[i] = x1[i];
    }
    void InputDifference(u32 *x, int word, int bit)
    {
        // int pattern = 1 << bit;
        // x[word] ^= pattern;
        ToggleBit(x[word], bit);
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
            fwdBit ^= GetBit(x[ODword[index]], ODbit[index]);
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
    ull totalLoopCount, samplesPerloop;
    double nm;

    // Constructor with default values
    DetailsParams(const char *cipher = nullptr, const char *progtype = nullptr,
                  u16 totalround = 0, u16 *IDword = nullptr, u16 *IDbit = nullptr, u16 *ODword = nullptr, u16 *ODbit = nullptr, size_t IDsize = 0, size_t ODsize = 0, u16 fwdround = 0,
                  const char *PNBmode = nullptr, const char *PNBvaluemode = nullptr,
                  ull totalSamples = 0, ull samplesPerloop = 0, bool halfroundflag = false, double nm = 0, size_t PNBcount = 0) : cipher(cipher), progtype(progtype), totalround(totalround), IDword(IDword), IDbit(IDbit), ODword(ODword), ODbit(ODbit), IDsize(IDsize), ODsize(ODsize), fwdround(fwdround), halfroundflag(halfroundflag), PNBmode(PNBmode), PNBvaluemode(PNBvaluemode), totalLoopCount(totalSamples), samplesPerloop(samplesPerloop), nm(nm), PNBcount(PNBcount) {}
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

    if (params.totalLoopCount)
        output << std::setw(2) << "            Total samples : 2^{" << log2(params.totalLoopCount) << "}                                             " << std::setw(7) << "\n";
    if (params.samplesPerloop)
        output << std::setw(4) << "            Samples per loop : 2^{" << log2(params.samplesPerloop) << "}                                          " << std::setw(7) << "\n";
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
