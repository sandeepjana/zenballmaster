#if 0
#include "grid_traversals.h"
#include <algorithm>

//https://stackoverflow.com/questions/2339101/knights-shortest-path-chess-question
//check image: The num_moves code below is different from this image -- the former assumes x >= y

const int max_ = (5 + 1 + 5);
int arr[max_][max_];
int arr2[max_][max_];
P arr3[max_][max_];

typedef Wrap<max_> W;
typedef WrapSilent<max_> WS;
typedef Wrap<max_, P> WP;

std::ostream& operator<<(std::ostream& o, P p)
{
	o << p.ro << "," << p.co;
	return o;
}

template<class T>
struct queue_t
{
	queue_t() {
		popid = pushid = 0;
	}
	T ele[max_ * max_];
	int popid;
	int pushid;
	T pop() { return ele[popid++]; }
	void push(T i) { ele[pushid++] = i; }
	bool empty() { return popid == pushid; }
};

queue_t<P> q;

void knight_bfs(WS w, P c, WP parents)
{
	fill(w, -1, max_);
	w(c) = 0;
	parents(c) = c;

	q.push(c);
	while (!q.empty())
	{
		P from = q.pop();
		int off = w(from);
		int moves[][2] = { { -2, -1 },{ 2, -1 },{ -2, 1 },{ 2, 1 },
		{ -1, -2 },{ 1, -2 },{ -1, 2 },{ 1, 2 } };
		for (int i = 0; i < 8; ++i)
		{
			P to = { from.ro + moves[i][0], from.co + moves[i][1] };
			int& mini = w(to);
			if (w.oob != mini && (mini == -1 || mini > off + 1))
			{
				q.push(to);
				parents(to) = from;
				mini = off + 1;
			}	
		}
	}
}

//floor towards minus infinity
int floor_(int n, int d)
{
	int q = n / d;
	if (n * d < 0 && q * d != n)
		--q;
	return q;
}

int num_moves(int x, int y)
{
	if (0 == x && 0 == y) return 0;
	//restrict the plane to zeroth octant
	x = std::abs(x);
	y = std::abs(y);
	if (x < y) std::swap(x, y);
	assert(x >= y);

	if (x == 1 && y == 0) return 3;
	if (x == 2 && y == 2) return 4;

	int delta = x - y;
	return (y > delta) ? delta - 2 * floor_(delta - y, 3) :
		delta - 2 * floor_(delta - y, 4);
}

void knight_formulaic(W w, P c)
{
	for(int ro = 0; ro < max_; ++ro)
		for (int co = 0; co < max_; ++co)
		{
			int x = (co - c.co);
			int y = -(ro - c.ro);
			w(ro, co) = num_moves(x, y);
		}
}

void print_path(W canvas, WP parents, P p)
{
	fill(canvas, 0, max_);
	int counter = 10;
	canvas(p) = ++counter;
	P q = parents(p);
	while (!(p.co == q.co && p.ro == q.ro))
	{
		canvas(q) = ++counter;
		p = q;
		q = parents(p);
	}
	print(canvas, true);
}


int main(int, char**)
{
	WS w; W w2; WP parents;
	w.a = arr;
	w2.a = arr2;
	parents.a = arr3;
	
	P c = { 5, 5 };
	knight_bfs(w, c, parents);
	knight_formulaic(w2, c);

	//print(w2, max_, true);
	check_equal(w, w2, max_);
	//print(parents, max_, true);

	P p = { 5 + 2, 5 + 2};
	print_path(w2, parents, p);


	return 0;
}
#endif