#if 0
#include <algorithm>
#include <iostream>

static uint32_t x = 0xBADC0DE;
uint32_t xorshift32()
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 15;
	return x;
}

void variance(int* a, int N)
{
	long long sum = 0;
	for (int i = 0; i < N; i++)
		sum += a[i];
	
	int mean = sum / N;
	std::cout << "mean " << mean << '\n';

	for (int i = 0; i < N; i++)
		sum += ((a[i] - mean) * (a[i] - mean));

	int var = sum / N;
	std::cout << "sd " << std::sqrt(var) << '\n';
}

const int N = 1024;
int a[N] = { 0 };

int main(int, char**)
{
	for (int i = 0; i < 1000'000; ++i)
		++a[xorshift32() % N];

	std::cout << *std::min_element(a, a + N) << " " <<
		*std::max_element(a, a + N) << '\n';

	variance(a, N);
	return 0;
}
#endif