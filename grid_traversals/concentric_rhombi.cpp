#if 0
#include "grid_traversals.h"
#include <algorithm>

const int max_ = 10;
int arr[max_][max_];
int arr2[max_][max_];

typedef Wrap<max_> W;


//works fine as long as the box does not go out of the grid
template<class T>
void one_rhombus_naive(T w, P c, int s, int& counter)
{
	if (0 == s)
		return void(w(c.ro, c.co) = ++counter);

	int y = s;
	int x = 0;

	for (; y != 0; --y, ++x) w(c.ro - y, c.co + x) = ++counter;
	for (; x != 0; --y, --x) w(c.ro - y, c.co + x) = ++counter;
	for (; y != 0; ++y, --x) w(c.ro - y, c.co + x) = ++counter;
	for (; x != 0; ++y, ++x) w(c.ro - y, c.co + x) = ++counter;
}

//[a, b)
struct Range { int a, b; };
Range intersect_ranges(Range r1, Range r2)
{
	bool forward = r1.a <= r1.b;
	assert(forward ? r2.a <= r2.b : r2.a >= r2.b);
	if (forward) return Range{ std::max(r1.a, r2.a), std::min(r1.b, r2.b) };
	return Range{ std::min(r1.a, r2.a), std::max(r1.b, r2.b) };
}

void one_rhombus(W w, P c, int s, int& counter)
{
	using namespace std;
#define INRANGE(u) (0 <= u && u < max_)
	if (0 == s) {
		if (INRANGE(c.ro) && INRANGE(c.co)) w(c.ro, c.co) = ++counter;
		else ++counter;
		return;
	}

	int gx0 = 0 - c.co, gy0 = -(0 - c.ro);
	int gx1 = max_ - c.co, gy1 = -(max_ - c.ro);

	Ranges mx, my, m;
	{	//x + y = s
		mx = divide_interval(0, s, gx0, gx1, true);
		my = divide_interval(s, 0, gy0, gy1, false);
		//find intersection
		m = divide_interval(mx.m1, mx.m2, s - my.m1, s - my.m2, true);
		for(int cnt = counter + m.m1, x = m.m1, y = s - x; x < m.m2; ++x, --y)
			w(c.ro - y, c.co + x) = ++cnt;
		counter += s;
	}
	{	//x - y = s
		mx = divide_interval(s, 0, gx1 - 1, gx0 - 1, false);
		my = divide_interval(0, -s, gy0, gy1, false);
		m = divide_interval(mx.m1, mx.m2, s + my.m1, s + my.m2, false);
		for (int cnt = counter + s - m.m1, x = m.m1, y = x - s; x > m.m2; --x, --y)
			w(c.ro - y, c.co + x) = ++cnt;
		counter += s;
	}
	{	//x + y = -s
		mx = divide_interval(0, -s, gx1 - 1, gx0 - 1, false); //(m1 m2]
		my = divide_interval(-s, 0, gy1 + 1, gy0 + 1, true); //[m1 m2)
		m = divide_interval(mx.m1, mx.m2, -s - my.m1, -s - my.m2, false); //(m1 m2]
		for (int cnt = counter + abs(m.m1), x = m.m1, y = -s - x; x > m.m2; --x, ++y)
			w(c.ro - y, c.co + x) = ++cnt;
		counter += s;
	}
	{	//x - y = -s
		mx = divide_interval(-s, 0, gx0, gx1, true);
		my = divide_interval(0, s, gy1 + 1, gy0 + 1, true);
		m = divide_interval(mx.m1, mx.m2, -s + my.m1, -s + my.m2, true);
		for (int cnt = counter + s - abs(m.m1), x = m.m1, y = x + s; x < m.m2; ++x, ++y)
			w(c.ro - y, c.co + x) = ++cnt;
		counter += s;
	}
}

void concentric_rhombi(W w, P c, int s)
{
	int counter = 10;
	for (int k = 0; k <= s; ++k)
	{
		one_rhombus(w, c, k, counter);
	}
}

void concentric_rhombi_test(WrapSilent<max_> w, P p, int s)
{
	int counter = 10;
	for (int k = 0; k <= s; ++k)
	{
		one_rhombus_naive(w, p, k, counter);
	}
}


int main(int, char**)
{
	W w; w.a = arr;
#if 0
	P center = { 4, 4 };
	int counter = 10;
	concentric_rhombi(w, center, 4);
	print(w, true);
#elif 1
	WrapSilent<max_> w2; w2.a = arr2;
	for (int t = 0; t < 10000; ++t)
	{
		P center = { SRAND10, SRAND10 };
		int s = RAND10;
		fill(w, 0, max_);
		fill(w2, 0, max_);
		concentric_rhombi(w, center, s);
		concentric_rhombi_test(w2, center, s);
		check_equal(w, w2, max_);
	}
#endif
}
#endif