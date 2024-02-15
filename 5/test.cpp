#include <iostream>
#include <bitset>
#include <random> // random number generator

static inline double unitRandom()
{
    static std::default_random_engine generator(std::random_device{}());
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

// void printFormattedBits(uint32_t number)
// {
//     std::bitset<32> bits(number);
//     std::string bitsString = bits.to_string();

//     // Insert a gap after every 8 bits
//     for (size_t i = 8; i < bitsString.size(); i += 9)
//     {
//         bitsString.insert(i, " ");
//     }

//     std::cout << bitsString << std::endl;
// }

int main()
{
    // uint32_t myNumber = 0xABCD1234; // Replace this with your 32-bit number

    // printFormattedBits(myNumber);

    // return 0;

    for (int index{0}; index < 20;++index)
        std::cout << unitRandom() << "\n";
}
