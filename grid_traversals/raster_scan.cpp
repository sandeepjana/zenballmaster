#if 0
#include "grid_traversals.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <conio.h> //getch
#include <stdlib.h> //system()
#include <chrono>
using namespace std;

/*
Goal: find the common mistakes. Reduce the complexity of code.
+ Good to go with start point and end point pair, rather than start point and size of the box
+ Given any LOC, find TL and then proceed.
+ Wrap the array, and overload operator[]. Datatype of array is not easy to remember but intellisense helps
+ Once you compute the margins, many computations simplify
*/

const int max_ = 10;
int arr[max_][max_];
int arr2[max_][max_];

typedef Wrap<max_> W;

const char sp = ' ';
const char nl = '\n';

//probably you can make these methods of Box
#define ATTOP		(b.tl.ro == ro)
#define ATBOTTOM	(b.br.ro - 1 == ro)
#define ATLEFT		(b.tl.co == co)
#define ATRIGHT		(b.br.co - 1 == co)

#define PASTTOP		(b.tl.ro > ro)
#define PASTBOT		(b.br.ro == ro)
#define PASTLEFT	(b.tl.co > co)
#define PASTRIGHT	(b.br.co == co)

//#define FOR(x, s, e, extra) for(int x = s.x; x < e.x; ++x, extra)
//#define RFOR(x, s, e, extra) for(int x = e.x - 1; x >= s.x; --x, extra)
#define FOR2(s, e) for (int ro = s.ro; ro < e.ro; ro++) for (int co = s.co; co < e.co; co++)

//enum Dir {
//	LR = 0x01, RL = 0x02, TB = 0x04, BT = 0x08
//};

struct Fit {
	int x;
	int operator*() { return x; }
	void operator++() { ++x; }
	bool operator!=(Fit& a) { return a.x != x; }
};
struct Rit {
	int x;
	int operator*() { return x; }
	void operator++() { --x; }
	bool operator!=(Rit& a) { return a.x != x; }
};
struct FitPair { Fit b, e; };
struct RitPair { Rit rb, re; };

//T outer, U inner
template<class T, class U>
void paint(T a, T b, U c, U d, int counter, int skip, int map[], W w)
{
	for (T i = a; i != b; ++i, counter += skip)
		for (U j = c; j != d; ++j)
		{
			int uv[] = { i.x, j.x };
			w(uv[map[0]], uv[map[1]]) = ++counter;
		}

}

struct Range
{
	int from, to;
	bool rev;
	int i;
};

template<int SIZE>
struct Looper
{
	Range ranges[SIZE];
	int map[SIZE]; //map[0] points to the range which corresponds to rows, 1 to columns
	int skip[SIZE];
	int counter;
	W w;

	void operator()(int depth)
	{
		Range& range = ranges[depth];
		if (SIZE == depth)
		{
			int ro = ranges[map[0]].i;
			int co = ranges[map[1]].i;
			w(ro, co) = counter;
			return;
		}

		if (range.rev)
		{
			for (range.i = range.to - 1; range.i >= range.from; --range.i, counter += skip[depth])
				(*this)(depth + 1);
		}
		else
		{
			for (range.i = range.from; range.i < range.to; ++range.i, counter += skip[depth])
				(*this)(depth + 1);
		}
	}
};

void raster_scan_v2(W w, P p, int s, LOC loc, DIR fast, DIR slow)
{
	P tl;
	switch (loc)
	{
	case TL: tl = p; break;
	case BR: tl = p + -(s - 1); break;
	case TR: tl = { p.ro, p.co - s + 1 }; break;
	case BL: tl = { p.ro - s + 1, p.co }; break;
	case CTR: tl = p + -(s - 1) / 2; break; //center index = (s - 1)/2
	}

	Box b = { tl, tl + s };
	Box b_ = b;
	b.crop(max_);

	Margins m = get_margins(b_, b);

	auto off = [&m](DIR d) { return (d == TB) ? m.top :
		(d == LR) ? m.left :
		(d == RL) ? m.right : m.bottom; };

#define FWD(x) Range{b.tl.x, b.br.x, false, 0 /* dummy*/}
#define REV(x) Range{b.tl.x, b.br.x, true, 0 /* dummy*/}

	auto rng = [&b](DIR d) { return (LR == d) ? FWD(co) :
		(RL == d) ? REV(co) :
		(TB == d) ? FWD(ro) : REV(ro); };

	Looper<2> loop;
	loop.w = w;
	loop.skip[0] = (slow == TB || slow == BT) ? m.left + m.right : m.top + m.bottom;
	loop.skip[1] = 1;
	loop.counter = off(slow) * s + off(fast);
	++loop.counter; //just to start with 1
	loop.ranges[0] = rng(slow);
	loop.ranges[1] = rng(fast);
	loop.map[0] = (slow == TB || slow == BT) ? 0 : 1;
	loop.map[1] = (0 == loop.map[0]) ? 1 : 0;
	loop(0);

	return;
}

// TL (ro, co) with side s colors sxs cells.
// The cells in row with index (ro + s) are not colored, but the rows before are.
void raster_scan(W w, P p, int s, LOC loc, DIR fast, DIR slow)
{
	P tl;
	switch (loc)
	{
	case TL: tl = p; break;
	case BR: tl = p + -(s - 1); break;
	case TR: tl = { p.ro, p.co - s + 1 }; break;
	case BL: tl = { p.ro - s + 1, p.co }; break;
	case CTR: tl = p + -(s - 1) / 2; break; //center index = (s - 1)/2
	}

	Box b = { tl, tl + s };
	Box b_ = b;
	b.crop(max_);

	Margins m = get_margins(b_, b);

	auto off = [&m](DIR d) { return (d == TB) ? m.top :
		(d == LR) ? m.left :
		(d == RL) ? m.right : m.bottom; };

	int counter = off(slow) * s + off(fast);
	int skip = (slow == TB || slow == BT) ? m.left + m.right : m.top + m.bottom;

#define FOR(x, extra) for(int x = b.tl.x; x < b.br.x; ++x, extra)
#define RFOR(x, extra) for(int x = b.br.x - 1; x >= b.tl.x; --x, extra)

	if (TB == slow)
	{
		FOR(ro, counter += skip)
			if (LR == fast)
				FOR(co, void()) w(ro, co) = ++counter;
			else
				RFOR(co, void()) w(ro, co) = ++counter;
	}
	else if (BT == slow)
	{
		RFOR(ro, counter += skip)
			if (LR == fast)
				FOR(co, void()) w(ro, co) = ++counter;
			else
				RFOR(co, void()) w(ro, co) = ++counter;
	}
	else if (LR == slow)
	{
		FOR(co, counter += skip)
			if (TB == fast)
				FOR(ro, void()) w(ro, co) = ++counter;
			else
				RFOR(ro, void()) w(ro, co) = ++counter;
	}
	else if (RL == slow)
	{
		RFOR(co, counter += skip)
			if (TB == fast)
				FOR(ro, void()) w(ro, co) = ++counter;
			else
				RFOR(ro, void()) w(ro, co) = ++counter;
	}

}



int main(int, char**)
{
	W w; w.a = arr;

#if 1
	std::cout << "Space key to begin, and arrow keys to move the box" << nl;
	P pos = { 0, 0 };
	animate([&](int c) {
		system("cls");
		fill(w, 0, max_);
		update(c, pos);
		raster_scan(w, pos, 5, CTR, BT, LR);
		print(w, true);
	});
#else
	using namespace std::chrono;
	long long time = 0;
	Wrap<int> w2; w2.a = arr2;
	auto st = steady_clock::now();
	for (int t = 0; t < 1000000; ++t)
	{
		P pos = { -max_ + (rand() % max_) * 2, -max_ + (rand() % max_) * 2 };
		//fill(w, 0); fill(w2, 0);

		int s = rand() % 20;
		int loc = rand() % 5;
		int fast = rand() % 2;
		int slow = 2 + rand() % 2;
		if (rand() % 2) std::swap(fast, slow);

		raster_scan(w, pos, s, LOC(loc), DIR(fast), DIR(slow));
		raster_scan_v2(w2, pos, s, LOC(loc), DIR(fast), DIR(slow)); //x1.5 slower
		check_equal(w, w2, s);
	}
	auto et = steady_clock::now();
	time += duration_cast<milliseconds>(et - st).count();
	cout << time << "ms\n";
#endif

	//raster_scan(w, P{ 9, 9 }, 4, BR);
	//raster_scan(w, P{ -2, 8 }, 5, TR); 
	//raster_scan(w, P{ 0, -4}, 5, BL);
	//raster_scan(w, P{ 4, 4 }, 8, CTR);

}
#endif