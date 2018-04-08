#if 0
#include "grid_traversals.h"
#include <algorithm>

const int max_ = 10;
int arr[max_][max_];
int arr2[max_][max_];

typedef Wrap<max_> W;


//works as long as the spiral does not go out of grid
//the spiral goes out in # steps in X direction (#X): 1U, 1R, 2D, 2L, 3U, 3R so on...
template<typename T>
void spiral_out_naive(T w, P c, int s)
{
	assert(s > 0);
	int ro = c.ro, co = c.co;
	int k = 0;
	int counter = 0;
	int maxcount = s * s - 1;

	w(ro, co) = 0;

#define FOR(k) for (int i = 0; i < k && counter < maxcount; ++i) 

	while (counter < maxcount)
	{
		++k;
		FOR(k) w(--ro, co) = ++counter;
		FOR(k) w(ro, ++co) = ++counter;
		++k;
		FOR(k) w(++ro, co) = ++counter;
		FOR(k) w(ro, --co) = ++counter;
	}
}

void spiral_out(W w, P c, int s)
{
	P tl = { c.ro - (s - 1) / 2, c.co - (s - 1) / 2 };
	Box b = { tl , tl + s };
	b.crop(max_);

	int ro = c.ro, co = c.co;
	int n = 0;
	int counter = -1;
	int maxcount = s * s - 1;

#define DIVIDE_INTERVAL_FORWARD(x) divide_interval(x, x + n, b.tl.x, b.br.x, true)
#define DIVIDE_INTERVAL_REVERSE(x) divide_interval(x, x - n, b.br.x - 1, b.tl.x - 1, false)
//using the local variables (x, c) help in incrementing their counterparts at once after the loop without worrying about the loop range
#define FOR_RANGE for (int x = r.m1, c = counter + r.m1 - r.m0; x < r.m2 && c < maxcount; ++x) 
#define RFOR_RANGE for (int x = r.m1, c = counter + r.m0 - r.m1; x > r.m2 && c < maxcount; --x)

#define INSIDE_BOX(x)  (b.tl.x <= x && x < b.br.x)
	//move (ro, co) AND THEN set the cell counter. That is, (ro, co) is always valid == on the spiral
	Ranges r;
	while (counter < maxcount)
	{
		++n;
		if (INSIDE_BOX(co))
		{
			r = DIVIDE_INTERVAL_REVERSE(ro);
			RFOR_RANGE w(x, co) = ++c;
		}
		counter += n, ro -= n;

		if (INSIDE_BOX(ro))
		{
			r = DIVIDE_INTERVAL_FORWARD(co);
			FOR_RANGE w(ro, x) = ++c;
		}
		counter += n, co += n;

		++n;
		if (INSIDE_BOX(co))
		{
			r = DIVIDE_INTERVAL_FORWARD(ro);
			FOR_RANGE w(x, co) = ++c;
		}
		counter += n, ro += n;

		if (INSIDE_BOX(ro))
		{
			r = DIVIDE_INTERVAL_REVERSE(co);
			RFOR_RANGE w(ro, x) = ++c;
		}
		counter += n, co -= n;
	}
}

int spiral_value_at(P p, P center)
{
	int x = p.co - center.co;
	int y = -(p.ro - center.ro);

	if (x == 0 && y == 0) return 0; //redundant check

	int t = std::max(std::abs(x), std::abs(y));
	int s = 2 * t + 1;
	//p lies on the periphery of box of side s
	//x, y belong to [-t t]

	//left should come before top!
	//left 
	if(-t == x) return (s * s - 1) - (t - y);
	//bottom 
	if(-t == y) return (s - 1) * (s - 1) + (t - x);
	//right
	if(t == x) return (s - 1) * (s - 1) - (t + y);

	return (s - 2) * (s - 2) + (t + x - 1);
}


int main(int, char**)
{
#if 0
	W w; w.a = arr;
	WrapSilent<max_> w2; w2.a = arr2;
	for (int t = 0; t < 100000; ++t)
	{
		P center = { SRAND10, SRAND10 }; 
		int s = 2 * RAND10 + 1;
		spiral_out(w, center, s);
		spiral_out_naive(w2, center, s);
		check_equal(w, w2, max_);
	}
#elif 0
	W w; w.a = arr;
	std::cout << "Space key to begin, and arrow keys to move the box\n";
	P center = { 4, 4 };
	animate([&](int c) {
		system("cls");
		fill(w, 0, max_);
		update(c, center);
		spiral_out(w, center, 5);
		print(w, true);
	});
#elif 1
	W w; w.a = arr;
	P center = { 4, 4 };
	spiral_out(w, center, 11);
	
	for (int i = 0; i < max_; ++i)
		for (int j = 0; j < max_; ++j)
			assert(w(i, j) == spiral_value_at({ i, j }, center));
#endif
}
#endif