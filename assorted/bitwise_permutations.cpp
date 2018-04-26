#if 0
#include <iostream>
#include <stdint.h>
#include <string>
#include <cassert>

//Expects the size of the bit-string to be strictly less than 16
//so that the bit left to MSB of bit-string can be used to detect 
//end of the sequence of permutations
//Returns 0 when all permutations are over
uint16_t next_permutation(uint16_t x, int bit_length)
{
	assert(bit_length <= 15);

    if (0 == x)
        return 0;

    int n = 0;
    //find (smallest) n such that (x >> n) has 1 in the LSB
    for (; 0 == (x & 0x01); ++n, x >>= 1) {}
    
    uint16_t y = x + 1;
    uint16_t z = (y << n);

    if (z & (1 << bit_length)) //ones are overflown
        return 0; //reached end of permuations

    return z | ((x ^ y) >> 2);
}

void print_binary(uint16_t x)
{
    std::string s; //lousy -- use putchar
    uint16_t mask = 0x8000;
    for (int i = 0; i < 16; ++i, x <<= 1)
        s += ((x & mask) ? "1" : "0"); //pick msb

    std::cout << s << std::endl;
}

int bit_counter[256];
void init_bit_counter()
{
	bit_counter[0] = 0;
	for (int i = 1; i < 256; ++i)
		bit_counter[i] = bit_counter[i >> 1] + (i & 1);
}
int bit_count(uint16_t x)
{
	return bit_counter[x >> 8] + bit_counter[x & 0xFF];
}

void test_permutations() 
{
	init_bit_counter();

	const int bit_length = 8;
	const int num_ones = 4;
	uint16_t x = 0x0F; //the smallest number with num_ones ones
	for (uint16_t n = x; n <= 0xF0; ++n)
	{
		if (bit_count(n) == num_ones)
		{
			assert(n == x);
			x = next_permutation(x, bit_length);
		}
	}
	assert(0 == x);
}

int main(int, char**)
{
	test_permutations();
	return 0;

    int numones = 0;
    for (uint16_t power2 = 1; numones < 4; power2 <<= 1, ++numones)
    {
        uint16_t x = power2 - 1;
        while (x)
        {
			print_binary(x);
			x = next_permutation(x, 8);
        }
		printf("----------\n");
    }
    
    return 0;
}
#endif