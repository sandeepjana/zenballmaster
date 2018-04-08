#if 0
/*
Page 326
Consider the problem of storing n books on shelves in a library. The order of the
books is fixed by the cataloging system and so cannot be rearranged. Therefore, we
can speak of a book bi, where 1 ≤ i ≤ N, that has a thickness ti and height hi. The
length of each bookshelf at this library is L.

Now consider the case where the height of the books is not constant, but we have the freedom to adjust the
height of each shelf to that of the tallest book on the shelf. Thus the cost of a
particular layout is the sum of the heights of the largest book on each shelf.

Give an algorithm for this problem.

When decided to use dynamic programming, ask the following questions:
+ what is the recurrence relation?
	+ how many degrees of freedom the sub-problem has? In other words, what is the dimension of the table?
+ what are the boundary conditions? That is, for each axis, what is the smallest possible index in the table?
+ Make sure the sub-problem is operating on a smaller set 

Let DP[n] be the total height of the shelves for arranging n books
DP[n] = min {i, 1 <= i <= n : DP[i - 1] + cost[i, n]}, where cost[i, n] = max {height of books i to n} or INF if books' width is > L
We want DP[N]

|----------|------|
1    ...   i ...  N
[.D[i-1]...) 
		   [..H...]

DP[0] = 0
DP[1] = min{ DP[0] + h1 } = h1
DP[2] = min{ DP[0] + max{h1, h2}, DP[1] + h2} = min{ max{h1, h2}, h1 + h2} if b1 + b2 <= L
	  = min{ DP[0] + INF, DP[1] + h2} = h1 + h2 if b1 + b2 > L (books didnt fit in a single row)

(1D) Table construction: 
Precompute B[0, n] = sum {1 <= i <= n: bi} for all n values
Then B[i, n] = B[0, n] - B[0, i -1] = sum of width of books i to n
If B[i, n] > L, then cost[i, n] is INF.

cost[i, j] = max{cost[i, j - 1], height of book bj, INF if B[i, j] > L}

*/
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>

const int MAX_BOOK_WIDTH = 5;
const int BASE_BOOK_HEIGHT = 10;
const int MAX_BOOK_HEIGHT = 20;
const int SHELF_WIDTH = 15;

typedef std::pair<int, int> book_t;
typedef std::vector<std::vector<int>> vec2d;
struct shelf_t
{
	int rowStart;
	int heightSoFar;
};

void arrange_books(const std::vector<book_t>& books)
{
	std::vector<int> vtemp(books.size() + 1, -1);
	vtemp[0] = 0;
	vec2d cost(books.size() + 1, vtemp);

	for(int i = 1; i <= books.size(); ++i)
	{
		int totalwidth = 0;
		for(int j = i; j <= books.size(); ++j)
		{
			totalwidth += books[j - 1].first;
			if(totalwidth > SHELF_WIDTH)
				break;

			cost[i][j] = std::max(cost[i][j - 1], books[j - 1].second);
		}
	}

	std::vector<shelf_t> opt(books.size() + 1);
	opt[0].heightSoFar = 0;
	opt[0].rowStart = 0;

	for(int n = 1; n <= books.size(); ++n)
	{
		//DP[n] = min {i, 1 <= i <= n : DP[i - 1] + cost[i, n]}
		int minHeight = INT_MAX;

		//cost[i][n] becomes -1 when i becomes smaller, so the loop is in reverse!
		for(int i = n; i >= 1 && cost[i][n] != -1; --i)
		{
			int heightSoFar = opt[i - 1].heightSoFar + cost[i][n];
			if(minHeight > heightSoFar)
			{
				minHeight = opt[n].heightSoFar = heightSoFar;
				opt[n].rowStart = i;
			}
		}
	}

	std::vector<int> rowStart;
	int numBooks = books.size();
	while(numBooks > 0)
	{
		rowStart.push_back(opt[numBooks].rowStart);
		numBooks = opt[numBooks].rowStart - 1;
	}

	std::reverse(rowStart.begin(), rowStart.end());
	
	std::cout << "The shelves are arranged as \n";
	std::adjacent_find(rowStart.begin(), rowStart.end(),
		[&](int a, int b) -> bool {
			for(int i = a; i < b; ++i)
				std::cout << books[i - 1].second << " ";
			std::cout << "\n";
			return false;
	});
	for(int i = rowStart.back(); i <= books.size(); ++i)
	{
		std::cout << books[i - 1].second << " ";
	}
	std::cout << "\n";

	return;
}

int main(int , char**)
{
	std::vector<book_t> books(10);
	std::generate(books.begin(), books.end(), 
		[](){return book_t(std::make_pair(1 + rand() % MAX_BOOK_WIDTH, 
		BASE_BOOK_HEIGHT + rand()%(MAX_BOOK_HEIGHT - BASE_BOOK_HEIGHT))); 
	});

	arrange_books(books);

	return 0;
}



#endif