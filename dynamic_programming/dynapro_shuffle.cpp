#if 0
/*
When decided to use dynamic programming, ask the following questions:
+ what is the recurrence relation?
	+ how many degrees of freedom the sub-problem has? In other words, what is the dimension of the table?
+ what are the boundary conditions? That is, for each axis, what is the smallest possible index in the table?
+ Make sure the sub-problem is operating on a smaller set 

Suppose you are given three strings of characters: X, Y , and Z, where |X| = n,
|Y| = m, and |Z| = n + m. Z is said to be a shuffle of X and Y iff Z can be
formed by interleaving the characters from X and Y in a way that maintains the
left-to-right ordering of the characters from each string.
(a) Show that cchocohilaptes is a shuffle of chocolate and chips, but chocochilatspe
is not.
(b) Give an efficient dynamic-programming algorithm that determines whether Z
is a shuffle of X and Y . Hint: the values of the dynamic programming matrix
you construct should be Boolean, not numeric.

Solution:
s(i) be the ith character in the shuffle. (1 <= i)
b(i, j) is a boolean that is true if {s[1]...s[i + j]} is a shuffle of {p1[1]...p1[i]} and {p2[1]...p2[j]}
let b[i, j] = (b[i - 1, j] && (s[i + j] == p1[i])) || (b[i, j - 1] && (s[i + j] == p2[j]))
# of degrees of freedom = 2
base conditions:
b[0, j] = {s[k] | 1 <= k <= j} is equal to {p2[k] | 1 <= k <= j}, 1 <= j <= p2.size()
b[i, 0] = {s[k] | 1 <= k <= i} is equal to {p1[k] | 1 <= k <= i}, 1 <= i <= p1.size()
e.g. iteration:
b[1, 1] = (b[0, 1] && (s[2] == p1[1])) || (b[1, 0] && (s[2] == p2[1])) will be true if, say,
	b[0, 1] is true, i.e., s[1] = p2[1].

*/
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>

typedef std::vector<std::vector<int> > vec2d; //bool vectors are dangerous

void print_table(const vec2d &table)
{
	auto end = table.end();
	for(auto& it = table.begin(); it != end; ++it)
	{
		std::copy(it->begin(), it->end(), std::ostream_iterator<int>(std::cout, "\t"));
		printf("\n");
	}
	printf("\n");
}

//error checks pending
bool is_shuffle(const std::string& s, const std::string& p1, const std::string& p2)
{
	vec2d table(p1.size() + 1, std::vector<int>(p2.size() + 1));

	std::cout << "Initializing table...\n";
	for(int i = 0; i < table.size(); ++i)
	{
		table[i][0] = std::equal(s.begin(), s.begin() + i, p1.begin());
		//std::cout << std::string(s.begin(), s.begin() + i) << "\n";
		//at max, equal(s[0...table.size() - 1), p1[0...table.size() - 1)
		// = equal(s[0...p1.size()), p1[0...p1.size())
	}

	for(int j = 0; j < table[0].size(); ++j)
	{
		table[0][j] = std::equal(s.begin(), s.begin() + j, p2.begin());
	}

	int matchP1;
	int matchP2;
	int lucky;
	for(int i = 1; i < table.size(); ++i)
	{
		for(int j = 1; j < table[0].size(); ++j)
		{
			//b[i, j] = (b[i - 1, j] && (s[i + j] == p1[i])) || (b[i, j - 1] && (s[i + j] == p2[j]))
			matchP1 = s[i + j - 1] == p1[i - 1];
			matchP2 = s[i + j - 1] == p2[j - 1];
			lucky = (table[i - 1][j] && matchP1) || (table[i][j - 1] && matchP2);
			table[i][j] = lucky;
		}
	}

	print_table(table);
	return (1 == table[table.size() - 1][table[0].size() - 1]);
}

int main(int , char** )
{

	const std::string p1("chocolate");
	const std::string p2("chips");
	const std::string s("cchocohilaptes");

	bool isShuffle = is_shuffle(s, p1, p2);
	std::cout << "It is " << (isShuffle ? "a" : "NOT a") << " shuffle\n";
	return 0;
}

#endif