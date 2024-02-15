/*
 * REFERENCE IMPLEMENTATION OF some common functions that is used in both Salsa and ChaCha cipher
 *
 * Filename: commonfiles.h
 *
 * created: 23/09/23
 * updated: 31/01/24
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
#include <iomanip>   // decimal numbers upto certain places, std::setw function
#include <mutex>     // thread locking
#include <random>
#include <string>
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

constexpr size_t BLOCK_LEN = 5; // if PNBs are in block form, then the block length

#define GetBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word >> bit) & 0x1) : false)
#define SetBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word) |= (1u << bit)) : false)
#define UnsetBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word) &= ~(1u << bit)) : false)
#define ToggleBit(word, bit) ((bit >= 0 && bit < sizeof(word) * 8) ? ((word) ^= (1u << bit)) : false)

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (WORD_SIZE - n)))

// random number of 32 bits
// static inline u32 GenerateRandom32Bits()
// {
//     // static std::mutex mt_mutex;                 // Static ensures that the mutex is shared among all calls to the function
//     // std::lock_guard<std::mutex> lock(mt_mutex); // Lock the mutex during random number generation
//     return MOD * drand48();
// }

u32 GenerateRandom32Bits()
{
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<uint_fast32_t> dis(1, MOD);
    return dis(gen);
}

// u32 GenerateRandom32Bits()
// {
//     static thread_local std::minstd_rand gen(std::random_device{}());
//     std::uniform_int_distribution<uint32_t> dis(0, MOD);
//     return dis(gen);
// }

// u32 GenerateRandom32Bits()
// {
//     static std::mutex mt_mutex;                 // Static ensures that the mutex is shared among all calls to the function
//     std::lock_guard<std::mutex> lock(mt_mutex); // Lock the mutex during random number generation

//     // Create a static instance of std::mt19937 to persist state between function calls
//     static std::mt19937 mt_engine(time(NULL));
//     static std::uniform_int_distribution<u32> distribution(1, MOD);

//     return distribution(mt_engine);
// }

bool GenerateRandomBoolean()
{
    thread_local std::mt19937 mt_engine(std::random_device{}());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return (distribution(mt_engine) > 0.5);

    // thread_local std::mt19937 mt_engine(static_cast<unsigned>(time(NULL)));
    // std::uniform_real_distribution<double> distribution(0.0, 1.0);
    // return (distribution(mt_engine) > 0.5);

    // return (drand48() > 0.5);
}

// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!
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
    struct basicDetailstrct
    {
        std::string cipher, programtype;
        u16 totalround;
        basicDetailstrct(std::string cipher = "", std::string programtype = "", u16 totalround = 0) : cipher(cipher), programtype(programtype), totalround(totalround) {}
    } basicdetails;

    struct diffDetailstrct
    {
        u16 fwdround, (*ID)[2], (*OD)[2];
        size_t IDsize, ODsize;
        bool halfroundflag;
        u16 precision_digit;
        diffDetailstrct(u16 fwdround = 0, u16 (*ID)[2] = nullptr, u16 (*OD)[2] = nullptr, bool halfroundflag = false, size_t IDsize = 0, u16 precision_digit = 0, size_t ODsize = 0) : fwdround(fwdround), ID(ID), OD(OD), halfroundflag(halfroundflag), IDsize(IDsize), ODsize(ODsize), precision_digit(precision_digit) {}
    } diffdetails;

    struct samplesDetailstrct
    {
        ull totalLoop, samplesperLoop, samplesperThread;
        samplesDetailstrct(ull totalLoop = 0, ull samplesperLoop = 0, ull samplesperThread = 0) : totalLoop(totalLoop), samplesperLoop(samplesperLoop), samplesperThread(samplesperThread) {}
    } sampledetails;

    // parameters for PNB values
    struct PNBvalueDetailstrct
    {
        std::string PNBfile;
        u32 *x, *x1;
        u16 *PNB, *PNBinblock, *restPNB;
        size_t PNBsize, PNBinblocksize, restPNBsize;
        bool PNBblockflag, PNBvaluechange_randomflag, sure_PNBrandomvaluechangeflag; // PNBvaluechange_randomflag: values will be 0/1 randomly, fixed_PNBrandomvaluechangeflag: values will 0/1 for sure
        double neutralitymeasure;

        // Constructor with default values
        PNBvalueDetailstrct(bool PNBblockflag = false, bool PNBvaluechange_randomflag = true, bool sure_PNBrandomvaluechangeflag = false, u32 *x = nullptr, u32 *x1 = nullptr, u16 *PNB = nullptr, size_t PNBsize = 0, u16 *PNBinblock = nullptr, u16 *restPNB = nullptr, size_t PNBinblocksize = 0, size_t restPNBsize = 0, double neutralitymeasure = 0, std::string PNBfile = "")
            : sure_PNBrandomvaluechangeflag(sure_PNBrandomvaluechangeflag), PNBvaluechange_randomflag(PNBvaluechange_randomflag), PNBblockflag(PNBblockflag), x(x), x1(x1), PNB(PNB), PNBinblock(PNBinblock), restPNB(restPNB), PNBsize(PNBsize), PNBinblocksize(PNBinblocksize), restPNBsize(restPNBsize), PNBfile(PNBfile), neutralitymeasure(neutralitymeasure) {}
    } PNBdetails;

    struct printstateDetailstrct
    {
        u32 *x;
        size_t size;
        bool matrixform, binaryform;

        // Constructor with default values
        printstateDetailstrct(u32 *x = nullptr, size_t size = WORD_COUNT, bool matrixform = true, bool binaryform = false)
            : x(x), size(size), matrixform(matrixform), binaryform(binaryform) {}
    } prntstate;

    struct setbitscounterDetailstrct
    {
        u32 *x;
        bool columnflag;
        int columnno, diagno;
        int word;

        // Constructor with default values
        setbitscounterDetailstrct(u32 *x = nullptr, bool columnflag = true, int columnno = 0, int diagno = 0, int word = 0)
            : x(x), columnflag(columnflag), columnno(columnno), diagno(diagno), word(word) {}
    } setbitscounter;
}
// !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!

namespace ChaCha
{
    u16 column[4][4] = {{0, 4, 8, 12}, {1, 5, 9, 13}, {2, 6, 10, 14}, {3, 7, 11, 15}};
    u16 diagonal[4][4] = {{0, 5, 10, 15}, {1, 6, 11, 12}, {2, 7, 8, 13}, {3, 4, 9, 14}};
    void inisialize(u32 *x, bool randflag = true, u32 value = 1)
    {
        if (randflag)
        {
            x[0] = 0x61707865;
            x[1] = 0x3320646e;
            x[2] = 0x79622d32;
            x[3] = 0x6b206574;

            for (size_t index{CHACHA_IV_START}; index <= CHACHA_IV_END; ++index)
                x[index] = GenerateRandom32Bits(); // IV
        }
        else
        {
            for (size_t index{0}; index < WORD_COUNT; ++index)
                x[index] = value;
        }
    }
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
    void valuePNB(Parameters::PNBvalueDetailstrct &params)
    {
        u32 WORD;
        u16 BIT;
        if (params.PNBblockflag)
        {
            for (int i{0}; i < params.PNBinblocksize; ++i)
            {
                ChaCha::calculateWORDandBIT(params.PNBinblock[i], WORD, BIT);
                if ((i % 5) == 4)
                {
                    SetBit(params.x[WORD], BIT);
                    if (params.x1)
                        SetBit(params.x1[WORD], BIT);
                }
                else
                {
                    UnsetBit(params.x[WORD], BIT);
                    if (params.x1)
                        UnsetBit(params.x1[WORD], BIT);
                }
            }
            for (int i{0}; i < params.restPNBsize; ++i)
            {
                ChaCha::calculateWORDandBIT(params.restPNB[i], WORD, BIT);

                if (GenerateRandomBoolean())
                {
                    ToggleBit(params.x[WORD], BIT);
                    if (params.x1)
                        ToggleBit(params.x1[WORD], BIT);
                }
            }
        }
        else
        {
            if (params.PNBvaluechange_randomflag) // random values
            {
                for (int i{0}; i < params.PNBsize; ++i)
                {
                    ChaCha::calculateWORDandBIT(params.PNB[i], WORD, BIT);
                    if (params.sure_PNBrandomvaluechangeflag) // flag:true means randomly change the PNBs
                    {
                        ToggleBit(params.x[WORD], BIT);
                        if (params.x1)
                            ToggleBit(params.x1[WORD], BIT);
                    }
                    else
                    {
                        if (GenerateRandomBoolean())
                        {
                            ToggleBit(params.x[WORD], BIT);
                            if (params.x1)
                                ToggleBit(params.x1[WORD], BIT);
                        }
                    }
                }
            }
            else // 0 values
            {
                for (int i{0}; i < params.PNBsize; ++i)
                {
                    ChaCha::calculateWORDandBIT(params.PNB[i], WORD, BIT);

                    UnsetBit(params.x[WORD], BIT);
                    if (params.x1)
                        UnsetBit(params.x1[WORD], BIT);
                }
            }
        }
    }
}

namespace Operation
{
    class INITIALISE_KEY
    {
    public:
        void Key256(u32 *k, bool randflag = true, u32 value = 0)
        {
            if (randflag)
            {
                for (int index{0}; index < KEY_COUNT; ++index)
                    k[index] = GenerateRandom32Bits();
            }
            else
            {
                for (int index{0}; index < KEY_COUNT; ++index)
                    k[index] = value;
            }
        }
        void Key128(u32 *k, bool randflag = true, u32 value = 1)
        {
            if (randflag)
            {
                for (int index{0}; index < KEY_COUNT / 2; ++index)
                {
                    k[index] = GenerateRandom32Bits();
                    k[index + 4] = k[index];
                }
            }
            else
            {
                for (int index{0}; index < KEY_COUNT / 2; ++index)
                {
                    k[index] = value;
                    k[index + 4] = k[index];
                }
            }
        }
    } init_key;

    // sum of x and x1 is stored in x
    void AddStates(u32 *x, u32 *x1)
    {
        for (int i{0}; i < WORD_COUNT; ++i)
            x[i] += x1[i];
    }
    void CopyState(u32 *x, u32 *x1)
    {
        for (int i{0}; i < WORD_COUNT; ++i)
            x[i] = x1[i];
    }
    void InputDifference(u32 *x, int word, int bit)
    {
        ToggleBit(x[word], bit);
    }

    double CalculateMedian(double *arr, size_t size)
    {
        // Copy the array to keep the original unchanged
        double *sortedArray = new double[size];
        std::copy(arr, arr + size, sortedArray);

        std::sort(sortedArray, sortedArray + size);

        if (size % 2 == 0)
        {
            // If the size is even, return the average of the middle two elements
            return static_cast<double>(sortedArray[size / 2 - 1] + sortedArray[size / 2]) / 2.0;
        }
        else
        {
            // If the size is odd, return the middle element
            return static_cast<double>(sortedArray[size / 2]);
        }
    }

    class ARRAYPRINT
    {
    public:
        void doubleprint(u16 (*arr)[2], size_t rows, size_t columns, std::string sep)
        {
            for (int index{0}; index < rows; ++index)
            {
                std::cout << "(";
                for (int jindex{0}; jindex < 2; ++jindex)
                {
                    if ((jindex == 1))
                        std::cout << (unsigned)arr[index][jindex] << ")";
                    else
                        std::cout << (unsigned)arr[index][jindex] << ", ";
                }
                if (rows == 1)
                    std::cout << "";
                if (index == rows - 1)
                    std::cout << "";
                else
                    std::cout << sep;
            }
            std::cout << "\n";
        }
        void singleprint(u16 *arr, size_t rows, std::string sep)
        {
            for (int index{0}; index < rows; ++index)
                std::cout << (unsigned)arr[index] << sep;
        }
    } arrayprint;

    // 4 cross 4 matrix form print, by default set to matrix form printing, size set to 0, set flag to false to not print in matrix form
    // bin is set to false, to print binary set it to true
    void PrintState(Parameters::printstateDetailstrct &params)
    {
        if (params.binaryform)
        {
            if (params.matrixform)
            {
                for (size_t index{0}; index < params.size; ++index)
                {
                    std::bitset<WORD_SIZE> temp(params.x[index]);
                    std::string bitsString = temp.to_string();

                    // Insert a gap after every 8 bits
                    for (size_t i = 8; i < bitsString.size(); i += 9)
                    {
                        bitsString.insert(i, " ");
                    }
                    std::cout << bitsString << " | ";
                    if (index > 0 && index % 4 == 3)
                        std::cout << "\n";
                }
                std::cout << "\n";
            }
            else
            {
                for (size_t index{0}; index < params.size; ++index)
                {
                    std::bitset<WORD_SIZE> temp(params.x[index]);
                    std::cout << temp << "  ";
                }
                std::cout << "\n";
            }
        }
        else if (params.matrixform)
        {
            for (size_t index{0}; index < params.size; ++index)
            {
                printf("%8x ", params.x[index]);
                if (index > 0 && index % 4 == 3)
                    std::cout << "\n";
            }
            std::cout << "\n";
        }
        else
        {
            for (size_t index{0}; index < params.size; ++index)
                printf("%u ", params.x[index]);
            std::cout << "\n";
        }
    }
    // all the values of x are set to value, by defualt it is set to 0
    void ResetState(u32 *x, u32 size, u32 value = -1)
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
    int SetBitsCount(u32 *x, u16 *indexarr, size_t indexarrsize)
    {
        int temp{0};
        for (size_t index{0}; index < indexarrsize; ++index)
            temp += __builtin_popcount(x[indexarr[index]]);
        return temp;
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

    // u16 DifferenceBit(u32 *x, u16 *ODword, u16 *ODbit, size_t size)
    // {
    //     u16 fwdBit{0};
    //     for (size_t index{0}; index < size; ++index)
    //         fwdBit ^= GetBit(x[ODword[index]], ODbit[index]);
    //     return fwdBit;
    // }

    u16 DifferenceBit(u32 *x, u16 (*OD)[2], size_t size)
    {
        u16 fwdBit{0};
        for (size_t row{0}; row < size; ++row)
        {
            for (size_t column{0}; column < 2; ++column)
            {
                fwdBit ^= GetBit(x[OD[row][column]], OD[row][column + 1]);
                column++;
            }
        }
        return fwdBit;
    }

    class CLASS_SET_BITS_COUNTER
    {
    public:
        int FunSetBitsCount(const Parameters::setbitscounterDetailstrct &params)
        {
            int counter;
            if (params.columnflag) // column
            {
                u16 *col;
                switch (params.columnno)
                {
                case 2:
                    col = ChaCha::column[1];
                    break;
                case 3:
                    col = ChaCha::column[2];
                    break;
                case 4:
                    col = ChaCha::column[3];
                    break;
                default:
                    col = ChaCha::column[0];
                    break;
                }
                counter = SetBitsCount(params.x, col, sizeof(col) / sizeof(col[0]));
            }
            else // diagonal
            {
                u16 *diag;
                switch (params.diagno)
                {
                case 2:
                    diag = ChaCha::diagonal[1];
                    break;
                case 3:
                    diag = ChaCha::diagonal[2];
                    break;
                case 4:
                    diag = ChaCha::diagonal[3];
                    break;
                default:
                    diag = ChaCha::diagonal[0];
                    break;
                }
                counter = SetBitsCount(params.x, diag, sizeof(diag) / sizeof(diag[0]));
            }
            return counter;
        }
    } classsetbitscounter;

    void PrintBasicDetails(Parameters::basicDetailstrct &params, std::ostream &output)
    {
        output << "+--------------------------------------------------------------------------------------------------+\n";
        if (params.cipher != "")
            output << " " << std::setw(6) << "Cipher Name: " << params.cipher << "\n";

        if (params.programtype != "")
            output << " " << std::setw(6) << "Programe Type: " << params.programtype << "\n";

        if (params.totalround)
            output << " " << std::setw(6) << "# of total rounds: " << (unsigned)params.totalround << "\n";
        // output << "+------------------------------------------------------------------------------------+\n";
    }
    void PrintDiffDetails(Parameters::diffDetailstrct &params, std::ostream &output)
    {
        if (params.fwdround)
            output << " " << std::setw(6) << "# of fwd rounds: " << (unsigned)params.fwdround << "\n";
        if (params.ID)
        {
            output << " " << std::setw(6) << "Input Differential: ";
            Operation::arrayprint.doubleprint(params.ID, params.IDsize, 2, "⊕");
        }
        if (params.OD)
        {
            output << " " << std::setw(6) << "Output Differential: ";
            Operation::arrayprint.doubleprint(params.OD, params.ODsize, 2, "⊕");
        }
        if(params.precision_digit)
            output << " The degree of precision is upto " << params.precision_digit - 1 << " digits after decimal\n";
        // output << "+------------------------------------------------------------------------------------+\n";
    }

    void PrintPNBDetails(Parameters::PNBvalueDetailstrct &params, std::ostream &output)
    {
        if (params.PNBfile != "")
        {
            output << " " << std::setw(6) << "PNBs are from the file " << params.PNBfile << "\n";
            if (params.PNBblockflag)
            {
                output << " " << std::setw(6) << "PNBs are in block mode"
                       << "\n";
                output << " " << std::setw(6) << "# of PNBs in block mode: " << params.PNBinblocksize << ", # of rest of the PNBs: " << params.restPNBsize << "\n";
            }
            else
            {
                output << " " << std::setw(6) << "PNBs are not in block mode"
                       << "\n";
                output << " " << std::setw(6) << "# of PNBs: " << params.PNBsize << "\n";
            }
            if (params.sure_PNBrandomvaluechangeflag)
                output << " " << std::setw(6) << "PNB values are changed deliberatley\n";
        }
        // output << "+------------------------------------------------------------------------------------+\n";
    }

    void PrintSampleDetails(Parameters::samplesDetailstrct &params, std::ostream &output)
    {
        if (params.samplesperLoop)
            output << " " << std::setw(6) << "Samples Per Loop: 2^{" << log2(params.samplesperLoop) << "}\n";
        if (params.totalLoop)
            output << " " << std::setw(6) << "Total Loop Count: 2^{" << log2(params.totalLoop) << "}\n";
        if (params.samplesperThread)
            output << " " << std::setw(6) << "Samples Per Thread: 2^{" << log2(params.samplesperThread) << "}\n";
    }
    void PrintBiasLoopEtc(std::ostream &output)
    {
        output << "+----------------------+----------------------------------+---------------------------------+---------------------+\n";
        output << "|" << std::setw(20) << "🔢Loop Count" << std::setw(5)
               << "|" << std::setw(25) << "🅱️Bias" << std::setw(15)
               << "|" << std::setw(32) << "⌛Apprx. exec. time (seconds)" << std::setw(3)
               << "|" << std::setw(17) << "📝Remarks" << std::setw(8)
               << "|\n";
        output << "+----------------------+----------------------------------+---------------------------------+---------------------+\n";
    }
};

// cipher name, total round, number of forward rounds, IDword array, IDbit array, total number of IDs, ODword array, ODbit array, total number of ODs,

//     else
//     {
//         output << "            # of total rounds: " << (unsigned)params.totalround << "\n";
//     }

//     if (params.IDword)
//     {
//         output << "            # of forward rounds: " << params.fwdround << "  " << std::std::setw(7) << "\n";
//         output << std::std::setw(4) << "            ID = ";

//         for (int i{0}; i < params.IDsize; ++i)
//         {
//             if (i == params.IDsize - 1)
//                 output << "(" << params.IDword[i] << ", " << params.IDbit[i] << "), ";
//             else
//             {
//                 output << "(" << params.IDword[i] << ", " << params.IDbit[i] << ")⊕";
//             }
//         }
//         output << "OD = ";
//         for (int i{0}; i < params.ODsize; ++i)
//         {
//             if (i == params.ODsize - 1)
//                 output << "(" << params.ODword[i] << ", " << params.ODbit[i] << ")                            " << std::std::setw(7) << "\n";
//             else
//             {
//                 output << "(" << params.ODword[i] << ", " << params.ODbit[i] << ")⊕";
//             }
//         }
//     }
//     if (params.nm)
//         output << std::std::setw(2) << "            Neutrality Measure: " << params.nm << std::std::setw(7) << "\n";
//     if (params.PNBcount)
//         output << std::std::setw(5) << "            Number of PNBs: " << params.PNBcount << std::std::setw(7) << "\n";
//     if (params.PNBmode)
//         output << std::std::setw(5) << "            Values of PNBs are in " << params.PNBmode << " mode                                  " << std::std::setw(7) << "\n";

//     if (params.samplesPerloop)
//     {
//         output << std::std::setw(4) << "            Samples per loop : 2^{" << log2(params.samplesPerloop) << "}" << std::std::setw(7) << "\n";
//         output << std::std::setw(4) << "            Total samples :    2^{" << log2(params.samplesPerloop * params.totalLoopCount) << "}" << std::std::setw(7) << "\n";
//     }

//     output << std::std::setw(4) << "+------------------------------------------------------------------------------------+\n";
// }

// void BiasSrotedPrint(std::vector<double> &PNB_BIAS, std::vector<u16> &PNB, std::ostream &output)
// {
//     int flag;
//     size_t size = PNB.size();
//     for (size_t l{size}; l >= 0; --l)
//     {
//         flag = 0;
//         for (int j = 0; j < l; ++j)
//         {
//             if (PNB_BIAS[j] < PNB_BIAS[j + 1])
//             {
//                 std::swap(PNB_BIAS.at(j), PNB_BIAS.at(j + 1));
//                 std::swap(PNB.at(j), PNB.at(j + 1));
//                 flag = 1;
//             }
//         }
//         if (flag == 0)
//             break;
//     }
//     output << "\nThe PNBs in descending order of BIAS is as follows:\n{";
//     bool firstElement = true;
//     for (auto const &i : PNB)
//     {
//         if (!firstElement)
//             output << ", ";
//         output << i;
//         firstElement = false;
//     }
//     output << "}\n";
// }

u16 *OpenFile(const char *file, size_t *PNBcount)
{
    std::ifstream PNB(file);

    if (!PNB.is_open())
    {
        std::cout << "|          ⚠ Unable to open the file ⚠                                          " << std::setw(7) << "|\n";
        exit(1);
    }
    else
    {
        u16 n;
        while (PNB >> n)
        {
            *PNBcount = n;
        }
    }
    PNB.close();
    PNB.open(file);

    u16 *arr = new u16[*PNBcount], index{0};

    while (!PNB.eof())
    {
        PNB >> arr[index++];
    }
    PNB.close();

    return arr;
}

u16 **OpenFile(const char *file, size_t *blockCount, size_t *restCount)
{
    std::ifstream PNB(file);

    if (!PNB.is_open())
    {
        std::cout << "|          ⚠ Unable to open the file ⚠                                         " << std::setw(7) << "|\n";
        exit(1);
    }

    u16 last, second_last, current;
    while (PNB >> current)
    {
        second_last = last;
        last = current;
    }

    *blockCount = second_last;
    *restCount = last;

    u16 **arr = new u16 *[2];

    arr[0] = new u16[*blockCount];
    arr[1] = new u16[*restCount];

    PNB.close();
    PNB.open(file);

    for (int i{0}; i < *blockCount; ++i)
        PNB >> arr[0][i];
    for (int j{0}; j < *restCount; ++j)
        PNB >> arr[1][j];

    PNB.close();

    return arr;
}
