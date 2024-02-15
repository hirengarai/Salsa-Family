#include <algorithm> // sorting purpose
#include <bitset>    // binary representation
#include <cmath>     // pow function
#include <cstring>   // string
#include <fstream>   // files
#include <iostream>  // cin cout
#include <iomanip>   // decimal numbers upto certain places, setw function
#include <random>    // decimal numbers upto certain places, setw function
#include <vector>    // vector

typedef uint16_t u16;           // positive integer of 16 bits
typedef uint32_t u32;           // positive integer of 32 bits
typedef unsigned long long ull; // 32 - 64 bits worth memory

constexpr int WORD_SIZE = std::numeric_limits<u32>::digits;
constexpr u32 MOD = 1ULL << WORD_SIZE; // pow(2, WORD_SIZE)

const size_t WORD_COUNT = 16; // state is formed by sixteen 32-bit words

const size_t SALSA_IV_START = 6;
const size_t SALSA_IV_END = 9;

const size_t CHACHA_IV_START = 12;
const size_t CHACHA_IV_END = 15;

const size_t CHACHA_KEY_START = 4;
const size_t CHACHA_KEY_END = 11;

const size_t BLOCK_LEN = 5; // if PNBs are in block form, then the block length

// random number of 32 bits
static inline u32 drandom1()
{
    return MOD * drand48();
}

static inline double drandom2()
{
    static std::default_random_engine generator(std::random_device{}());
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    double probability = distribution(generator);
    return probability;
}

inline bool GetBit(u32 word, u16 bit)
{
    return (bit >= 0 && bit < sizeof(word) * 8) ? ((word >> bit) & 0x1) : false;
}

inline void UnsetBit(u32 &word, u16 bit)
{
    if (bit >= 0 && bit < sizeof(word) * 8)
        word &= ~(0x1u << bit);
}

inline void SetBit(u32 &word, u16 bit)
{
    if (bit >= 0 && bit < sizeof(word) * 8)
        word |= (0x1u << bit);
}

inline void ToggleBit(u32 &word, u16 bit)
{
    if (bit >= 0 && bit < sizeof(word) * 8)
        word ^= (0x1u << bit);
}

enum BitOperation
{
    TOGGLE,
    UNSET,
    SET
};

void ModifyBits(u32 &word, u16 &bit, BitOperation operation, bool arbitrarychangeflag = true)
{
    if (arbitrarychangeflag)
    {
        switch (operation)
        {
        case TOGGLE:
            if (drandom2() < 0.5)
                ToggleBit(word, bit);
            break;
        case UNSET:
            if (drandom2() > 0.5)
                UnsetBit(word, bit);
            break;
        case SET:
            if (drandom2() < 0.5)
                SetBit(word, bit);
            break;
        default:
            // Handle error or default case
            break;
        }
    }
    else
    {
        switch (operation)
        {
        case TOGGLE:
            ToggleBit(word, bit);
            break;
        case UNSET:
            UnsetBit(word, bit);
            break;
        case SET:
            SetBit(word, bit);
            break;
        default:
            // Handle error or default case
            break;
        }
    }
}

// Example usage:
// ModifyBits(myWord, myBit, TOGGLE);
// ModifyBits(myWord, myBit, UNSET, true);
// ModifyBits(myWord, myBit, SET, false);

// void RandomizeBits(u32 &word, u16 &bit, bool fixedChangeFlag = false)
// {
//     static std::default_random_engine generator(std::random_device{}());
//     static std::uniform_real_distribution<double> distribution(0.0, 1.0);

//     if (fixedChangeFlag || (distribution(generator) < 0.5))
//     {
//         ToggleBit(word, bit);
//     }
// }

void CalculateWORDandBIT(int index, int &WORD, int &BIT)
{
    WORD = (index / WORD_SIZE) + 4;
    BIT = index % WORD_SIZE;
}

// void Randomizebits(int &word, int &bit, bool fixedchangeflag = false)
// {
//     if (fixedchangeflag)
//         TOGGLE_BIT(word, bit);
//     else
//     {
//         if (drand48() < 0.5)
//             TOGGLE_BIT(word, bit);
//     }
// }

// void Unsetbits(int &word, int &bit, bool fixedchangeflag = false)
// {
//     if (fixedchangeflag)
//         UNSET_BIT(word, bit);
//     else
//     {
//         if (drand48() > 0.5)
//             UNSET_BIT(word, bit);
//     }
// }

// void Setbits(int &word, int &bit, bool fixedchangeflag = false)
// {
//     if (fixedchangeflag)
//         SET_BIT(word, bit);
//     else
//     {
//         if (drand48() > 0.5)
//             SET_BIT(word, bit);
//     }
// }

#define ROTATE_LEFT(x, n) (((x) << (n)) ^ ((x) >> (WORD_SIZE - (n))))

// x, x1: States | pnb: listmode/ pnbinblock && restpnbsize | pnbblockflag: false, pnbvaluerandomflag: true, pnbvaluechangerandomlyflag: true
struct PutValueParams
{
    u32 *x, *x1;
    u16 *pnb, *pnbinblock, *restpnb;
    size_t pnbsize, pnbinblocksize, restpnbsize;
    bool pnbblockflag, pnbvaluechange_randomflag, fixed_pnbvaluechangeflag;

    // Constructor with default values
    PutValueParams(bool pnbblockflag = false, bool pnbvaluerandomflag = true, bool pnbvaluechangerandomlyflag = true, u32 *x = nullptr, u32 *dx = nullptr, u16 *pnb = nullptr, size_t pnbsize = 0, u16 *pnbinblock = nullptr, u16 *restpnb = nullptr, size_t pnbinblocksize = 0, size_t restpnbssize = 0)
        : fixed_pnbvaluechangeflag(pnbvaluechangerandomlyflag), pnbvaluechange_randomflag(pnbvaluerandomflag), pnbblockflag(pnbblockflag), x(x), x1(x1), pnb(pnb), pnbinblock(pnbinblock), restpnb(restpnb), pnbsize(pnbsize), pnbinblocksize(pnbinblocksize), restpnbsize(restpnbsize) {}
};

class SALSA
{
public:
    void inisialisation(u32 *x = nullptr)
    {
        if (x)
        {
            x[0] = 0x61707865;
            x[5] = 0x3120646e;
            x[10] = 0x79622d36;
            x[15] = 0x6b206574;
            for (size_t index{SALSA_IV_START}; index <= SALSA_IV_END; ++index)
                x[index] = drandom2(); // IV
        }
        else
        {
            for (size_t index{0}; index < WORD_COUNT; ++index)
                default_values[index] = 0;
            x = default_values;
        }
    }
    // fitting the key k into the state matrix x
    void keyinsertion(u32 *x, u32 *k)
    {
        for (size_t index{1}; index < 5; ++index)
            x[index] = k[index - 1];
        for (size_t index{11}; index < 15; ++index)
            x[index] = k[index - 7];
    }

    void calculateWORDandBIT(int index, int &WORD, int &BIT)
    {

        WORD = (index / WORD_SIZE) + (((index / WORD_SIZE) > 3) ? 7 : 1);
        BIT = index % WORD_SIZE;
        // if ((index / WORD_SIZE) > 3)
        //     WORD = (index / WORD_SIZE) + 7;
        // else
        //     WORD = (index / WORD_SIZE) + 1;
    }
    // void pnbvalue(PutValueParams &params)
    // {
    //     int WORD, BIT;
    //     if (!(params.pnbblockflag))
    //     {
    //         if (!(params.valuemode)) // random
    //         {
    //             for (int index{0}; index < params.pnbsize; ++index)
    //             {
    //                 calculateWORDandBIT(params.pnb[index], WORD, BIT);

    //                 if (params.flag) // flag:true means randomly change the PNBs
    //                 {
    //                     if (drand48() > 0.5)
    //                     {
    //                         params.x[WORD] ^= (0x1 << BIT);
    //                         if (params.x1)
    //                             params.x1[WORD] ^= (0x1 << BIT);
    //                     }
    //                 }
    //                 else
    //                 {
    //                     params.x[WORD] ^= (0x1 << BIT);
    //                     if (params.x1)
    //                         params.x1[WORD] ^= (0x1 << BIT);
    //                 }
    //             }
    //         }
    //         else // 0 values
    //         {
    //             for (int index{0}; index < params.pnbsize; ++index)
    //             {
    //                 calculateWORDandBIT(params.pnb[index], WORD, BIT);

    //                 if (drand48() > 0.5)
    //                 {
    //                     params.x[WORD] &= ~(0x1 << BIT);
    //                     if (params.x1)
    //                         params.x1[WORD] &= ~(0x1 << BIT);
    //                 }
    //             }
    //         }
    //     }
    // }

private:
    u32 default_values[16];
};

class CHACHA
{
    void inisialisation(u32 *x = nullptr)
    {
        if (x)
        {
            x[0] = 0x61707865;
            x[1] = 0x3320646e;
            x[2] = 0x79622d32;
            x[3] = 0x6b206574;
            for (size_t index{CHACHA_IV_START}; index <= CHACHA_IV_END; ++index)
                x[index] = drandom2(); // IV
        }
        else
        {
            for (size_t index{0}; index < WORD_COUNT; ++index)
                default_values[index] = 0;
            x = default_values;
        }
    }

    // fitting the key k into the state matrix x
    void keyinsertion(u32 *x, u32 *k)
    {
        for (size_t index{CHACHA_KEY_START}; index <= CHACHA_KEY_END; ++index)
            x[index] = k[index - 4];
    }

    void calculateWORDandBIT(int index, u32 &WORD, u16 &BIT)
    {
        WORD = (index / WORD_SIZE) + 4;
        BIT = index % WORD_SIZE;
    }
    void pnbvalue(PutValueParams &params)
    {
        u32 WORD;
        u16 BIT;
        if (params.pnbblockflag)
        {
            for (size_t index{0}; index < params.pnbinblocksize; ++index)
            {
                calculateWORDandBIT(params.pnb[index], WORD, BIT);

                if ((index % BLOCK_LEN) == BLOCK_LEN - 1) // last element of the block is made 1
                {
                    ModifyBits(params.x[WORD], BIT, SET);
                    if (params.x1)
                        ModifyBits(params.x1[WORD], BIT, SET);
                }
                else
                {
                    ModifyBits(params.x[WORD], BIT, UNSET);
                    if (params.x1)
                        ModifyBits(params.x1[WORD], BIT, UNSET);
                }
            }
            for (size_t index{0}; index < params.restpnbsize; ++index)
            {
                calculateWORDandBIT(params.pnb[index], WORD, BIT);

                ModifyBits(params.x[WORD], BIT, TOGGLE);
                if (params.x1)
                    ModifyBits(params.x1[WORD], BIT, TOGGLE);
            }
        }
        else
        {
            if (params.pnbvaluechange_randomflag) // random values
            {
                for (size_t index{0}; index < params.pnbsize; ++index)
                {
                    calculateWORDandBIT(params.pnb[index], WORD, BIT);

                    if (params.fixed_pnbvaluechangeflag)
                    {
                        ModifyBits(params.x[WORD], BIT, TOGGLE);
                        if (params.x1)
                            ModifyBits(params.x1[WORD], BIT, TOGGLE);
                    }

                    else
                    {
                        ModifyBits(params.x[WORD], BIT, TOGGLE, false);
                        if (params.x1)
                            ModifyBits(params.x1[WORD], BIT, TOGGLE, false);
                    }
                }
            }
            else // 0 values
            {
                for (size_t index{0}; index < params.pnbsize; ++index)
                {
                    calculateWORDandBIT(params.pnb[index], WORD, BIT);

                    ModifyBits(params.x[WORD], BIT, UNSET);
                    if (params.x1)
                        ModifyBits(params.x1[WORD], BIT, UNSET);
                }
            }
        }
    }

private:
    u32 default_values[16];
};

class INITIALISE_KEY
{
public:
    void key256(std::array<u32, 8> &k)
    {
        for (int i = 0; i < 8; ++i)
            k[i] = drandom2();
    }

    void key256()
    {
        key256(default_values);
    }

    void key128(std::array<u32, 8> &k)
    {
        for (int i = 0; i < 4; ++i)
        {
            k[i] = drandom2();
            k[i + 4] = k[i];
        }
    }

    void key128()
    {
        key128(default_values);
    }

private:
    std::array<u32, 8> default_values{0, 0, 0, 0, 0, 0, 0, 0};

    u32 drandom()
    {
        // Your implementation of drandom()
        return std::rand();
    }
};

class HELPER
{
public:
    // sum of x and x1 is stored in z
    void addStates(u32 *x, u32 *x1, u32 *z)
    {
        for (size_t index{0}; index < WORD_COUNT; ++index)
            z[index] = x[index] + x1[index];
    }
    // x1 is copied in x
    void copyState(u32 *x, u32 *x1, size_t xsize)
    {
        for (size_t index{0}; index < xsize; ++index)
            x[index] = x1[index];
    }
    // returns the bit at the OD position
    u16 differenceBit(u32 *x, u16 *ODword, u16 *ODbit, size_t size)
    {
        u16 fwdBit{0};
        for (size_t index{0}; index < size; ++index)
            fwdBit ^= GetBit(x[ODword[index]], ODbit[index]);
        return fwdBit;
    }
    inline void inputDifference(u32 *x, u16 word, u16 bit)
    {
        ModifyBits(x[word], bit, TOGGLE);
    }
    // all the values of x are set to value, by defualt it is set to 0
    void resetState(u32 *x, size_t size, int value = -1)
    {
        if (value == -1)
        {
            for (size_t index{0}; index < size; ++index)
                x[index] = 0;
        }
        else
        {
            for (size_t index{0}; index < size; ++index)
                x[index] = value;
        }
    }
    // diff. of x and x1 is stored in z
    void subtractStates(u32 *x, u32 *x1, u32 *z)
    {
        for (size_t index{0}; index < WORD_COUNT; ++index)
            z[index] = x[index] - x1[index];
    }
    // word wise XOR of the states x and x1, which is stored in z
    void xorDifference(u32 *z, size_t xsize, u32 *x, u32 *x1 = nullptr)
    {
        if (x1)
        {
            for (int index{0}; index < xsize; ++index)
                z[index] = x[index] ^ x1[index];
        }
        else
        {
            for (int index{0}; index < xsize; ++index)
                z[index] = x[index];
        }
    }
    // returns the total number of set bits in the array of corr. size
    int setBitsCount(u32 *x, size_t xsize)
    {
        int temp{0};
        for (size_t index{0}; index < xsize; ++index)
            temp += __builtin_popcount(x[index]);
        return temp;
    }
} Helper;

struct SetBitsCounterParams
{
    u32 *x;
    bool columnflag;
    int columnno, diagno;
    int word;

    // Constructor with default values
    SetBitsCounterParams(u32 *x = nullptr, bool columnflag = true, int columnno = 0, int diagno = 0, int word = 0)
        : x(x), columnflag(columnflag), columnno(columnno), diagno(diagno), word(word) {}
};

class SET_BITS_COUNTER
{
public:
    int SetBitsCount(const SetBitsCounterParams &params)
    {
        int counter;
        if (params.columnflag) // column
        {
            u32 col[]{0};
            switch (params.columnno)
            {
            case 2:
                col[0] = {5};
                col[1] = {9};
                col[2] = {13};
                col[3] = {1};
                break;
            case 3:
                col[0] = {10};
                col[1] = {14};
                col[2] = {2};
                col[3] = {6};
                break;
            case 4:
                col[0] = {15};
                col[1] = {3};
                col[2] = {7};
                col[3] = {11};
                break;
            default:
                col[0] = {0};
                col[1] = {4};
                col[2] = {8};
                col[3] = {12};
                break;
            }
            counter = Helper.setBitsCount(col, sizeof(col) / sizeof(col[0]));
        }
        else // diagonal
        {
            u32 diag[]{0};
            switch (params.diagno)
            {
            case 2:
                diag[0] = {1};
                diag[1] = {6};
                diag[2] = {11};
                diag[3] = {12};
                break;
            case 3:
                diag[0] = {2};
                diag[1] = {7};
                diag[2] = {8};
                diag[3] = {3};
                break;
            case 4:
                diag[0] = {3};
                diag[1] = {4};
                diag[2] = {9};
                diag[3] = {14};
                break;
            default:
                diag[0] = {0};
                diag[1] = {5};
                diag[2] = {10};
                diag[3] = {15};
                break;
            }
            counter = Helper.setBitsCount(diag, sizeof(diag) / sizeof(diag[0]));
        }
        return counter;
    }
};

struct PrintStateParams
{
    u32 *x;
    size_t size;
    bool matrixform, binaryform;

    // Constructor with default values
    constexpr PrintStateParams(u32 *x = nullptr, bool matrixform = true, bool binaryform = true)
        : x(x), size(16), matrixform(matrixform), binaryform(binaryform) {}
};

void PrintState(const PrintStateParams &params)
{
    constexpr int elementsPerRow = 4;

    if (params.matrixform)
    {
        if (params.binaryform)
        {
            for (size_t index = 0; index < params.size; ++index)
            {
                std::bitset<32> temp(params.x[index]);
                std::cout << temp << "  ";
                if (index > 0 && index % elementsPerRow == elementsPerRow - 1)
                    std::cout << '\n';
            }
            std::cout << '\n';
        }
        else
        {
            for (size_t index = 0; index < params.size; ++index)
            {
                std::cout << std::setw(8) << std::hex << params.x[index] << ' ';
                if (index > 0 && index % elementsPerRow == elementsPerRow - 1)
                    std::cout << '\n';
            }
            std::cout << '\n';

        }
    }
    else 
    {
        for (size_t index = 0; index < params.size; ++index)
        {
            std::bitset<32> temp(params.x[index]);
            std::cout << temp << "  ";
        }
        std::cout << '\n';
    }
}

struct DetailsParams
{
    const char *cipher, *progtype, *PNBmode, *PNBvaluemode;
    u16 totalround, *IDword, *IDbit, *ODword, *ODbit, fwdround;
    size_t IDsize, ODsize, PNBcount;
    ull totalLoopCount, samplesPerloop, samplesPerkey, totalSampleKey;
    double nm;
    bool halfroundflag;

    // Constructor with default values
    DetailsParams(const char *cipher = nullptr, const char *progtype = nullptr,
                  u16 totalround = 0, u16 *IDword = nullptr, u16 *IDbit = nullptr, u16 *ODword = nullptr, u16 *ODbit = nullptr, size_t IDsize = 0, size_t ODsize = 0, u16 fwdround = 0,
                  const char *PNBmode = nullptr, const char *PNBvaluemode = nullptr,
                  ull totalSamples = 0, ull samplesPerkey = 0, ull samplesPerloop = 0, ull totalSampleKey = 0, size_t PNBcount = 0, double nm = 0, bool halfroundflag = false)
        : cipher(cipher), progtype(progtype), PNBmode(PNBmode), PNBvaluemode(PNBvaluemode),
          totalround(totalround), IDword(IDword), IDbit(IDbit), ODword(ODword), ODbit(ODbit),
          IDsize(IDsize), ODsize(ODsize), fwdround(fwdround), PNBcount(PNBcount),
          totalLoopCount(totalSamples), samplesPerkey(samplesPerkey), samplesPerloop(samplesPerloop),
          totalSampleKey(totalSampleKey), nm(nm), halfroundflag(halfroundflag) {}
};

void Details(const DetailsParams &params, std::ostream &output)
{
    output << std::setw(4) << "+------------------------------------------------------------------------------------+\n";

    if (params.cipher)
        output << std::setw(4) << "Cipher Name: " << params.cipher << "\n";

    if (params.progtype)
        output << std::setw(4) << params.progtype << std::setw(7) << "\n";

    output << " # of total rounds: " << (params.halfroundflag ? (unsigned)params.totalround + 0.5 : (unsigned)params.totalround)
           << ", # of forward rounds: " << params.fwdround << "  " << std::setw(7) << "\n";

    if (params.IDword)
    {
        output << " ID = ";
        printArrayPairs(params.IDword, params.IDbit, params.IDsize, output);
        output << " OD = ";
        printArrayPairs(params.ODword, params.ODbit, params.ODsize, output);
    }

    if (params.nm)
        output << " Neutrality Measure: " << params.nm << std::setw(7) << "\n";

    if (params.PNBcount)
        output << " Number of PNBs: " << params.PNBcount << std::setw(7) << "\n";

    if (params.PNBmode)
        output << " Values of PNBs are in " << params.PNBmode << " mode" << std::setw(7) << "\n";

    if (params.totalSampleKey)
        output << " Total key samples : 2^{" << log2(params.totalSampleKey) << "}\n";

    output << " Samples per loop : 2^{" << log2(params.samplesPerloop) << "} " << std::setw(7) << "\n";
    output << " Samples per key : 2^{" << log2(params.samplesPerkey) << "} " << std::setw(7) << "\n";
    output << " Total samples : 2^{" << log2(params.totalLoopCount) << "} " << std::setw(7) << "\n";

    output << std::setw(4) << "+------------------------------------------------------------------------------------+\n";
}

void printArrayPairs(const u16 *words, const u16 *bits, size_t size, std::ostream &output)
{
    for (int i = 0; i < size; ++i)
    {
        output << "(" << words[i] << ", " << bits[i] << ")";
        output << (i == size - 1 ? "  " : "⊕");
    }
    output << std::setw(7) << "\n";
}
