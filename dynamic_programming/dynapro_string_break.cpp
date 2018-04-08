#if 0
/* 
A certain string processing language allows the programmer to break a string
into two pieces. It costs n units of time to break a string of n characters into two
pieces, since this involves copying the old string. A programmer wants to break a
string into many pieces, and the order in which the breaks are made can affect the
total amount of time used. For example, suppose we wish to break a 20-character
string after characters 3, 8, and 10. If the breaks are made in left-right order, then
the first break costs 20 units of time, the second break costs 17 units of time, and
the third break costs 12 units of time, for a total of 49 steps. If the breaks are made
in right-left order, the first break costs 20 units of time, the second break costs 10
units of time, and the third break costs 8 units of time, for a total of only 38 steps.
Give a dynamic programming algorithm that takes a list of character positions after
which to break and determines the cheapest break cost in O(n3) time.
*/
/*
When decided to use dynamic programming, ask the following questions:
+ what is the recurrence relation?
	+ how many degrees of freedom the sub-problem has? In other words, what is the dimension of the table?
+ what are the boundary conditions? That is, for each axis, what is the smallest possible index in the table?
+ Make sure the sub-problem is operating on a smaller set 

Recurrence relation:
Let the positions to break the string are P = {0, p1, ..., p(K-1), N} where p0 and pk just correspond to the begin and 
end of the string. The cost of breaking the string into K pieces
C[0, K) = min {i, 0 < i < K : C[0, i) + C0i + C[i, K) + CiK },
In general
C[i, k) = min {j, i < j < k : C[i, k) + Cik + C[k, j) + Ckj }
where
Cij = number of characters between pi and pj = (pj - pi)

For e.g., P = {0, 3, 8, 10, 20} -- below I converted braces to square
C[0, 4] = min{ C[0, 1] + C01 + C[1, 4] + C14, 
				C[0, 2] + C02 + C[2, 4] + C24,
				C[0, 3] + C03 + C[3, 4] + C34}
		where C01 = (3 - 0), C14 = (20 - 3) = 17, C02 = 5, C24 = (20 - 5) etc

	Two degrees: i and k which decide the cost C[i, k) -- Two dimensional table
Boundary conditions:
Nothing special -- C[0, 1] is simply zero, C[i, i + 1] is also zero: 

*/

#include <iostream>
#include <vector>
#include <algorithm>

struct table_element_t
{
	int cost;
	int parent;
};

typedef std::vector<std::vector<table_element_t>> vec2d;

int breakup_helper(int begin, int vend /* valid end */, vec2d& table, std::vector<int>& pos)
{
	if(vend == begin)
		return 0; //should never occur!

	if(-1 != table[begin][vend].cost)
		return table[begin][vend].cost;

	if(vend == begin + 1)
	{
		int mincost = 0;
		table[begin][vend].cost = mincost;
		table[begin][vend].parent = 0;
		printf("C[%d, %d] = %d\n", begin, vend, mincost);
		return mincost;
	}

	int mincost = INT_MAX;
	int minidx = -1;
	for(int i = begin + 1; i < vend; ++i)
	{
		int cost = breakup_helper(begin, i, table, pos) + 
			+ breakup_helper(i, vend, table, pos) + pos[vend] - pos[begin];

		if(cost < mincost)
		{
			mincost = cost;
			minidx  = i;
		}
	}

	printf("C[%d, %d] = %d, break at %d (%d)\n", begin, vend, mincost,
		pos[minidx], minidx);

	table[begin][vend].cost = mincost;
	table[begin][vend].parent = minidx;

	return mincost;
}

void trace_breakup(const vec2d& table, std::vector<int>& pos, int begin, int vend)
{
	int parent = table[begin][vend].parent;
	if(0 != parent)
	{
		//printf("[%d, %d] break at %d\n", begin, vend, parent);
		printf("[%d, %d] break at %d\n", pos[begin], pos[vend], pos[parent]);
		trace_breakup(table, pos, begin, parent);
		trace_breakup(table, pos, parent, vend);
	}
}

int string_breakup(const int* pos_, int size)
{
	table_element_t t = {-1, -1};
	vec2d table(size, std::vector<table_element_t>(size, t)); //contains C[i, j)
	std::vector<int> pos(pos_, pos_ + size);

	int cost = breakup_helper(0, size - 1, table, pos);

	trace_breakup(table, pos, 0, table[0].size() - 1);

	return cost;
}

int main(int , char**)
{
	const int pos[] = {0, 3, 8, 10, 20};
	std::cout << string_breakup(pos, _countof(pos)) << "\n";

	return 0;
}
#endif