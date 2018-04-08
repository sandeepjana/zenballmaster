#if 0
#include "grid_traversals.h"

/*
Goal is to draw concentric square of odd numbered size around a point
+ Start at tl, and go till the last cell in that row, excluding the last cell.
+ In the next, loop start from the cell excluded above and go downwards. And so on
*/

const int max_ = 10;
int arr[max_][max_];
int arr2[max_][max_];

typedef Wrap<max_> W;

#define FOR(x) for(int x = b.tl.x; x < b.br.x; ++x)
#define RFOR(x) for(int x = b.br.x - 1; x >= b.tl.x; --x)

#define FOR_1SHORT(x) for(int x = b.tl.x; x < b.br.x && x < b_.br.x - 1; ++x)
#define RFOR_1SHORT(x) for(int x = b.br.x - 1; x >= b.tl.x && x > b_.tl.x; --x)

//works fine as long as the box does not go out of the grid
template<class T>
void one_square_naive(T w, P p, int s, int& counter)
{
	P tl = p + -(s - 1) / 2;
	P br = tl + s;

	if (s < 3) return;

#define NAIVE_FOR_1SH(x) for (int x = tl.x; x < br.x - 1; ++x)
#define NAIVE_RFOR_1SH(x) for(int x = br.x - 1; x > tl.x; --x)

	NAIVE_FOR_1SH(co) w(tl.ro, co) = counter++;
	NAIVE_FOR_1SH(ro) w(ro, br.co - 1) = counter++;
	NAIVE_RFOR_1SH(co) w(br.ro - 1, co) = counter++;
	NAIVE_RFOR_1SH(ro) w(ro, tl.co) = counter++;
}

//post condition: counter is incremented around the b_
void one_square(W w, P p, int s, int& counter)
{
	P tl = p + -(s - 1) / 2;
	Box b = { tl, tl + s };
	Box b_ = b;
	b.crop(max_);

	if (!(b.tl.ro < b.br.ro && b.tl.co < b.br.co))
	{
		counter += 4 * (s - 1);
		return;
	}

	Margins m = get_margins(b_, b);

	if (0 == m.top)
	{
		counter += m.left;
		int ro = b.tl.ro;
		FOR_1SHORT(co) w(ro, co) = counter++;
		counter += (m.right - 1) > 0 ? (m.right - 1) : 0;
	}
	else counter += s - 1;

	if (0 == m.right)
	{
		counter += m.top;
		int co = b.br.co - 1;
		FOR_1SHORT(ro) w(ro, co) = counter++;
		counter += (m.bottom - 1) > 0 ? (m.bottom - 1) : 0;
	} 
	else  counter += s - 1;

	if (0 == m.bottom)
	{
		counter += m.right;
		int ro = b.br.ro - 1;
		RFOR_1SHORT(co) w(ro, co) = counter++;
		counter += (m.left - 1) > 0 ? (m.left - 1) : 0;
	}
	else  counter += s - 1;

	if (0 == m.left)
	{
		counter += m.bottom;
		int co = b.tl.co;
		RFOR_1SHORT(ro) w(ro, co) = counter++;
		counter += (m.top - 1) > 0 ? (m.top - 1) : 0;
	}
	else  counter += s - 1;
}

void one_square_condensed(W w, P p, int s, int& counter)
{
	P tl = p + -(s - 1) / 2;
	Box b = { tl, tl + s };
	Box b_ = b;
	b.crop(max_);

	if (!(b.tl.ro < b.br.ro && b.tl.co < b.br.co))
	{
		counter += 4 * (s - 1);
		return;
	}

	Margins m = get_margins(b_, b);

#define FOR_1SHORT_V2(x, mar) for(int x = b.tl.x, c = counter + m.mar; x < b.br.x && x < b_.br.x - 1; ++x)
#define RFOR_1SHORT_V2(x, mar) for(int x = b.br.x - 1, c = counter + m.mar; x >= b.tl.x && x > b_.tl.x; --x)

	if (0 == m.top)
		FOR_1SHORT_V2(co, left)  w(b.tl.ro, co) = c++;
	counter += s - 1;

	if (0 == m.right)
		FOR_1SHORT_V2(ro, top)  w(ro, b.br.co - 1) = c++;
	counter += s - 1;

	if (0 == m.bottom)
		RFOR_1SHORT_V2(co, right) w(b.br.ro - 1, co) = c++;
	counter += s - 1;

	if (0 == m.left)
		RFOR_1SHORT_V2(ro, bottom) w(ro, b.tl.co) = c++;
	counter += s - 1;
}
void concentric_squares(W w, P p, int s)
{
	int counter = 11;
	for (int k = 3; k <= s; k += 2)
	{
		one_square_condensed(w, p, k, counter);
	}
}

void concentric_squares_test(WrapSilent<max_> w, P p, int s)
{
	int counter = 11;
	for (int k = 3; k <= s; k += 2)
	{
		one_square_naive(w, p, k, counter);
	}
}


int main(int, char**)
{
#if 0
	W w; w.a = arr;

	std::cout << "Space key to begin, and arrow keys to move the box" << nl;
	P pos = { 5, 5 };
	animate([&](int c) {
		system("cls");
		fill(w, 0, max_);
		update(c, pos);
		concentric_squares(w, pos, 7);
		print(w, true);
	});
#else
	W w; w.a = arr;
	WrapSilent<max_> w2; w2.a = arr2;
	for (int t = 0; t < 100000; ++t)
	{
		P pos = { SRAND10, SRAND10 };
		int s = 2 * RAND10 + 1;
		concentric_squares(w, pos, s);
		concentric_squares_test(w2, pos, s);
		check_equal(w, w2, max_);
	}

#endif
}
#endif