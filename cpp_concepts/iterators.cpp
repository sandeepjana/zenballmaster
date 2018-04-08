#if 0
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <cassert>
#include <deque>
#include <string>
using namespace std;

//sum of integers in a container
template <typename Iter>
int my_accumulate(Iter first, Iter last)
{
	int sum = 0;
	while(first != last)
	{
		sum += *first;
		first++;
	}
	return sum;
}

template <typename Iter>
void foo(Iter in)
{
	std::vector<typename std::iterator_traits<Iter>::value_type> v(1);
	auto out = v.begin();
	*out = *in;
	cout << *out << endl;
}

int main()
{
	vector<int> v(10);
	generate(v.begin(), v.end(), [](){ return rand()%99; });
	vector<string> d(10);

	auto iter = v.begin();
	cout << *iter << endl;
	*iter = 5;
	cout << *iter << endl;

	assert(my_accumulate(v.begin(), v.end()) == std::accumulate(v.begin(), v.end(), 0));

	foo(v.begin());
	foo(d.begin());
}
#endif