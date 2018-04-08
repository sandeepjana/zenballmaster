#if 0
#include <iostream>
#include <algorithm>
#include <list>
#include <forward_list>
#include <array>
#include <chrono>
#include <time.h>

using namespace std::chrono;

#define START(id) auto start##id = steady_clock::now();
#define END(id) duration_cast<milliseconds>(steady_clock::now() - start##id).count()

const int amax = 1'000'000;
const int lmax = 1'000'000;

typedef int T;
std::forward_list<T> list;
T array[amax];

void create_list(const int size)
{
	//generate_n(back_inserter(list), size, []() { return 0; });
	generate_n(front_inserter(list), size, []() { return 0; });
}
void create_array(const int size)
{
	std::generate_n(array, size, []() { return 0; });
}
T traverse_list(const int size)
{
	int len = rand() % size;
	auto it = list.begin();
	std::advance(it, len);
	return *it;
}
long long traverse_array(const int size)
{
	int len = rand() % size;
	long long dummy = 0;
	for (int i = 0; i < len; ++i)
		dummy += array[i];
	return dummy;
}
void insert_array(const int size)
{
	int pos = rand() % size;
	int i = size - 2;
	for (; i >= pos; --i)
		array[i + 1] = array[i];
	array[pos] = rand() % 100;
}
void insert_list(const int size)
{
	int pos = rand() % size;
	auto it = list.begin();
	std::advance(it, pos);
	//list.insert(it, rand() % 100);
	list.insert_after(it, rand() % 100);
}

void measure_creation_times()
{
	long long t = 0;
	int numiter = 100;
	for (int size = 1000; size <= lmax; size *= 10)
	{
		std::cout << size << "  ";
		for (int i = 0; i < numiter; ++i)
		{
			START(0);
			create_list(size);
			t += END(0);
			list.clear();
		}
		std::cout << t << "ms\n";

		START(1);
		for (int i = 0; i < numiter; ++i)
		{
			create_array(size);
		}
		t = END(1);
		std::cout << t << "ms\n";
	}
}

void measure_traversal_times()
{
	long long t = 0, dummy = 0;
	int numiter = 1000;
	create_list(lmax);
	for (int size = 1000; size <= lmax; size *= 10)
	{
		std::cout << size << "  ";
		START(0);
		for (int i = 0; i < numiter; ++i)
		{
			dummy += traverse_list(size);
		}
		t = END(0);
		std::cout << t << " ";

		START(1);
		for (int i = 0; i < numiter; ++i)
		{
			dummy += traverse_array(size);
		}
		t = END(1);
		std::cout << t << "\n";
	}
	std::cout << dummy;
}

void measure_insertion_times()
{
	long long t = 0;
	int numiter = 1'000;

	for (int size = 1000; size <= lmax; size *= 10)
	{
		list.clear();
		create_list(size);
		std::cout << size << "  ";
		START(0);
		for (int i = 0; i < numiter; ++i)
		{
			insert_list(size);
		}
		t = END(0);
		std::cout << t << "  ";

		START(1);
		for (int i = 0; i < numiter; ++i)
		{
			insert_array(size);
		}
		t = END(1);
		std::cout << t << "\n";
	}
}

int main(int, char**)
{
	srand(time(NULL));

	//measure_creation_times();

	//measure_traversal_times();

	measure_insertion_times();

	return 0;
}
#endif