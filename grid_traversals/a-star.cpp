#if 0
#include "grid_traversals.h"
#include <algorithm>
#include <cassert>

const int max_ = 10;
int arr[max_][max_];

/* Takeaways:
+ Maintaining explicit state helps rather than implicity keeping track by Openset, and some member variables
+ The priority queue Openset does not store distances in it, but just the points (ro, co).
	This is because the distances do get updated even for the points that are in Openset.
+ Pre-computation of heuristic for every node is not needed but doing so makes life a bit easier
+ 
*/

#define DIAG 1

enum State {
	INVALID, //an obstacle
	UNVISITED,
	INPROCESS,  //on fringe
	DONE //closed set
};

struct Info
{
	State state;
	P parent;
	int fromstart;
	int heuristic;
};

Info infoarr[max_][max_];

typedef Wrap<max_> W;
typedef Wrap<max_, Info> Wi;

//expects info member is set. 
//of course, poor man's priority queue, could have used std::priority_queue but 
//is it really trivial?
struct Openset
{
	Openset(Wi info_) : info(info_) {}

	Wi info;
	P set[max_*max_];
	int size = 0;

	void insert(P p) { set[size++] = p; }
	bool empty() { return size == 0; }
	//pre-condition: fromstart and heuristics are set
	P remove_min() {
		assert(false == empty());
		Info in = info(set[0]);
		int h = in.heuristic;
		int minscore = in.fromstart + h;
		int winner = 0;
		for (int i = 1; i < size; ++i)
		{
			in = info(set[i]);
			int score = in.fromstart + in.heuristic;
			if (minscore > score || minscore == score && in.heuristic < h)
			{
				minscore = score; h = in.heuristic;
				winner = i;
			}
		}
		P p = set[winner];
		set[winner] = set[--size];
		return p;
	}
};

int get_heuristic(P p)
{
#if DIAG
	int a = (max_ - p.ro), b = (max_ - p.co);
	//how did this over-estimate work? return a * a + b * b;
	return std::max(a, b);
#else
	return (max_ - p.ro) + (max_ - p.co);
#endif
}

/*
1. each node is either unvisited, in-process (open set = fringe), or done (closed set)
2. In loop:
	+ pick the smallest score node, say p, and check if it is the goal node
	+ Otherwise, add p to closed set 
	+ Explore its !done neighbors, and update their scores (specifically 'fromstart' distance might have changed)
	+ Add them to open set if they are not already part of it
*/

//returns false if no path is found
template<class T>
bool a_star(W canvas, Wi info, T show)
{
	//set up Info
	for (int ro = 0; ro < max_; ++ro)
		for (int co = 0; co < max_; co++)
		{
			Info& in = info(ro, co);
			in.state = (0 == canvas(ro, co)) ? UNVISITED : INVALID;
			in.heuristic = (0 == canvas(ro, co)) ? get_heuristic(P{ ro, co }) : -1;
		}

	Openset openset(info);
	{
		P p = { 0, 0 };
		Info& in = info(p);
		in.fromstart = 0;
		in.parent = p;
		openset.insert(p);
	}
	
	while (!openset.empty())
	{
		show();

		P p = openset.remove_min();
		if (max_ - 1 == p.ro && max_ - 1 == p.co)
			return true; //yey!

		Info& from = info(p);
		from.state = DONE;

#if DIAG
		const int neighbors = 8;
		int moves[][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 }, {-1, -1},
		{1, 1}, {-1, 1}, {1, -1} };
#else
		const int neighbors = 4;
		int moves[][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
#endif
		P q = p;
		for (int i = 0; i < neighbors; ++i)
		{
#define INRANGE(x) (0 <= x && x < max_)
			q.ro = p.ro + moves[i][0], q.co = p.co + moves[i][1];
			if (!INRANGE(q.ro) || !INRANGE(q.co)) continue;

			Info& to = info(q);
			if (INVALID == to.state || DONE == to.state) continue;

			if (UNVISITED == to.state || to.fromstart > from.fromstart + 1)
			{
				to.fromstart = from.fromstart + 1;
				to.parent = p;
				if (UNVISITED == to.state) 
					to.state = INPROCESS, openset.insert(q);
			}
		}
	}
	return false;
}

void mark_sets(W w, Wi info)
{
	for (int ro = 0; ro < max_; ro++)
		for (int co = 0; co < max_; co++)
		{
			if (info(ro, co).state == INPROCESS) w(ro, co) = 2;
			if (info(ro, co).state == DONE) w(ro, co) = 1;
		} 
}
void draw_path(W w, Wi info)
{
	int counter = 10;

	P q = { max_ - 1, max_ - 1 };
	while (!(q.ro == 0 && q.co == 0))
	{
		w(q) = ++counter;
		q = info(q).parent;
	}
	w(q) = ++counter;
}

void put_obstacles(W w)
{
	for (int i = 0; i < 50; ++i)
		w(RAND10, RAND10) = 99;
	w(0, 0) = w(max_ - 1, max_ - 1) = 0;
}

int main(int, char**)
{
	W w; w.a = arr;
	fill(w, 0, max_);
	put_obstacles(w);

	Wi info;
	info.a = infoarr;

	std::cout << "space key to step\n";

#define STEPBYSTEP 0

	if (false == a_star(w, info, [&] {
#if STEPBYSTEP
		int c = getch();
		system("cls");		
		mark_sets(w, info);
		print(w, true);
#endif
	}))
	{
		std::cout << "no path found\n";
	}
	else
	{
		system("cls");
		mark_sets(w, info);
		draw_path(w, info);
		print(w, true);
	}
}
#endif