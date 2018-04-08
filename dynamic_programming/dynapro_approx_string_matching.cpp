#if 0
/* dynamic programs systematically search through all possibilities to ensure correctness,
while caching the intermediate results of the sub-problems to avoid recomputations.

me: dynamic program is a way to convert a recursive method of solving the problem to an iterative one by 
storing the intermediate results.

When decided to use dynamic programming, ask the following questions:
+ what is the recurrence relation?
	+ how many degrees of freedom the sub-problem has? In other words, what is the dimension of the table?
+ what are the boundary conditions? That is, for each axis, what is the smallest possible index in the table?
+ Make sure the sub-problem is operating on a smaller set 

*/

/* Compute the 'edit distance' between two strings */

/* 
 D[i, j] convention:
 INSERT and DELETE operations are mentioned in edit_distance (not the nonrecursive function) with respect to pattern.
 Text and pattern are given i and j indecies respectively.
 D[i - 1, j] means a character in the string whose index is 'j' is 'INSERTED'
 D[i, j - 1] means a character from the string whose index is 'j' is 'DELETED'

    <-- pattern --->
	+------+-------+
	| M/S  |   I   |
	+------+-------+
	|  D   |       |
	+------+-------+


*/
/* table e.g.

	s		h		o		u		l		d		<-- pattern
s	0(M)	1(D)	2(D)	3(D)	4(D)	5(D)	
h	1(I)	0(M)	1(D)	2(D)	3(D)	4(D)	
a	2(I)	1(I)	1(S)	2(D)	3(D)	4(D)	
l	3(I)	2(I)	2(I)	2(S)	2(M)	3(D)	
t	4(I)	3(I)	3(I)	3(I)	3(I)	3(S)	

If 'D' is encountered in the table, it calls for deleting the character in pattern that is on top of it.
If 'I' is encountered in the table, it calls for inserting a space in pattern after the character that is on top of it.

Note: 
step		direction of traversal
M			North west
S			North west
D			left (because the character in pattern is deleted, text index remains same)
I			up

therefore,
pattern	: should
edit req: MMSDMS
text	: sha_lt

*/
#include <limits.h>
#include <string>
#include <vector>

#include <algorithm>
#include <iostream>
#include <iterator>

#define NEEDLE_PIECES_IN_HAYSTACK 1
#define TRANSPOSITION_ERROR		  0

#define COST_INSERT_DELETE	1
#if NEEDLE_PIECES_IN_HAYSTACK
#define COST_MISMATCH		3 //should be greater than 2 = (insertion + deletion) cost for NEEDLE_PIECES_IN_HAYSTACK
#else
#define COST_MISMATCH		1
#endif

#define COST_TRANSPOSE		1
#define COST_TRANSPOSE_MISS	5

typedef struct
{
	int cost;
	char step;
}table_element_t;

typedef std::vector<std::vector<table_element_t> > vec2d;

void print_table(const vec2d &table)
{
	auto end = table.end();
	//for(auto& it = table.begin(); it != end; ++it)
	//{
	//	std::copy(it->begin(), it->end(), std::ostream_iterator<int>(std::cout, "\t"));
	//	printf("\n");
	//}

	auto end1 = table.end();
	for(auto& it1 = table.begin(); it1 != end1; ++it1)
	{
		auto end2 = it1->end();
		for(auto& it2 = it1->begin(); it2 != end2; ++it2)
			std::cout << it2->cost << "(" << it2->step << ")" << "\t";

		std::cout << std::endl;
	}
}

void print_solution(const vec2d &table, const std::string &pattern, const std::string &text)
{
	int i = table.size() - 1;
	int j = table[i].size() - 1;
	std::string v;

	do
	{
		auto step = table[i][j].step;
		v.push_back(step);
		switch(step)
		{
		case 'M':
		case 'S':
			--i; --j;
			break;
		case 'I':
			--i;
			break;
		case 'D':
			--j;
			break;
		default:
			--i, --j;
			break;
		}

		if(i < 0 || j < 0)
			break;

	}while(1);

	if(!(i < 0)) //the path ended at left without reaching the origin of table
	{
		v.insert(v.end(), i + 1, 'I');
	}
	else if(!(j < 0)) // ended at top
	{
		v.insert(v.end(), j + 1, 'D');
	}

	std::reverse(v.begin(), v.end());
	std::cout << v << std::endl;

	//if(j < 0 && !(i < 0))
	//{
	//	std::cout << std::string(i + 1, '_');		
	//}
		
	size_t k = 0;
	for(auto it = v.begin(); it != v.end(); ++it)
	{
		if('I' == *it)
		{
			std::cout << "_";
		}
		else
		{
			std::cout << pattern[k];
			++k;
		}
	}
	if(k < pattern.size())
	{
		std::cout << std::string(pattern.begin() + k, pattern.end());
	}

	std::cout << std::endl;

	//if(i < 0 &&  !(j < 0))
	//{
	//	std::cout << std::string(j + 1, '_');
	//}

	k = 0;
	for(auto it = v.begin(); it != v.end(); ++it)
	{
		if('D' == *it)
		{
			std::cout << "_";
		}
		else
		{
			std::cout << text[k];
			++k;
		}
	}
	if(k < text.size())
	{
		std::cout << std::string(text.begin() + k, text.end());
	}

	std::cout << std::endl;
}

/* this function uses recursion to find the optimal edit distance. i -> text and j -> pattern
TODO: confirm once: D[i, j] = min{ D[i - 1, j - 1] + cost of (mis)match, 
	D[i, j - 1] + cost of deleting a character in pattern or inserting a space in text, 
	D[i - 1, j] + cost of inserting a space in pattern or deleting a character in text}
The optimal edits can be tracked by following which of the three arguments above yielded to minimum. */

int edit_distance_helper(vec2d &table,  const std::string &pattern, const std::string& text, int patternPos, int textPos)
{
	if(-1 == patternPos && -1 == textPos)
		return 0;

	if(-1 == patternPos)
#if NEEDLE_PIECES_IN_HAYSTACK
		return 0;
#else
		return (textPos + 1)*COST_INSERT_DELETE;
#endif

	if(-1 == textPos)
		return (patternPos + 1)*COST_INSERT_DELETE;

	if(-1 != table[textPos][patternPos].cost)
		return table[textPos][patternPos].cost;

	int costMismatch = (pattern[patternPos] == text[textPos]) ? 0 : COST_MISMATCH;
	int opt[3];

	opt[0] = edit_distance_helper(table, pattern, text, patternPos - 1, textPos - 1) + costMismatch;
	opt[1] = edit_distance_helper(table, pattern, text, patternPos - 1, textPos) + COST_INSERT_DELETE;
	opt[2] = edit_distance_helper(table, pattern, text, patternPos, textPos - 1) + COST_INSERT_DELETE;

	int minIndex = opt[0] < opt[1] ? 0 : 1;
	minIndex = opt[minIndex] < opt[2] ? minIndex : 2;

	table[textPos][patternPos].cost = opt[minIndex];

	table[textPos][patternPos].step = (0 == minIndex) ? (costMismatch == 0 ? 'M' : 'S')
		: ((1 == minIndex) ? 'D' : 'I');

	return opt[minIndex];
}

int edit_distance(const std::string &pattern, const std::string &text)
{
	table_element_t t = {-1, 'x'};
	std::vector<table_element_t> v(pattern.size(), t);
	vec2d table(text.size(), v);

	int editDistance = edit_distance_helper(table, pattern, text, pattern.size() - 1, text.size() - 1);

	std::copy(pattern.begin(), pattern.end(), std::ostream_iterator<char>(std::cout, "\t\t"));
	std::cout << std::endl;	

	print_table(table);

#if NEEDLE_PIECES_IN_HAYSTACK
	int lastCol = table[0].size() - 1;
	editDistance = table[0][lastCol].cost;
	int minRow = 0;
	for(int i = 1; i < table.size(); ++i)
	{
		if(editDistance > table[i][lastCol].cost)
		{
			editDistance = table[i][lastCol].cost;
			minRow = i;
		}
	}

	table.resize(minRow + 1);
	print_solution(table, pattern, std::string(text.begin(), text.begin() + minRow + 1));
#else
	print_solution(table, pattern, text);
#endif

	return editDistance;
}

void solution_for_nonrecursive(const vec2d &table, std::string& v)
{
	int i = table.size() - 1; //pattern index
	int j = table[i].size() - 1; //text index

	v.clear();

	do
	{
		auto step = table[i][j].step;
		v.push_back(step);
		switch(step)
		{
		case 'M':
		case 'S':
			--i; --j;
			break;
		case 'I':
			--j;
			break;
		case 'D':
			--i;
			break;
		case 'T':
			i -= 2; j -= 2;
			break;
		default:
			--i, --j;
			break;
		}

		if(i < 0 || j < 0)
			break;

	}while(1);

	if('X' ==  v.back() && !(i < 0)) //the path ended at left without reaching the origin of table
	{
		v.erase(v.end() - 1);
		v.insert(v.end(), i + 1, 'D');
	}
	else if('X' ==  v.back() && !(j < 0)) // ended at top
	{
		v.erase(v.end() - 1);
		v.insert(v.end(), j + 1, 'I');
	}

	std::reverse(v.begin(), v.end());
	std::cout << v << std::endl;
}
int edit_distance_nonrecursive(const std::string &pattern, const std::string &text, std::string &solution)
{
	std::string p = ' ' + pattern;
	std::string t = ' ' + text;

	table_element_t ele = {-1, 'X'};
	vec2d table(p.size(), std::vector<table_element_t>(t.size(), ele));

	for(size_t i = 0; i < table.size(); ++i)
	{
		table[i][0].cost = i * COST_INSERT_DELETE;
	}
	for(size_t i = 0; i < table[0].size(); ++i)
	{
		table[0][i].cost = i * COST_INSERT_DELETE;
	}

	int opt[4];
	for(size_t i = 1; i < p.size(); ++i)
	{
		for(size_t j = 1; j < t.size(); ++j)
		{
			/* IMPORTANT: braces around the ?: are a must! */
			opt[0] = table[i - 1][j - 1].cost + ((p[i] == t[j]) ? 0 : COST_MISMATCH);
			opt[1] = table[i][j - 1].cost + COST_INSERT_DELETE; //insert in pattern (index i loops over pattern)
			opt[2] = table[i - 1][j].cost + COST_INSERT_DELETE; //delete from pattern 

#if TRANSPOSITION_ERROR
			opt[3] = COST_TRANSPOSE_MISS;
			if(i > 1 && j > 1)
				opt[3] = table[i - 2][j - 2].cost + ((p[i] == t[j - 1]) && (p[i - 1] == t[j])? COST_TRANSPOSE : COST_TRANSPOSE_MISS);

			const int numOptions = 4;
#else
			const int numOptions = 3;
#endif
			table[i][j].cost = opt[0];
			table[i][j].step = (p[i] == t[j]) ? 'M' : 'S';

			for(int k = 1; k < numOptions; ++k)
			{
				if(opt[k] < table[i][j].cost)
				{
					table[i][j].cost = opt[k];
					table[i][j].step = (k == 1) ? 'I' : (k == 2) ? 'D' : 'T';
				}
			}

			//std::cout << "table[" << i << "][" << j << "] " << table[i][j].cost << std::endl;
		}
	}

	print_table(table);

	solution_for_nonrecursive(table, solution);

	return table[p.size() - 1][t.size() - 1].cost;
}

void maximum_monotone_subsequence(const std::string& integerString)
{
	std::string monotone(integerString);
	std::stable_sort(monotone.begin(), monotone.end());

	std::cout << "Monotone: " << monotone << std::endl;

	std::string solution;

	//requires the COST_MISMATCH to be set to a high value
	edit_distance_nonrecursive(integerString, monotone, solution);

	solution.erase(std::remove_if(solution.begin(), solution.end(), [](char c){ return c == 'I'; }), solution.end());
	std::cout << "squeezed sol: " << solution << std::endl;

	std::string mms;
	int k = 0;
	for(auto it = solution.begin(); it != solution.end(); ++it, ++k)
	{
		if('M' == *it)
		{
			mms.push_back(integerString[k]);
		}
	}

	std::cout << "MMS: " << mms << std::endl;
}

/*
Find the shortest common supersequence (SCS) of T and P is the smallest sequence L such that both T and
P are a subsequence of L. 
Solution: Find LCS (longest common subsequence) of T and P. Then replace the insertion/deletion portions with letters
from T and P that donot belong to LCS.
e.g., LCS of T= democrat and P = republican is "eca" with insertioin/deletion pattern DIMDDIIIIIMDMDI
The SCS is then dremopublicratn.
|SCS| = |T| + |P| - |LCS|
The edit distance d(T, P) with no substitutions = # of insertions/deletions
= (|T| - |LCS|) + (|P| - |LCS|) = |T| + |P| - 2|LCS|
Therefore, d(T, P) = |SCS| - |LCS|

void shortest_super_sequence()
{
}
*/

int main(int , char** )
{
	int editDistance;
	std::string solution;
	//int editDistance = edit_distance("you should not", "thou shalt not lie");
	//int editDistance = edit_distance("stating",
	//	"cost of starting the match is independent of the position in the text");

	//editDistance = edit_distance("democrat", "republican");
	//printf("[recursive] edit distance %d\n", editDistance);

	//
	editDistance = edit_distance_nonrecursive("democrat", "republican", solution);
	//DIMDDIIIIIMDMDI

	//editDistance = edit_distance_nonrecursive("thou shalt not", "you should not", solution);
	//editDistance = edit_distance_nonrecursive("laogrithtm", "algorithm", solution);

	printf("[nonrecurs] edit distance %d\n", editDistance);

	//maximum_monotone_subsequence("243517698");

	return 0;
}

#endif