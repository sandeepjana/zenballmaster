#if 0
#include <iostream>
#include <cassert>
#include <algorithm>
#include <array>
#include <functional>

const int max_ = 5000;
int parentof(int c) { return (c % 2) ? (c - 1) / 2 : (c - 2) / 2; }

struct Maxheap {
	int a[max_];
	int n = 0;
	bool empty() { return 0 == n; }
	void reset() { n = 0; }
	int size() { return n; }
	
	//pre-condition: i < n (j and k can be greater)
	int max3_index(int i, int j, int k)
	{
		assert(i < n);
		int m = i;
		if (j < n && a[j] > a[m]) m = j;
		if (k < n && a[k] > a[m]) m = k;
		return m;
	}
	//put a[i] in right place in the heap rooted at array index i
	//(rest of the tree rooted at i satisfies heap property
	void siftdown(int i)
	{
		while (1)
		{
			int m = max3_index(i, 2 * i + 1, 2 * i + 2);
			if (m == i) return; //root is the biggest -- so fine!
			std::swap(a[m], a[i]);
			i = m;
		}
	}
	//siftup the last element - pre-condition: rest of the elements form a heap
	//it does not make sense to sift up some intermediate index
	//because we dont know if it belongs to above levels or below levels in the final heap
	void siftup()
	{
		int c = n - 1;
		int p = parentof(c);
		while (p >= 0 && a[p] < a[c])
		{
			std::swap(a[p], a[c]);
			c = p;
			p = parentof(c);
		}
	}
	void fill(int e)
	{
		a[n++] = e;
	}
	void make_heap()
	{
		for (int i = parentof(n - 1); i >= 0; --i)
			siftdown(i);
	}
	int top()
	{
		assert(n > 0);
		return a[0];
	}
	void pop()
	{
		--n;
		if (n < 1) return;
		a[0] = a[n];
		siftdown(0);
	}
	void push(int e)
	{
		a[++n] = e;
		siftup();
	}
};

Maxheap heap;


bool covered[max_];
void test_heap(int* a, int n)
{
	if (n < 2) return;
	std::fill(std::begin(covered), std::end(covered), false);

	int i = 0;
	for (; i < parentof(n - 1); ++i)
	{
		assert(a[i] >= a[2 * i + 1] && a[i] >= a[2 * i + 2]);
		covered[i] = covered[2 * i + 1] = covered[2 * i + 2] = true;
	}
		
	for (int c = 2 * i + 1; c < n; ++c)
	{
		assert(a[i] >= a[c]);
		covered[i] = covered[c] = true;
	}

	for (int i = 0; i < n; ++i)
		assert(covered[i]);
	for (int i = n; i < max_; ++i)
		assert(!covered[i]);
#if 0 		
	for (int i = 0; i < n; ++i)
	{
		int c = 2 * i + 1;
		if (!(c < n)) return;
		assert(a[i] >= a[c]);
		++c;
		if (!(c < n)) return;
		assert(a[i] >= a[c]);
	}
#endif
}

std::array<int, max_> order;


int main(int, char**)
{
	int T = 100;
	for (int t = 0; t < T; ++t)
	{
		heap.reset();

		int n = rand() % max_;
		std::cout << t << " " << n << '\n';

		for (int i = 0; i < n; i++)
			heap.fill(rand() % 100);

		assert(n == heap.n);
		heap.make_heap();
		test_heap(heap.a, heap.n);

		while (heap.n < max_ - 1)
		{
			heap.push(rand() % 100);
			test_heap(heap.a, heap.n);
		}

		int o = 0;
		while (heap.n > 0)
		{
			order[o++] = heap.top();
			heap.pop();
			test_heap(heap.a, heap.n);
		}

		std::is_sorted(begin(order), end(order), std::greater<int>());
	}
}
#endif