#if 0
#include <stdio.h>
#include <algorithm>
#include <array>
#include <cassert>
const int max_ = 10000;
std::array<int, max_> aa, bb, cc;

long long hoare = 0;
long long lomuto = 0;

//with hoare partition
//pivot does not end up in its final position after each iteration
//0. input arguments (array, length)
//1. base case (just one element)
//2. pivot choosing (mid element)
	//dont worry to place aside the pivot. in fact you must not for below code to work
//3. two pointers pointing both ends (indices)
//4. move them inwards as long as they are strictly less than and strictly greater than elements respectively
//dont worry that they go beyond the array bounds, they wont because the pivot is not placed aside
//5. swap the elements pointed by them (some equal elements may get swapped), if they did not cross (or became equal)
//6. increment left pointer and decrement right pointer. go to 4.
	// dont worry that the pointers collide or cross after the increment/decrement
//7. quicksort lower partiion and upper partition (no "three" partitions)
	// left partition and right partition both can contain elements equal to pivot
	// for e.g. think of case all the elements are equal
//This works well for sorted and all-equal element arrays, because we picked mid element as pivot
// Sorted -- no swaps when all are unique elements, and as usual n comparisons at each stage
// All identical -- swaps n/2 times at each stage, but since the partition is midway, still it is O(nlogn)
void quicksort(int* a, int len)
{
	assert(len < 2 ? 1 == len : true);
	if (len < 2) return;

	int i = 0, j = len - 1;
	int p = a[len / 2]; //pivot is chosen as the middle point 
	//because for sorted/reverse sorted arrays and arrays with same elements
	//this choice still leads to O(nlogn) comparisons
	while (1)
	{
		while (a[i] < p) { ++i; ++hoare; }
		while (a[j] > p) { --j; ++hoare; }
		if (!(i < j)) break; //at break, i == j or i = j + 1 depending on len being odd or even
		std::swap(a[i], a[j]);
		++i, --j;
	}

	assert(i == j || i == j + 1);

	//i & j crossed and [0 to j] [i=(j+1) len-1] are the arrays to recurse
	quicksort(a, i);
	quicksort(a + i, len - i);
}

//
void quicksort_good(int * a, int len)
{
	if (len < 2) return;
	int p = a[len / 2];
	int i = 0, j = len - 1;
	while (1)
	{
		while (a[i] < p) ++i;
		while (a[j] > p) ++j;
		if (!(i < j)) break;
		std::swap(a[i++], a[j--]);
	}
	quicksort_good(a, i);
	quicksort_good(a + i, len - i);
}

//becomes lopsided when all the elements are equal <-- so never use it!!!
void quicksort_lomuto(int* a, int len)
{
	if (len < 2) return;
	int p = a[len - 1];
	int i = 0;
	while (a[i] < p) ++i;
	for (int j = i + 1; j < len - 1; ++j)
	{
		++lomuto;
		if (a[j] < p) std::swap(a[i++], a[j]);
	}

	std::swap(a[i], a[len - 1]);
	quicksort_lomuto(a, i);
	quicksort_lomuto(a + i + 1, len - (i + 1));
}

void insertion_sort_v1(int* a, int len)
{
	for (int i = 1; i < len; ++i)
	{
		for (int j = i; j >= 1 && a[j] < a[j - 1]; --j)
			std::swap(a[j], a[j - 1]); //swap is overkill
	}
}
void insertion_sort(int* a, int len)
{
	for (int i = 1; i < len; ++i)
	{
		int j = i, key = a[i];
		for (; j >= 1 && key < a[j - 1]; --j)
			a[j] = a[j - 1];
		a[j] = key;
	}
}


int main(int, char**)
{
	std::generate(aa.begin(), aa.end(), []() { return rand() % 1000; });
	//std::fill(aa.begin(), aa.end(), 0);
	//std::sort(aa.begin(), aa.end());
	bb = aa;
	cc = aa;

	long long count = 0;
	std::sort(aa.begin(), aa.end(), [&count](int a, int b) {
		++count;
		return a < b;
	});

	quicksort(&bb[0], bb.size());
	//quicksort_lomuto(&cc[0], cc.size());
	insertion_sort(&cc[0], cc.size());
	assert(bb == aa);
	assert(cc == aa);

	auto sum = count + hoare + lomuto;

	printf("%lld %lld %lld\n", (count * 100) / sum, (hoare * 100) / sum, (lomuto * 100) / sum);

	return 0;
}
#endif