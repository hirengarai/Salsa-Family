#include <iostream>
#include <string>

struct DetailsParams
{
    const char *cipher, *progtype;
    u16 totalround, *IDword, *IDbit, *ODword, *ODbit, fwdround;
    const char *PNBmode, PNBvaluemode;
    ull totalSamples, samplesPerloop;

    // Constructor with default values
    DetailsParams(const char *cipher, *progtype,
                  u16 totalround, *IDword, *IDbit, *ODword, *ODbit, fwdround,
                  const char *PNBmode, PNBvaluemode,
                  ull totalSamples, samplesPerloop): cipher(cipher), progtype(progtype),totalround(totalround), IDword(IDword),IDbit(IDbit),ODword(ODword), ODbit(ODbit), fwdround(fwdround), PNBmode(PNBmode), PNBvaluemode(PNBvaluemode), totalSamples(totalSamples),samplesPerloop(samplesPerloop)
    {    }
};

// void myFunction(const PutValueParams &params)
// {

// }

class PUT_VALUE
{
public:
    int WORD, BIT;
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, count is the total element number in y
    void salsa(PutValueParams &params)
    {
        if (!(params.mode))
        {
            if (!(params.valuemode))
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    if ((params.y[i] / 32) > 3)
                    {
                        WORD = (params.y[i] / 32) + 7;
                    }
                    else
                    {
                        WORD = (params.y[i] / 32) + 1;
                    }
                    BIT = params.y[i] % 32;

                    if (drand48() > 0.5)
                    {
                        params.x[WORD] ^= (0x1 << BIT);
                        params.x1[WORD] ^= (0x1 << BIT);
                    }
                }
            }
            else
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    if ((params.y[i] / 32) > 3)
                    {
                        WORD = (params.y[i] / 32) + 7;
                    }
                    else
                    {
                        WORD = (params.y[i] / 32) + 1;
                    }
                    BIT = params.y[i] % 32;

                    if (drand48() > 0.5)
                    {
                        params.x[WORD] &= ~(0x1 << BIT);
                        params.x1[WORD] &= ~(0x1 << BIT);
                    }
                }
            }
        }
    }
    // accroding to str, random or any specific values are put to the bits of x and x1, which is listed in y, count is the total element number in y
    void chacha(PutValueParams &params)
    {
        if (!(params.mode))
        {
            if (!(params.valuemode)) // random values
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    WORD = (params.y[i] / 32) + 4;
                    BIT = params.y[i] % 32;

                    if (drand48() > 0.5)
                    {
                        params.x[WORD] ^= (0x1 << BIT);
                        params.x1[WORD] ^= (0x1 << BIT);
                    }
                }
            }
            else
            {
                for (int i{0}; i < params.ysize; ++i)
                {
                    WORD = (params.y[i] / 32) + 4;
                    BIT = params.y[i] % 32;

                    params.x[WORD] &= ~(0x1 << BIT);
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
                    params.x1[WORD] |= (0x1 << BIT);
                }
                else
                {
                    params.x[WORD] &= ~(0x1 << BIT);
                    params.x1[WORD] &= ~(0x1 << BIT);
                }
            }
            for (int i{0}; i < params.list2size; ++i)
            {
                WORD = (params.list2[i] / 32) + 4;
                BIT = params.list2[i] % 32;

                if (drand48() > 0.5)
                {
                    params.x[WORD] ^= (0x1 << BIT);
                    params.x1[WORD] ^= (0x1 << BIT);
                }
            }
        }
    }
};

int main()
{
    // Call the function with named parameters using constructor
    PutValueParams params;
    params.str = "Hello";
    params.y = 5;
    myFunction(params);

    return 0;
}
