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
Zigzag: (from (0, 0) it goes down to (1, 0))
+ Number of diagonals = 2 * (size - 1) + 1 = 2s - 1 = upper diagonals + lower diagonals + one main diagonal
+ Index of main diagonal = s - 1 (of course)
+ Number of elements in i-th diagonal = (i + 1) (e.g. 0-th diagonal has one element), if (i <= s - 1)
= (2s - 1) - i, if (s - 1) <= i < (2s - 1)
Check: number of elements on main diagonal --> i + 1 == (2s - 1) - i for i = (s - 1)
+ Triangular sum code below -- basically depends on if d <= (s - 1) or not, and if d is even or odd.
+ During zigzag scan, you need to handle five cases of (ro, co)
top left corner, at top, at left, at right, at bottom
+ Once you decide the direction of movement from top left corner, that decides how the rest of the traversal goes
+
*/

const int max_ = 10;
int arr[max_][max_];
int arr2[max_][max_];

typedef Wrap<max_> W;

//probably you can make these methods of Box
#define ATTOP		(b.tl.ro == ro)
#define ATBOTTOM	(b.br.ro - 1 == ro)
#define ATLEFT		(b.tl.co == co)
#define ATRIGHT		(b.br.co - 1 == co)

#define PASTTOP		(b.tl.ro > ro)
#define PASTBOT		(b.br.ro == ro)
#define PASTLEFT	(b.tl.co > co)
#define PASTRIGHT	(b.br.co == co)

void zigzag_scan_a1(W w, P p, int s)
{
	Box b = { p, p + s };
	b.crop(max_);

	int counter = 10;
	int ro = b.tl.ro, co = b.tl.co;

	if (ro < b.br.ro && co < b.br.co)
		w(ro, co) = ++counter;

	while (1)
	{
		if (b.tl.ro == ro && co + 1 < b.br.co) //top
		{
			co += 2; --ro;
			do { w(++ro, --co) = ++counter; } while (co > b.tl.co);
		}
		else if (b.tl.co == co && ro + 1 < b.br.ro) //left 
		{
			ro += 2; --co;
			do { w(--ro, ++co) = ++counter; } while (ro > b.tl.ro);
		}
		else if (b.br.ro - 1 == ro && co + 1 < b.br.co) //bottom
		{
			++ro;
			do { w(--ro, ++co) = ++counter; } while (co < b.br.co - 1);
		}
		else if (b.br.co - 1 == co && ro + 1 < b.br.ro) //right
		{
			++co;
			do { w(++ro, --co) = ++counter; } while (ro < b.br.ro - 1);
		}
		else
			break;
	}
}

//including n
int triangle_sum(int n, int m)
{
	//[1 to n]
	auto forward_sum = [](int n) -> int { return (n + 1) * n / 2; };
	return (n <= m) ? forward_sum(n) : (m * m) - forward_sum(2 * m - (n + 1));
#ifdef _DEBUG
	int s1 = forward_sum(m);
	int s2 = m * m - forward_sum(2 * m - (m + 1));
	assert(s1 == s2);
#endif
}

//returns 1 for (0, 0)
//and moves down from origin, as a convention
int get_zigzag_offset(int ro, int co, int size)
{
	int diag = ro + co;
	int off = triangle_sum(diag - 1 + 1, size);
	if (diag <= (size - 1))
		off += (diag % 2 == 0) ? ro + 1 : co + 1;
	else
		off += (diag % 2 == 0) ? (size - co) : (size - ro);

	return off;
}

//precondition: you start at either bottom or left
//post condition: you end up at either top or right
void move_north_east_and_park(W w, const Box& b, int& ro, int& co, int &counter)
{
	for (; !PASTTOP && !PASTRIGHT; --ro, ++co)
		w(ro, co) = ++counter;
	++ro; --co; //bring back into box

				//when you go NE, you hit either top or right or both (TR corner).
				//In last two cases, you should move downwards
	ATRIGHT ? ++ro : ++co;
}

void move_south_west_and_park(W w, const Box& b, int& ro, int& co, int &counter)
{
	for (; !PASTLEFT && !PASTBOT; ++ro, --co)
		w(ro, co) = ++counter;
	--ro; ++co;
	ATBOTTOM ? ++co : ++ro;
}

void zigzag_scan_general(W w, P p, int s)
{
	Box b = { p, p + s };
	Box b_ = b;
	b.crop(max_);

	int counter = 0;
	int ro = b.tl.ro, co = b.tl.co;

	//this check is only necessary during the entrance
	while (ro < b.br.ro && co < b.br.co)
	{
		//ro, co with respect to tl of b_
		//YOU CAN COMMENT OUT BELOW LINE to make the entire code behave as if the box size varies
		counter = get_zigzag_offset(ro - b_.tl.ro, co - b_.tl.co, s) - 1; //-1 is just offset var

#pragma region 
		if (ATTOP && ATLEFT) //origin of box
		{
			w(ro, co) = ++counter;
			int origin_diag = (ro - b_.tl.ro) + (co - b_.tl.co);
			//our convention is to move down from the origin of the uncropped box
			//so, if you are in the odd indexed diags of b_, you should move left
			ATRIGHT ? ++ro : //the box is one vertical line
				ATBOTTOM ? ++co : //the box is one horizontal line
				(origin_diag % 2 != 0) ? ++co : ++ro; //when you have room to move in both directions
		}
#pragma endregion //this is not required if the box does not go out of the grid
		else if (ATTOP)
			move_south_west_and_park(w, b, ro, co, counter);
		else if (ATLEFT)
			move_north_east_and_park(w, b, ro, co, counter);
		else if (ATBOTTOM)
			move_north_east_and_park(w, b, ro, co, counter);
		else if (ATRIGHT)
			move_south_west_and_park(w, b, ro, co, counter);
		else //reached past the bottom right corner
			break;
	}
}

//works as long as the box does not go out of grid when Wrap is used
//of course, works out of grid templated on WrapSilent
template<typename T>
void zigzag_scan_naive(T w, P p, int s)
{
	Box b = { p, p + s };

	int counter = 0;
	int ro = b.tl.ro, co = b.tl.co;

	while (ro < b.br.ro && co < b.br.co)
	{
		if (ATTOP)
		{
			for (; !PASTLEFT && !PASTBOT; --co, ++ro) w(ro, co) = ++counter;
			++co; --ro; //bring back into box
			ATBOTTOM ? ++co : ++ro; //Imp: this condition also works when BOTH ATBOTTOM and ATLEFT are true
		}
		else if (ATLEFT)
		{
			for (; !PASTTOP && !PASTRIGHT; ++co, --ro) w(ro, co) = ++counter;
			++ro; --co;
			ATRIGHT ? ++ro : ++co;
		}
		else if (ATBOTTOM)
		{
			for (; !PASTTOP && !PASTRIGHT; ++co, --ro) w(ro, co) = ++counter;
			++ro; --co;
			ATRIGHT ? ++ro : ++co;
		}
		else if (ATRIGHT)
		{
			for (; !PASTLEFT && !PASTBOT; ++ro, --co) w(ro, co) = ++counter;
			--ro, ++co;
			ATBOTTOM ? ++co : ++ro;
		}
		else
			break;
	}

}

void print_zigzag_indices()
{
	const int size_ = 6;
	int a[size_][size_] = { 0 };
	Wrap<size_> w; w.a = a;

	for (int ro = 0; ro < size_; ro++)
		for (int co = 0; co < size_; co++)
			w(ro, co) = get_zigzag_offset(ro, co, size_);

	print(w, true);
}

int main(int, char**)
{
#if 1
	Wrap<max_> w; w.a = arr;

	cout << "Space key to begin, and arrow keys to move the box" << nl;
	P pos = { 0, 0 };
	animate([&](int c) {
		system("cls");
		fill(w, 0, max_);
		update(c, pos);
		zigzag_scan_general(w, pos, 5);
		//zigzag_scan_naive(w, pos, 7);
		print(w, true);
	});
#else
	W w; w.a = arr;
	WrapSilent<max_> w2; w2.a = arr2;
	for (int t = 0; t < 100000; ++t)
	{
		P p = { SRAND10, SRAND10 };
		int s = rand() % 20;
		zigzag_scan_general(w, p, s);
		zigzag_scan_naive(w2, p, s);
		check_equal(w, w2, max_);
	}
#endif

}
#endif