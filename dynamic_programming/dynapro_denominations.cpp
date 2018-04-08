#if 0
/* NOTE: NOT DONE WITH MEMOIZATION */

/*
Coins are minted with denominationsof {d1, . . . , dk} units. We want to count how many distinct ways C(X) there are
to make change of X units.

n1d1 + n2d2 + ... + nkdk = X say where ni is the number of coins of denomination di. ni >= 0
=> n1d1 + n2d2 + ... n(k-1)d(k-1) = X - nkdk, while (X - nkdk) >= 0
LHS is the new sub-problem. Therefore, 
Let C[X, i] be the number of ways to make change of X units with {d1, d2, ..., di}
Then, the recurrence relation is
C[x, i] = Sum { C[x - n * di, d(i-1)] | n : (x - n * dki) >= 0 }
We want to compute C[X, dk]

dimension of table: X x dk
base condition: C[x, d1] = 1, C[0, di] = 1

Q. What is the way of giving change that uses least number of coins?
Q. List down all ways

Lesson: If you are explicitly maintaining state during recursion (for e.g, using a vector as below),
then follow the stack model just like recursion: push_back the stuff to vector before recursive call,
and pop_back the same after the call.

*/
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <string>

struct change_t
{
	change_t(int numCoins, int denom) 
	{
		char str[32];
		sprintf(str, "(%d x %d) + ", numCoins, denom);
		s = str;
	}
	std::string s;
};

std::vector<change_t> change;

int num_ways_denom(int X, std::vector<int>& d, int numDenom)
{
	//printf("C(%d, %d)\n", X, numDenom);
	int lastDenom = d[numDenom - 1];

	if(numDenom == 1 && 0 != X)
	{
		change.push_back(change_t(X/lastDenom, lastDenom));
		std::for_each(change.begin(), change.end(), [](change_t& c) {
			std::cout << c.s;
		});
		std::cout << "\n";
		change.pop_back();
		return 1; //Assuming last denomination is 1 or something that can divide X
	}

	if(0 == X) 
	{
		change.push_back(change_t(X/lastDenom, lastDenom));
		std::for_each(change.begin(), change.end(), [](change_t& c) {
			std::cout << c.s;
		});
		std::cout << "\n";
		change.pop_back();
		return 1;
	}

	int numWays = 0;

	int remain = X;
	for(int n = 0; ; ++n)
	{
		remain = (X - n * lastDenom); //first time, remain = X
		if(remain < 0)
			break;

		change.push_back(change_t(n, lastDenom)); //append to the state

		numWays += num_ways_denom(remain, d, numDenom - 1);

		change.pop_back(); //restore the state
	}

	return numWays;
}

int main(int, char**)
{
	int X = 36;
	int dtemp[] = {1, 6, 10};
	std::vector<int> d(dtemp, dtemp + 3);

	std::cout << num_ways_denom(X, d, d.size()) << "\n";
}



#endif