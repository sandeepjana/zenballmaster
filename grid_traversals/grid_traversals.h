#pragma once
#include <iostream>
#include <iomanip>
#include <conio.h>
#include <cassert>

const char sp = ' ';
const char nl = '\n';

#define SRAND10 (rand() % 20 - 10)
#define RAND10 (rand()%10)

enum LOC {
	TL = 0, TR, BL, BR, CTR //floored center for even sides
};

enum DIR {
	LR = 0, RL, TB, BT
};

struct P {
	int ro, co;
};

static P operator+(P p, int i)
{
	p.ro += i, p.co += i;
	return p;
}

//[lo, hi]
static int clamp(int x, int lo, int hi)
{
	if (x < lo) x = lo;
	if (x > hi) x = hi;
	return x;
}

struct Box {
	P tl, br; //[tl, br)
	void crop_(int& a, int& b, int size) {
		if (a < 0) a = 0;
		if (b > size) b = size;
		if (b < a) b = a;
	}
	void crop(int size) {
		crop_(tl.ro, br.ro, size);
		crop_(tl.co, br.co, size);
	}
	int width() const { return br.co - tl.co; }
	int height() const { return br.ro - tl.ro; }
};

struct Margins
{
	int top, left, right, bottom;
};

template<int size, typename T = int>
struct Wrap {
	T(*a)[size];
	T& operator()(int y, int x) {
		assert(0 <= y && y < size);
		assert(0 <= x && x < size);
		return *(&a[0][0] + y*size + x);
	}
	T& operator()(P p)
	{
		return operator()(p.ro, p.co);
	}
};

template<int size>
struct WrapSilent {
	int(*a)[size];
	const int oob = 0xBADC0DE;
	int dummy;
	int& operator()(int y, int x) {
		if (!(0 <= y && y < size)) return (dummy = oob);
		if (!(0 <= x && x < size)) return (dummy = oob);
		return *(&a[0][0] + y*size + x);
	}
	int& operator()(P p)
	{
		return operator()(p.ro, p.co);
	}
};

//b1 - b2
static Margins get_margins(const Box& outer, const Box& inner)
{
	Margins m;
	m.top = inner.tl.ro - outer.tl.ro;
	m.left = inner.tl.co - outer.tl.co;
	m.bottom = outer.br.ro - inner.br.ro;
	m.right = outer.br.co - inner.br.co;
	assert(m.top + m.bottom == outer.height() - inner.height());
	assert(m.left + m.right == outer.width() - inner.width());
	return m;
}

template<class T>
void print(T w, int size, bool indices = false)
{
	using namespace std;
	if (indices)
	{
		cout << setw(4) << sp;
		for (int x = 0; x < size; x++)
			cout << setw(4) << x;
		cout << nl;
	}

	for (int y = 0; y < size; y++)
	{
		if (indices) cout << setw(4) << y;
		for (int x = 0; x < size; x++)
			cout << setw(4) << w(y, x);
		cout << nl;
	}

}

template<int size>
void print(Wrap<size> w, bool indices = false)
{
	print(w, size, indices);
}

//https://stackoverflow.com/questions/24708700/c-detect-when-user-presses-arrow-key
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

template <class T>
void animate(T fun)
{
	while (1)
	{
		int c = _getch();
		if (224 == c) continue; //arrow keys have a start code
		fun(c);
	}
}

static void update(int c, P& p)
{
	if (KEY_UP == c) --p.ro;
	else if (KEY_DOWN == c) ++p.ro;
	else if (KEY_LEFT == c) --p.co;
	else if (KEY_RIGHT == c) ++p.co;
}

template<class T>
void fill(T w, int val, int size)
{
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			w(i, j) = val;
}

template<typename T, typename U>
static void check_equal(T w1, U w2, int size)
{
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			assert(w1(i, j) == w2(i, j));
}

struct Ranges {
	int m0, m1, m2, m3; //[m0, m1) [m1, m2) [m2, m3)
};

//refer to the image
//divide [a0, a1) wrt [w0, w1) into pre, main and post ranges.
//forward: a0 <= a1 and w0 <= w1. [m0 = a0 m1) [m1  m2) [m2 m3 = a1) are the three ranges
//reverse: a0 >= a1 and w0 >= w1, so the input ranges are (a1 a0], (w1 w0] and output ranges are
//(m1 m0 = a0], (m2 m1], (m3 = a1 m2]
static Ranges divide_interval(int a0, int a1, int w0, int w1, int forward = true)
{
	if (false == forward) a0 = -a0, a1 = -a1, w0 = -w0, w1 = -w1;
	assert(a0 <= a1 && w0 <= w1);

#define COMPARE(x, a0, a1) (x < a0) ? a0 : (x < a1) ? x : a1
	Ranges r;
	r.m0 = a0; r.m3 = a1;
	r.m1 = COMPARE(w0, a0, a1);
	r.m2 = COMPARE(w1, a0, a1);

	if (false == forward) r.m0 = -r.m0, r.m1 = -r.m1, r.m2 = -r.m2, r.m3 = -r.m3;
	return r;
}


