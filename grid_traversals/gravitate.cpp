#if 0
#include "grid_traversals.h"

const int max_ = 10;
int arr[max_][max_];
int arr2[max_][max_];

typedef Wrap<max_> W;

struct OctInfo {
	int oct; //[0, ... 7]
	bool onleftboundary; 	
};

//return +ve if ccw, 0 if collinear
int ccw(P p1, P p2) //ref (0, 0)
{
	int x1 = p1.co, y1 = -p1.ro;
	int x2 = p2.co, y2 = -p2.ro;
	return (x1 * y2) - (x2 * y1);
}

OctInfo get_octant(P center, P p)
{
	P refs[] = { {0, 1},
	{-1, 1}, {-1, 0}, {-1, -1}, //top points
	{0, -1},
	{1, -1}, {1, 0}, {1, 1} }; //bottom points

	p.ro -= center.ro;
	p.co -= center.co;

	int s[8 + 1];
	for (int i = 0; i < 8; ++i)
		s[i] = ccw(refs[i], p);
	s[8] = s[0];
	int j = 0;
	for (; j < 8; ++j)
		if ((s[j] >= 0) && s[j + 1] < 0)
			break;
		
	return OctInfo{ j, s[j] == 0 };
}

OctInfo get_octant_v2(P center, P p)
{
	int y = -(p.ro - center.ro);
	int x = +(p.co - center.co);

	if (0 == x && 0 == y)
		return OctInfo{ 8, true }; //always better to keep this corner case separate
								   
	//upper half plane, +x axis included, -x axis excluded: then offset is zero
	int off = (y > 0 || 0 == y && x > 0) ? 0 : 4; 
	
	//if the point belongs to lower half-plane, rotate the plane by 180 degrees, 
	//so that [4... 7] aligns with [0 ..3]
	//rotating by 180 is same as flipping wrt origin
	if (4 == off) x = -x, y = -y; 

	off += (x > 0) ? 0 : 2; //remeber: x = 0 belongs to octant 2, not to octant 1.

	//if point belongs to top left quadrant (O2 and O3), 
	//rotate point by 90 degress clockwise to align O2 and O3 with O0 and O1
	//90 degree clockwise rotation: y <- -x, x <- y
	if (x <= 0) {
		int t = y;
		y = -x; x = t;
	}
	assert(x > 0 && y >= 0);
	off += (x > y) ? 0 : 1; //point belongs to O0
	return OctInfo{ off, 0 == x || 0 == y || x == y };
}

OctInfo get_octant_v3(P center, P p)
{
	int y = -(p.ro - center.ro);
	int x = +(p.co - center.co);
	//basically two lines making half-open region
	int oct = (x == 0 && y == 0) ? 8 :
		(y >= 0 && x > y) ? 0 :
		(y >= x && x > 0) ? 1 :
		(x <= 0 && y > -x) ? 2 :
		(-x >= y && y > 0) ? 3 :
		(y <= 0 && -x > -y) ? 4 :
		(-y >= -x && x < 0) ? 5 :
		(x >= 0 && -y > x) ? 6 : 7;
	bool b = (x == 0 || y == 0 || std::abs(x) == std::abs(y));
	return OctInfo{ oct, b };
}

bool always_left_turn(int ro, int co, P c)
{
	ro -= c.ro;
	co -= c.co;
	//Q1 or Q3 among Q0 to Q3
	return (0 == co) || (ro < 0 && co < 0 || ro > 0 && co > 0);
}

//shortest side of L first
bool short_side_first(int ro, int co, P c)
{
#define ABSDEL(x) std::abs(x - c.x)
	return 0 == ABSDEL(co) || 0 != ABSDEL(ro) && ABSDEL(ro) < ABSDEL(co);
}

bool long_side_first(int ro, int co, P c)
{
	return ABSDEL(co) < ABSDEL(ro);
}


template<class F>
void el(W w, P c, P p, F go_row_loop)
{
	int ro = p.ro, co = p.co;

#define FOR_1SH(x) for(; x < c.x; ++x)
#define RFOR_1SH(x) for(; x > c.x; --x)

	int counter = 10;
	//so instead of putting two if else conditions, we use one while lopp
	while (ABSDEL(ro) || ABSDEL(co))
	{
		if (go_row_loop(ro, co, c))
		{
			//only one of the for loops below gets executed - saved some if checks
			FOR_1SH(ro) w(ro, co) = ++counter;
			RFOR_1SH(ro) w(ro, co) = ++counter;
		}
		else
		{
			FOR_1SH(co) w(ro, co) = ++counter;
			RFOR_1SH(co) w(ro, co) = ++counter;
		}
	}
	w(ro, co) = ++counter;
}

void shortest_path_8connected(W w, P c, P p)
{
	int x = p.co - c.co;
	int y = -(p.ro - c.ro);

	int counter = 10;

	//reaching diag == making |x| and |y| same, and equal to smaller of those abs values.
	//Decrementing |x| means decrementing (incremening) x if x is positive (negative)
#define FOR_REACH_DIAG(u, v) for(; std::abs(u) > std::abs(v); (u < 0) ? ++u : --u)
#define WALK_ON_DIAG(x, y)	for(; std::abs(x) != 0; (x < 0) ? ++x : --x, (y < 0) ? ++y : --y)

	//one (or none -- if already on diagonal) of FOR_REACH_DIAG will execute
	FOR_REACH_DIAG(y, x) w(c.ro - y, c.co + x) = ++counter;
	FOR_REACH_DIAG(x, y) w(c.ro - y, c.co + x) = ++counter;
	WALK_ON_DIAG(x, y) w(c.ro - y, c.co + x) = ++counter;

	w(c.ro - y, c.co + x) = ++counter; //touch the center
}

void staircase(W w, P c, P p)
{
	int x = p.co - c.co;
	int y = -(p.ro - c.ro);
	int counter = 10;
#if 1 //first complete staircase and then flat out
	while (x || y)
	{
		w(c.ro - y, c.co + x) = ++counter;
		if (x) (x < 0) ? ++x : --x;
		if (y) (y < 0) ? ++y : --y;
	}
#else // == shortest_path_8connected
	while (x || y)
	{
		w(c.ro - y, c.co + x) = ++counter;
		if (std::abs(x) == std::abs(y))
			(x < 0) ? ++x : --x, (y < 0) ? ++y : --y;
		else if (std::abs(x) > std::abs(y))
			(x < 0) ? ++x : --x;
		else 
			(y < 0) ? ++y : --y;
	}
#endif
	w(c.ro - y, c.co + x) = ++counter; //touch the center
}


int main(int, char**)
{
	W w; w.a = arr;
#if 0
	for (int t = 0; t < 100000; ++t)
	{
		P center = { 0, 0 }; //{ SRAND10, SRAND10 }; //
		P p = { SRAND10, SRAND10 };
		//P p = { 8, 5 };
		auto a = get_octant(center, p);
		auto b = get_octant_v3(center, p);
		assert(a.oct == b.oct);
		assert(a.onleftboundary == b.onleftboundary);
	}
#endif

#if 0
	for (int t = 0; t < 100000; ++t)
	{
		//shortest_path_8connected(w, P{ RAND10, RAND10 }, P{ RAND10 , RAND10 });
		el(w, P{ RAND10, RAND10 }, P{ RAND10 , RAND10 }, long_side_first);
	}
	return 0;
#endif

	std::cout << "Space key to begin, and arrow keys to move the box" << nl;
	P center = { 4,  4};
	animate([&](int c) {
		system("cls");
		fill(w, 0, max_);
		P p = { RAND10, RAND10 };
		el(w, center, p, long_side_first);
		//staircase(w, center, p);
		print(w, true);
	});


	return 0;

}
#endif