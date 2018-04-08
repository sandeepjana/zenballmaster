#if 0
#include <iostream>
#include <stdint.h>
#include <string>

uint16_t next_permutation(uint16_t x)
{
    if (0 == x)
        return 0;

    int n = 0;
    //find (smallest) n such that (x >> n) has 1 in the LSB
    for (; 0 == (x & 0x01); ++n, x >>= 1) {}
    
    uint16_t y = x + 1;
    uint16_t z = (y << n);

    if (0 == z) //no most significant bits
        return 0; //reached end of permuations

    return z | ((x ^ y) >> 2);
}

void print_binary(uint16_t x)
{
    std::string s;
    uint16_t mask = 0x8000;
    for (int i = 0; i < 16; ++i, x <<= 1)
        s += ((x & mask) ? "1" : "0"); //pick msb

    std::cout << s << std::endl;
}


int main(int, char**)
{
    int numones = 0;
    for (uint16_t power2 = 1; numones < 9; power2 <<= 1, ++numones)
    {
        uint16_t x = power2 - 1;
        while (x)
        {
            x = next_permutation(x);
        }
    }
    
    return 0;
}
#endif