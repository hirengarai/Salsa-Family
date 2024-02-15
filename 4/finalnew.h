/*
 * REFERENCE IMPLEMENTATION OF some common functions that is used in both Salsa and ChaCha cipher
 *
 * Filename: commonfiles.h
 *
 * created: 23/09/23
 * updated: 16/01/24
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
#include <random>    // random number generator
#include <vector>    // vector

typedef uint16_t u16;           // positive integer of 16 bits
typedef uint32_t u32;           // positive integer of 32 bits
typedef unsigned long long ull; // 32 - 64 bits memory

constexpr int WORD_SIZE = std::numeric_limits<u32>::digits;
constexpr u32 MOD = (1ULL << (WORD_SIZE - 1)); // pow(2, WORD_SIZE)

const size_t WORD_COUNT = 16; // state is formed by sixteen 32-bit words
const size_t KEY_COUNT = 8;   // state is formed by eight 32-bit keyWords

const size_t SALSA_IV_START = 6;
const size_t SALSA_IV_END = 9;

const size_t CHACHA_IV_START = 12;
const size_t CHACHA_IV_END = 15;

const size_t CHACHA_KEY_START = 4;
const size_t CHACHA_KEY_END = 11;

const size_t BLOCK_LEN = 5; // if PNBs are in block form, then the block length

// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// random number of 32 bits
static inline u32 dRandom()
{
    return MOD * drand48();
}

static inline double unitRandom()
{
    static std::default_random_engine generator(std::random_device{}());
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}
// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!
// ChaCha round functions helper
inline u32 rotateLeft(u32 x, u32 n)
{
    return (x << n) | (x >> (WORD_SIZE - n));
}

inline u32 rotateRight(u32 x, u32 n)
{
    return (x >> n) | (x << (WORD_SIZE - n));
}

inline u32 update(u32 a, u32 b, u32 n)
{
    return rotateLeft((a ^ b), n);
}
// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// bit of word
inline bool GetBit(u32 word, u16 bit)
{
    return (bit >= 0 && bit < sizeof(word) * 8) ? ((word >> bit) & 0x1) : false;
}
// unsets bit of word
inline void UnsetBit(u32 &word, u16 bit)
{
    if (bit >= 0 && bit < sizeof(word) * 8)
        word &= ~(0x1u << bit);
}
// sets bit of word
inline void SetBit(u32 &word, u16 bit)
{
    if (bit >= 0 && bit < sizeof(word) * 8)
        word |= (0x1u << bit);
}
// toggles bit of word
inline void ToggleBit(u32 &word, u16 bit)
{
    if (bit >= 0 && bit < sizeof(word) * 8)
        word ^= (0x1u << bit);
}
// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

enum BitOperation
{
    SURE_TOGGLE,
    TOGGLE,
    UNSET,
    SET
};

// Example usage:
// ModifyBits(myWord, myBit, TOGGLE);
// ModifyBits(myWord, myBit, UNSET, true);
// ModifyBits(myWord, myBit, SET, false);

enum ChaChaRotationConstant
{
    ROTATION_7 = 7,
    ROTATION_8 = 8,
    ROTATION_12 = 12,
    ROTATION_16 = 16
};

// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// All the parameters are in this namespace
namespace Parameters
{

    // struct Details
    // {
    //     ull totalsamples, samplesPerloop;
    //     u16 ROUNDS, FWDROUNDS;
    //     Details(ull totalsamples = 0, ull samplesPerloop =0, u16 ROUNDS = 0, u16 FWDROUNDS = 0) : totalsamples(totalsamples), ROUNDS(ROUNDS), FWDROUNDS(FWDROUNDS), samplesPerloop(samplesPerloop) {}
    // } details;

    // parameters for PNB values
    struct PNBValuedetails
    {
        u32 *x, *x1;
        u16 *pnb, *pnbinblock, *restpnb;
        size_t pnbsize, pnbinblocksize, restpnbsize;
        bool pnbblockflag, pnbvaluechange_randomflag, fixed_pnbrandomvaluechangeflag; // pnbvaluechange_randomflag: values will be 0/1 randomly, fixed_pnbrandomvaluechangeflag: values will 0/1 for sure

        // Constructor with default values
        PNBValuedetails(bool pnbblockflag = false, bool pnbvaluechange_randomflag = true, bool fixed_pnbrandomvaluechangeflag = false, u32 *x = nullptr, u32 *dx = nullptr, u16 *pnb = nullptr, size_t pnbsize = 0, u16 *pnbinblock = nullptr, u16 *restpnb = nullptr, size_t pnbinblocksize = 0, size_t restpnbssize = 0)
            : fixed_pnbrandomvaluechangeflag(fixed_pnbrandomvaluechangeflag), pnbvaluechange_randomflag(pnbvaluechange_randomflag), pnbblockflag(pnbblockflag), x(x), x1(x1), pnb(pnb), pnbinblock(pnbinblock), restpnb(restpnb), pnbsize(pnbsize), pnbinblocksize(pnbinblocksize), restpnbsize(restpnbsize) {}
    } pnbvalue;

    // struct PrintStateParams
    // {
    //     u32 *x;
    //     size_t size;
    //     bool matrixform, binaryform;

    //     // Constructor with default values
    //     PrintStateParams(u32 *x = nullptr, size_t size = WORD_COUNT, bool matrixform = true, bool binaryform = false)
    //         : x(x), size(size), matrixform(matrixform), binaryform(binaryform) {}
    // } prntst;

    struct SetBitsCounterParams
    {
        u32 *x;
        bool columnflag;
        int columnno, diagno;
        int word;

        // Constructor with default values
        SetBitsCounterParams(u32 *x = nullptr, bool columnflag = true, int columnno = 0, int diagno = 0, int word = 0)
            : x(x), columnflag(columnflag), columnno(columnno), diagno(diagno), word(word) {}
    } setbitscount;
}
// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// All the helper functions are in this namespace
namespace Operation
{
    // class INITIALISE_KEY
    {
    public:
        void Key256(u32 * k, u32 value = 1)
        {
            if (value)
            {
                for (int index{0}; index < KEY_COUNT; ++index)
                    k[index] = dRandom();
            }
            else
            {
                for (int index{0}; index < KEY_COUNT; ++index)
                    k[index] = 0;
            }
        }
        void Key128(u32 * k, u32 value = 1)
        {
            if (value)
            {
                for (int index{0}; index < KEY_COUNT / 2; ++index)
                {
                    k[index] = dRandom();
                    k[index + 4] = k[index];
                }
            }
            else
            {
                for (int index{0}; index < KEY_COUNT / 2; ++index)
                {
                    k[index] = 0;
                    k[index + 4] = k[index];
                }
            }
        }
    }
    initkey;

    class OPERATE
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
            modifyBits(x[word], bit, SURE_TOGGLE);
        }

        void modifyBits(u32 &word, u16 bit, BitOperation operation)
        {
            // if (arbitrarychangeflag)
            {
                switch (operation)
                {
                case SURE_TOGGLE:
                    ToggleBit(word, bit);
                    break;
                case TOGGLE:
                    if (drand48() < 0.5)
                        ToggleBit(word, bit);
                    break;
                case UNSET:
                    if (drand48() > 0.5)
                        UnsetBit(word, bit);
                    break;
                case SET:
                    if (drand48() < 0.5)
                        SetBit(word, bit);
                    break;
                default:
                    // Handle error or default case
                    break;
                }
            }
            // else
            // {
            //     switch (operation)
            //     {
            //     case TOGGLE:
            //         ToggleBit(word, bit);
            //         break;
            //     case UNSET:
            //         UnsetBit(word, bit);
            //         break;
            //     case SET:
            //         SetBit(word, bit);
            //         break;
            //     default:
            //         // Handle error or default case
            //         break;
            //     }
            // }
        }
        // void printState(Parameters::PrintStateParams &params)
        // {
        //     if (params.binaryform)
        //     {
        //         if (params.matrixform)
        //         {
        //             for (size_t index{0}; index < params.size; ++index)
        //             {
        //                 std::bitset<32> temp(params.x[index]);
        //                 std::string bitsString = temp.to_string();

        //                 // Insert a gap after every 8 bits
        //                 for (size_t i = 8; i < bitsString.size(); i += 9)
        //                 {
        //                     bitsString.insert(i, " ");
        //                 }
        //                 std::cout << bitsString << " | ";
        //                 if (index > 0 && index % 4 == 3)
        //                     std::cout << "\n";
        //             }
        //             std::cout << "\n";
        //         }
        //         else
        //         {
        //             for (size_t index{0}; index < params.size; ++index)
        //             {
        //                 std::bitset<32> temp(params.x[index]);
        //                 std::cout << temp << "  ";
        //             }
        //             std::cout << "\n";
        //         }
        //     }
        //     else if (params.matrixform)
        //     {
        //         for (size_t index{0}; index < params.size; ++index)
        //         {
        //             printf("%8x ", params.x[index]);
        //             if (index > 0 && index % 4 == 3)
        //                 std::cout << "\n";
        //         }
        //         std::cout << "\n";
        //     }
        //     else
        //     {
        //         for (size_t index{0}; index < params.size; ++index)
        //             printf("%u ", params.x[index]);
        //         std::cout << "\n";
        //     }
        // }
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
        // returns the total number of set bits in the array of corr. size
        int setBitsCount(u32 *x, u16 *indexarr, size_t indexarrsize)
        {
            int temp{0};
            for (size_t index{0}; index < indexarrsize; ++index)
                temp += __builtin_popcount(x[indexarr[index]]);
            return temp;
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
    } operate;

    class SET_BITS_COUNTER
    {
    public:
        int SetBitsCount(const Parameters::SetBitsCounterParams &params)
        {
            int counter;
            if (params.columnflag) // column
            {
                u16 col[4]{0};
                switch (params.columnno)
                {
                case 2:
                    col[0] = 5;
                    col[1] = 9;
                    col[2] = 13;
                    col[3] = 1;
                    break;
                case 3:
                    col[0] = 10;
                    col[1] = 14;
                    col[2] = 2;
                    col[3] = 6;
                    break;
                case 4:
                    col[0] = 15;
                    col[1] = 3;
                    col[2] = 7;
                    col[3] = 11;
                    break;
                default:
                    col[0] = 0;
                    col[1] = 4;
                    col[2] = 8;
                    col[3] = 12;
                    break;
                }
                counter = operate.setBitsCount(params.x, col, sizeof(col) / sizeof(col[0]));
            }
            else // diagonal
            {
                u16 diag[4]{0};
                switch (params.diagno)
                {
                case 2:
                    diag[0] = 1;
                    diag[1] = 6;
                    diag[2] = 11;
                    diag[3] = 12;
                    break;
                case 3:
                    diag[0] = 2;
                    diag[1] = 7;
                    diag[2] = 8;
                    diag[3] = 3;
                    break;
                case 4:
                    diag[0] = 3;
                    diag[1] = 4;
                    diag[2] = 9;
                    diag[3] = 14;
                    break;
                default:
                    diag[0] = 0;
                    diag[1] = 5;
                    diag[2] = 10;
                    diag[3] = 15;
                    break;
                }
                counter = operate.setBitsCount(params.x, diag, sizeof(diag) / sizeof(diag[0]));
            }
            return counter;
        }
    } setbitscounter;
}
// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

// All the functions related to ChaCha cipher are in this namespace
namespace ChaCha
{
    // --------------------------- ChaCha FW QR -----------------------------------
    inline void fwdQR(u32 &a, u32 &b, u32 &c, u32 &d, ChaChaRotationConstant fr, bool X = false)
    {
        switch (fr)
        {
        case ROTATION_16: // ARX_16
            (X) ? (a ^= b) : (a += b);
            d = update(a, d, 16);
            break;
        case ROTATION_12: // ARX_12
            (X) ? (c ^= d) : (c += d);
            b = update(b, c, 12);
            break;
        case ROTATION_8: // ARX_8
            (X) ? (a ^= b) : (a += b);
            d = update(a, d, 8);
            break;
        case ROTATION_7: // ARX_7
            (X) ? (c ^= d) : (c += d);
            b = update(b, c, 7);
            break;
        default:
            break;
        }
    }
    // --------------------------- ChaCha BW QR -----------------------------------
    inline void bwdQR(u32 &a, u32 &b, u32 &c, u32 &d, ChaChaRotationConstant fr, bool X = false)
    {
        switch (fr)
        {
        case ROTATION_7:
            b = rotateRight(b, 7) ^ c;
            (X) ? (c ^= d) : (c -= d);
            break;
        case ROTATION_8:
            d = rotateRight(d, 8) ^ a;
            (X) ? (a ^= b) : (a -= b);
            break;
        case ROTATION_12:
            b = rotateRight(b, 12) ^ c;
            (X) ? (c ^= d) : (c -= d);
            break;
        case ROTATION_16:
            d = rotateRight(d, 16) ^ a;
            (X) ? (a ^= b) : (a -= b);
            break;
        default:
            break;
        }
    }
    // put value = 0 to get the 0 state matrix
    void inisialize(u32 *x, u32 value = 1)
    {
        if (value)
        {
            x[0] = 0x61707865;
            x[1] = 0x3320646e;
            x[2] = 0x79622d32;
            x[3] = 0x6b206574;
            for (size_t index{CHACHA_IV_START}; index <= CHACHA_IV_END; ++index)
                x[index] = dRandom(); // IV
        }
        else
        {
            for (size_t index{0}; index < WORD_COUNT; ++index)
                x[index] = value;
        }
    }

    // fitting the key k into the state matrix x
    void insertkey(u32 *x, u32 *k)
    {
        for (size_t index{CHACHA_KEY_START}; index <= CHACHA_KEY_END; ++index)
            x[index] = k[index - 4];
    }
    // calculates the position of the index in the state matrix
    void calculateWORDandBIT(int index, u32 &WORD, u16 &BIT)
    {
        WORD = (index / WORD_SIZE) + 4;
        BIT = index % WORD_SIZE;
    }
    void inputvalueTopnb(Parameters::PNBValueParams &params)
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
                    Operation::operate.modifyBits(params.x[WORD], BIT, SET);
                    if (params.x1)
                        Operation::operate.modifyBits(params.x1[WORD], BIT, SET);
                }
                else
                {
                    Operation::operate.modifyBits(params.x[WORD], BIT, UNSET);
                    if (params.x1)
                        Operation::operate.modifyBits(params.x1[WORD], BIT, UNSET);
                }
            }
            for (size_t index{0}; index < params.restpnbsize; ++index)
            {
                calculateWORDandBIT(params.pnb[index], WORD, BIT);

                Operation::operate.modifyBits(params.x[WORD], BIT, TOGGLE);
                if (params.x1)
                    Operation::operate.modifyBits(params.x1[WORD], BIT, TOGGLE);
            }
        }
        else
        {
            if (params.pnbvaluechange_randomflag) // random values
            {
                for (size_t index{0}; index < params.pnbsize; ++index)
                {
                    calculateWORDandBIT(params.pnb[index], WORD, BIT);
                    if (params.fixed_pnbrandomvaluechangeflag)
                    {
                        Operation::operate.modifyBits(params.x[WORD], BIT, SURE_TOGGLE);
                        if (params.x1)
                            Operation::operate.modifyBits(params.x1[WORD], BIT, SURE_TOGGLE);
                    }
                    else
                    {
                        Operation::operate.modifyBits(params.x[WORD], BIT, TOGGLE);
                        if (params.x1)
                            Operation::operate.modifyBits(params.x1[WORD], BIT, TOGGLE);
                    }
                }
            }
            else // 0 values
            {
                for (size_t index{0}; index < params.pnbsize; ++index)
                {
                    calculateWORDandBIT(params.pnb[index], WORD, BIT);

                    Operation::operate.modifyBits(params.x[WORD], BIT, UNSET);
                    if (params.x1)
                        Operation::operate.modifyBits(params.x1[WORD], BIT, UNSET);
                }
            }
        }
    }

    // class FORWARD
    // {
    // public:
    //     // XOR version of full round functions, round means even or odd round
    //     void XRoundFunction(u32 *x, u16 round)
    //     {
    //         if (round & 1)
    //         {
    //             fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_16, true); // column 1
    //             fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_12, true); // column 1
    //             fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_8, true);  // column 1
    //             fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_7, true);  // column 1

    //             fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_16, true); // column 2
    //             fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_12, true); // column 2
    //             fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_8, true);  // column 2
    //             fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_7, true);  // column 2

    //             fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_16, true); // column 3
    //             fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_12, true); // column 3
    //             fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_8, true);  // column 3
    //             fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_7, true);  // column 3

    //             fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_16, true); // column 4
    //             fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_12, true); // column 4
    //             fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_8, true);  // column 4
    //             fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_7, true);  // column 4
    //         }
    //         else
    //         {
    //             fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_16, true); // diagonal 1
    //             fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_12, true); // diagonal 1
    //             fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_8, true);  // diagonal 1
    //             fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_7, true);  // diagonal 1

    //             fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_16, true); // diagnoal 2
    //             fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_12, true); // diagnoal 2
    //             fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_8, true);  // diagnoal 2
    //             fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_7, true);  // diagnoal 2

    //             fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_16, true); // diagonal 3
    //             fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_12, true); // diagonal 3
    //             fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_8, true);  // diagonal 3
    //             fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_7, true);  // diagonal 3

    //             fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_16, true); // diagonal 4
    //             fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_12, true); // diagonal 4
    //             fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_8, true);  // diagonal 4
    //             fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_7, true);  // diagonal 4
    //         }
    //     }

    //     // half round functions
    //     // half 16-12
    //     // void Half_1_EvenRF(u32 *x)
    //     // {
    //     //   FWQR_16_12((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    //     //   FWQR_16_12((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    //     //   FWQR_16_12((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    //     //   FWQR_16_12((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
    //     // }
    //     // // half 8-7
    //     // void Half_2_EvenRF(u32 *x)
    //     // {
    //     //   FWQR_8_7((x[0]), (x[5]), (x[10]), (x[15])); // diagonal 1
    //     //   FWQR_8_7((x[1]), (x[6]), (x[11]), (x[12])); // diagonal 2
    //     //   FWQR_8_7((x[2]), (x[7]), (x[8]), (x[13]));  // diagonal 3
    //     //   FWQR_8_7((x[3]), (x[4]), (x[9]), (x[14]));  // diagonal 4
    //     // }
    //     // // half 16-12
    //     // void Half_1_OddRF(u32 *x)
    //     // {
    //     //   FWQR_16_12((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    //     //   FWQR_16_12((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    //     //   FWQR_16_12((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    //     //   FWQR_16_12((x[3]), (x[7]), (x[11]), (x[15])); // column 4
    //     // }
    //     // // half 8-7
    //     // void Half_2_OddRF(u32 *x)
    //     // {
    //     //   FWQR_8_7((x[0]), (x[4]), (x[8]), (x[12]));  // column 1
    //     //   FWQR_8_7((x[1]), (x[5]), (x[9]), (x[13]));  // column 2
    //     //   FWQR_8_7((x[2]), (x[6]), (x[10]), (x[14])); // column 3
    //     //   FWQR_8_7((x[3]), (x[7]), (x[11]), (x[15])); // column 4
    //     // }

    //     void Quarter_1_EvenRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_16); // diagonal 1
    //         fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_16); // diagonal 2
    //         fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_16);  // diagonal 3
    //         fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_16);  // diagonal 4
    //     }

    //     void Quarter_1_OddRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_16);  // column 1
    //         fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_16);  // column 2
    //         fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_16); // column 3
    //         fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_16); // column 4
    //     }

    //     void Quarter_2_EvenRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_12); // diagonal 1
    //         fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_12); // diagonal 2
    //         fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_12);  // diagonal 3
    //         fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_12);  // diagonal 4
    //     }

    //     void Quarter_2_OddRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_12);  // column 1
    //         fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_12);  // column 2
    //         fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_12); // column 3
    //         fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_12); // column 4
    //     }

    //     void Quarter_3_EvenRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_8); // diagonal 1
    //         fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_8); // diagonal 2
    //         fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_8);  // diagonal 3
    //         fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_8);  // diagonal 4
    //     }

    //     void Quarter_3_OddRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_8);  // column 1
    //         fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_8);  // column 2
    //         fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_8); // column 3
    //         fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_8); // column 4
    //     }

    //     void Quarter_4_EvenRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_7); // diagonal 1
    //         fwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_7); // diagonal 2
    //         fwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_7);  // diagonal 3
    //         fwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_7);  // diagonal 4
    //     }

    //     void Quarter_4_OddRF(u32 *x)
    //     {
    //         fwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_7);  // column 1
    //         fwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_7);  // column 2
    //         fwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_7); // column 3
    //         fwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_7); // column 4
    //     }
    //     void Half_1_EvenRF(u32 *x)
    //     {
    //         Quarter_1_EvenRF(x);
    //         Quarter_2_EvenRF(x);
    //     }
    //     void Half_1_OddRF(u32 *x)
    //     {
    //         Quarter_1_OddRF(x);
    //         Quarter_2_OddRF(x);
    //     }

    //     void Half_2_EvenRF(u32 *x)
    //     {
    //         Quarter_3_EvenRF(x);
    //         Quarter_4_EvenRF(x);
    //     }

    //     void Half_2_OddRF(u32 *x)
    //     {
    //         Quarter_3_OddRF(x);
    //         Quarter_4_OddRF(x);
    //     }
    //     // full round function, round means even or odd round
    //     void RoundFunction(u32 *x, u32 round)
    //     {
    //         if (round & 1)
    //         {
    //             Half_1_OddRF(x);
    //             Half_2_OddRF(x);
    //         }
    //         else
    //         {
    //             Half_1_EvenRF(x);
    //             Half_2_EvenRF(x);
    //         }
    //     }
    // } fwd;
    // class BACKWARD
    // {
    // public:
    //     // // XOR function
    //     // void XFWRound(u32 *x, int round)
    //     // { // round numbering means even or odd round
    //     //   if (round & 1)
    //     //   {
    //     //     XFWfullQR((x[0]), (x[4]), (x[8]), (x[12]));
    //     //     XFWfullQR((x[1]), (x[5]), (x[9]), (x[13]));
    //     //     XFWfullQR((x[2]), (x[6]), (x[10]), (x[14]));
    //     //     XFWfullQR((x[3]), (x[7]), (x[11]), (x[15]));
    //     //   }
    //     //   else
    //     //   {
    //     //     XFWfullQR((x[0]), (x[5]), (x[10]), (x[15]));
    //     //     XFWfullQR((x[1]), (x[6]), (x[11]), (x[12]));
    //     //     XFWfullQR((x[2]), (x[7]), (x[8]), (x[13]));
    //     //     XFWfullQR((x[3]), (x[4]), (x[9]), (x[14]));
    //     //   }
    //     // }

    //     void Quarter_1_EvenRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_7); // diagonal 1
    //         bwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_7); // diagonal 2
    //         bwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_7);  // diagonal 3
    //         bwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_7);  // diagonal 4
    //     }

    //     void Quarter_1_OddRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_7);  // column 1
    //         bwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_7);  // column 2
    //         bwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_7); // column 3
    //         bwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_7); // column 4
    //     }

    //     void Quarter_2_EvenRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_8); // diagonal 1
    //         bwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_8); // diagonal 2
    //         bwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_8);  // diagonal 3
    //         bwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_8);  // diagonal 4
    //     }

    //     void Quarter_2_OddRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_8);  // column 1
    //         bwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_8);  // column 2
    //         bwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_8); // column 3
    //         bwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_8); // column 4
    //     }

    //     void Quarter_3_EvenRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_12); // diagonal 1
    //         bwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_12); // diagonal 2
    //         bwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_12);  // diagonal 3
    //         bwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_12);  // diagonal 4
    //     }

    //     void Quarter_3_OddRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_12);  // column 1
    //         bwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_12);  // column 2
    //         bwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_12); // column 3
    //         bwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_12); // column 4
    //     }

    //     void Quarter_4_EvenRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[5]), (x[10]), (x[15]), ROTATION_16); // diagonal 1
    //         bwdQR((x[1]), (x[6]), (x[11]), (x[12]), ROTATION_16); // diagonal 2
    //         bwdQR((x[2]), (x[7]), (x[8]), (x[13]), ROTATION_16);  // diagonal 3
    //         bwdQR((x[3]), (x[4]), (x[9]), (x[14]), ROTATION_16);  // diagonal 4
    //     }

    //     void Quarter_4_OddRF(u32 *x)
    //     {
    //         bwdQR((x[0]), (x[4]), (x[8]), (x[12]), ROTATION_16);  // column 1
    //         bwdQR((x[1]), (x[5]), (x[9]), (x[13]), ROTATION_16);  // column 2
    //         bwdQR((x[2]), (x[6]), (x[10]), (x[14]), ROTATION_16); // column 3
    //         bwdQR((x[3]), (x[7]), (x[11]), (x[15]), ROTATION_16); // column 4
    //     }
    //     void Half_1_EvenRF(u32 *x)
    //     {
    //         Quarter_1_EvenRF(x);
    //         Quarter_2_EvenRF(x);
    //     }
    //     void Half_1_OddRF(u32 *x)
    //     {
    //         Quarter_1_OddRF(x);
    //         Quarter_2_OddRF(x);
    //     }

    //     void Half_2_EvenRF(u32 *x)
    //     {
    //         Quarter_3_EvenRF(x);
    //         Quarter_4_EvenRF(x);
    //     }

    //     void Half_2_OddRF(u32 *x)
    //     {
    //         Quarter_3_OddRF(x);
    //         Quarter_4_OddRF(x);
    //     }
    //     // full round function, round means even or odd round
    //     void RoundFunction(u32 *x, u32 round)
    //     {
    //         if (round & 1)
    //         {
    //             Half_1_OddRF(x);
    //             Half_2_OddRF(x);
    //         }
    //         else
    //         {
    //             Half_1_EvenRF(x);
    //             Half_2_EvenRF(x);
    //         }
    //     }
    // } bwd;
}
// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!
