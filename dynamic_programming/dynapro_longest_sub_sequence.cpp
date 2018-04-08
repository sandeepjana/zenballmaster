#if 0
/* The definition of maximum monotonic subsequence of dynapro_approx_string_matching is same as this.
The following is just another way of computing it. */

/* 
Consider a string of length N: s[0] ... s[N - 1] 
The recurrence relation is built on this premise:
Q: Given the character s[i] at i-th location, what is the largest subsequence in {0, i-1} that can be
successfully "extended" by the s[i]?
Let L[i] be the length of such sequence. Then,
	L[i] = max { L[j] : 0 < j < i and s[j] < s[i] } + 1 

Note that L[N - 1] obviously does not represent the LSS of string s. It just represents the LSS that 
ends with AND contains the last character s[N - 1]. The LSS of the string is actually 
	L_max = max { 0 < j < N : L[j] }

Q. How above logic reduces the number of computations? (wrt to what other technique?)

Q. Suppose s[j1] < s[i] and s[j2] < s[i] where j1 < j2 < i. When going from right to left for computing
L[i], is it OK to stop at s[j2] and report L[j2] + 1 or should one go beyond j2 till j1 and then
evaluate max {L[j2], L[j1]} + 1?
Ans: Not OK
e.g. 
1. ABCxxxPQxxxS -- here stopping at Q is OK because LSS at Q encompasses ABC as well
2. PQRxxxABxxxS -- here stopping at B is not OK because PQRS is longer than ABS
3. ABRxxxCPxxxS
*/

#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <regex>

typedef struct
{
	int length;
	int parent;
}table_element_t;

int longest_subsequence(const std::string &text, std::string &lss)
{
	std::vector<table_element_t> table(text.size());

	table[0].length = 1;
	table[0].parent = -1;
	for(size_t i = 1; i < text.size(); ++i)
	{
		auto si = text[i];
		int max = 0;
		int parent = -1;
		//check if any of the L[j] can be extended by si.
		for(size_t j = 0; j < i; ++j)
		{
			if(text[j] < si && max < table[j].length)
			{
				max = table[j].length;
				parent = j;
			}
		}
		table[i].length = max  + 1;
		table[i].parent = parent;
	}

	auto it = std::max_element(table.begin(), table.end(),
		//the predicate function should return true if first argument is "less than" the second
		[](table_element_t x, table_element_t y){ return x.length < y.length; });

	int lssLength = it->length;
	int lssEndsAt = (int) (it - table.begin());
	//reconstruct the LSS
	lss.push_back(text[lssEndsAt]);
	int parent = it->parent;
	while(-1 != parent)
	{
		lss.push_back(text[parent]);
		parent = table[parent].parent;
	}

	assert(lss.size() == lssLength);
	std::reverse(lss.begin(), lss.end());

	return lssLength;
}

//Not working
int lss_with_regex(const std::string &text, std::string &lss)
{
	//std::string monotone(text);
	//std::sort(monotone.begin(), monotone.end());

	//std::string monotone
	//const std::regex reg("(1)?(2)?(3)?(4)?(5)?(6)?(7)?(8)?(9)?", std::regex::extended);
	//const std::regex reg(".*?(1?).*?(2?).*?(3?).*?(4?).*?(5?).*?(6?).*?(7?).*?(8?).*?(9?).*");
	const std::regex reg(".*?(5?).*?(6?).*?(7?).*?(8?).*?(9?)");

	std::smatch m;
	const bool b = std::regex_search(text, m, reg);

	/*for(std::sregex_iterator it(text.begin(), text.end(), reg), end; it != end; ++it)
	{
		std::cout << (*it)[0] << std::endl;
	}*/

	for(int i = 0; i < m.size(); ++i)
	{
		std::cout << m[i] << std::endl;
	}

	lss = m[0];

	return lss.size();
}


/* 8 - 11: Assume that there are n numbers (some possibly negative) on a circle, and
we wish to find the maximum contiguous sum along an arc of the circle. Give an
efficient algorithm for solving this problem.
*/

/

int main(int , char** )
{
	std::string lss;
	std::cout << longest_subsequence("243517698", lss) << std::endl;
	//std::cout << lss_with_regex("243517698", lss) << std::endl; <-- didnt work :(
	std::cout << lss << std::endl;
	return 0;
}

#endif