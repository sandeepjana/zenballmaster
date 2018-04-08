#if 0
/*
Consider the following data compression technique. We have a table of m text
strings, each at most k in length. We want to encode a data string D of length n
using as few text strings as possible. For example, if our table contains (a,ba,abab,b)
and the data string is bababbaababa, the best way to encode it is (b,abab,ba,abab,a)—
a total of five code words. Give an O(nmk) algorithm to find the length of the best
encoding. You may assume that every string has at least one encoding in terms of
the table.
*/

/* 
Solution below using back tracking: at each step down the tree, check if each of the symbol string in the 
table matches with the remaining target string.

Now, should I use a plain recursive function or fancy DFS class?
What is the depth of tree? At max, sizeof(target string)/sizeof(shortest symbol string)
Number of branches at each node? Number of symbols

This problem is similar/different to/from denominations problem:
 + the coins of a particular denomination can be used multiple number of times
 - this problem has implicit order in the solution:
	strings should concatenate in order to form target data string,
	coins need not be "in order" to make up a sum

 In case of denominations problem, once we are done with finding the (possible) solutions that include
 multiples of the highest denomiation, we move to the second highest denomination.
 In case of compression problem, even though we cannot find match for the first string
 at the beginning of the data string, we may find it somewhere in the middle.

*/

/* 
For dynamic programming solution:
+ what is the recurrence relation?
	+ how many degrees of freedom the sub-problem has? In other words, what is the dimension of the table?
+ what are the boundary conditions? That is, for each axis, what is the smallest possible index in the table?
+ Make sure the sub-problem is operating on a smaller set 

recurrence:
(Not encoding the feasibility check in the recurrence relation)
Let C[n] be the minimum number of symbols required to encode the string S[0, n), which is a prefix of target string S[0, N)
C[n] = min {i, 0 < i < n : C[i] + 1 such that S[i, n) is encoded by one of the symbols }, n > 1
C[1] = 1
e.g.: if symbols are {a, b, ab, abab} and the string is bababbaabab, N = 11
Then C[N] = min { C[N - 1] for 'b' matching S[N - 1, N), C[N - 2] for 'ab', C[N - 4] for 'abab'} + 1

For finding whether the suffix S[i, n) matches with the each symbol, we do some pre-computation:
create a bool table with table[i, j] being true if the suffix S[i, ..) contains jth symbol as prefix

*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <regex>
#include <cassert>

/* Note for constructing tree:
+ Since the API call DFS starts at depth 0, the first recursive call contains the 
	depth argument as 1.
+ The tree depth is set as the max length of the solution vector. 
+ When reached a promising leaf node, the recursion may or may not end depending on the problem:
for e.g., for an optimization problem the recursion may not end because we may reach another leaf
with better score.

depth of the leaf node = maximum number of elements in the possible solution vector

*/

struct state_t
{
	int depth;
	int branch;
	int whatever;
};

/* custom contains the parameters/config specific to the current problem */
struct custom_t
{
	std::vector<std::string> vs;
	std::string target;

	//
	int optimialSymbolLength;
};

class backtrack_t
{
public:
	backtrack_t(int tdepth, int degree, custom_t& ctx) : m_tdepth(tdepth), m_degree(degree),
		m_ctx(ctx)
	{
		m_v.reserve(tdepth);

		//custom init
		m_ctx.optimialSymbolLength = INT_MAX;
	}

	int dfs()
	{
		bool found = false;
		for(int i = 0; i < m_degree; ++i)
		{
			//I'm at root node, at depth 0. Now I will go through the children at depth 1
			m_dfs(1, i, found);
			if(true == found)
				break;
		}

		//probably the recursion was kept rolling even after the solution was found
		// with a hope to find a better solution
		return m_solution.size();
	}

	void print_solution()
	{
		std::string recon;
		std::cout << "Solution: \n";
		for(auto it = m_solution.begin(); it != m_solution.end(); ++it)
		{
			std::cout << m_ctx.vs[it->branch] << " ";
			recon += m_ctx.vs[it->branch];
		}
		std::cout << "\n";

		assert(recon == m_ctx.target);
	}

private:
	int m_tdepth; //tree depth
	int m_degree;
	std::vector<state_t> m_v;
	std::vector<state_t> m_solution;

	//custom stuff
	custom_t m_ctx;


	enum NODE_STATUS_T
	{
		PROMISING,
		DEAD_END,
		NAILED_IT,
		KEEP_NAILING
	};

	void m_dfs(int depth, int branch, bool& found)
	{
		state_t s;
		s.depth = depth;
		s.branch = branch;

		auto status = m_promising(s);

		if(NAILED_IT == status) //for this problem, this case does not arise
		{
			m_v.push_back(s);
			m_solution = m_v;
			found = true;
			return;
		}

		if(KEEP_NAILING == status)
		{
			m_solution = m_v;
			m_solution.push_back(s);
			return;
		}

		if(PROMISING == status)
		{
			if(depth == m_tdepth)
			{
				//though the node was promising, it is in fact a leaf
				//we could not nail it even though we reached the leaf
				//std::for_each(m_v.begin(), m_v.end(), 
				//	[](state_t& s){ printf("(%d, %d) ", s.depth, s.branch); });
				//printf("(%d, %d)\n", s.depth, s.branch);
				return;
			}

			m_v.push_back(s);

			//go deeper
			for(int i = 0; i < m_degree; ++i)
			{
				m_dfs(depth + 1, i, found);
				if(true == found)
					return;
			}

			m_v.pop_back(); //restore state aka backtrack
		}
	}

	//the custom function
	NODE_STATUS_T m_promising(state_t& s)
	{
		int symbolLengthSoFar = m_v.size();

		if(symbolLengthSoFar + 1 >= m_ctx.optimialSymbolLength)
			return DEAD_END; //no point in looking at the current node

		auto& vs = m_ctx.vs;
		int lengthSoFar = 0;
		
		for(auto it = m_v.begin(); it != m_v.end(); ++it)
		{
			lengthSoFar += vs[it->branch].size();
		}

		if(m_ctx.target.size() - lengthSoFar < vs[s.branch].size())
			return DEAD_END;

		if(true == std::equal(vs[s.branch].begin(), vs[s.branch].end(),
			m_ctx.target.begin() + lengthSoFar))
		{
			lengthSoFar += vs[s.branch].size();
			
			if(lengthSoFar == m_ctx.target.size())
			{
				m_ctx.optimialSymbolLength = symbolLengthSoFar + 1;

				return KEEP_NAILING;
			}
			else
				return PROMISING;
		}
		else
		{
			return DEAD_END;
		}
		
	}

};

typedef std::vector<std::vector<int>> vec2d;
struct cost_context_t
{
	int cost;
	int lookBehindPos;
	int symbol;
};

int dynapro_string_compression(const custom_t& ctx)
{
	auto& vs = ctx.vs;
	auto& target = ctx.target;
	vec2d table(target.size(), std::vector<int>(vs.size(), 0));

	//is there easy regex trick to search for the symbols all at once
	//giving what *sub set* of symbols matched?
	/* The following does not work because the regex does not detect overlapping matches.
	and lookahead assertions do not seem to be yet supported by this compiler.
	for(int i = 0; i < vs.size(); ++i)
	{
		const std::regex reg(vs[i]);

		std::for_each(std::sregex_iterator(target.begin(), target.end(), reg),
			std::sregex_iterator(), 
			[&](const std::smatch m){ 
				int matchEndsAt = m.position() + m.length() - 1;
				table[matchEndsAt][i] = 1; 
		});
	}
	*/
	for(size_t i = 0; i < target.size(); ++i)
	{
		for(size_t j = 0; j < vs.size(); ++j)
		{
			const std::string& s = vs[j];
			int matchEndsAt = i + s.size() - 1; 
			//matchEndsAt check: if symbol length is 1, and target[i] matches it, then matchEndsAt is i itself

			if(matchEndsAt < target.size() //if the suffix of target string is sufficiently long
				&& true == std::equal(s.begin(), s.end(), target.begin() + i))
			{
				//the prefix of the suffix of the target string matched with this symbol
				table[matchEndsAt][j] = 1;
			}
		}
	}

	std::vector<cost_context_t> menu(target.size());

	//for each position in the target string
	for(size_t i = 0; i < menu.size(); ++i)
	{
		cost_context_t c  = {INT_MAX, -1, -1};

		//for each symbol which has 1 in the table for this index
		for(size_t j = 0; j < vs.size(); ++j)
		{
			if(1 == table[i][j])
			{
				int lookBehindPos = i - vs[j].size();
				int costOfPrefix = (lookBehindPos < 0) ? 
					0 /* no previous match, the symbol matched all the way up to beginning of the string */
					: menu[lookBehindPos].cost;

				if(c.cost > costOfPrefix)
				{
					c.cost = costOfPrefix;
					c.symbol = j;
					c.lookBehindPos = lookBehindPos;
				}
			}
		}
		
		if(-1 == c.symbol) 
		{
			std::cout << "target string cannot be encoded with given symbols\n";
			return -1;
		}

		menu[i] = c;
		menu[i].cost += 1; //+1 for the cost of current symbol
	}

	//construct the solution
	std::vector<int> compressed;
	for(int pos = menu.size() - 1; pos != -1; )
	{
		compressed.push_back(menu[pos].symbol);
		pos = menu[pos].lookBehindPos;
	}

	std::reverse(compressed.begin(), compressed.end());

	std::string recon;
	std::for_each(compressed.begin(), compressed.end(),
		[&](int i) { 
			std::cout << vs[i] << " "; 
			recon += vs[i];
	});

	std::cout << "\n"; 
	assert(recon == target);

	return compressed.size();
}



int main(int , char**)
{
	custom_t ctx;
	ctx.target = std::string("ababbaabababbabababababbbbababa");
	//ctx.target = std::string("bababbaababa");
	auto& vs = ctx.vs;
	vs.emplace_back("a");
	vs.emplace_back("b");
	vs.emplace_back("ba");
	vs.emplace_back("abab");

	int dynaproCost = dynapro_string_compression(ctx);

	std::sort(vs.begin(), vs.end(), [](const std::string& left, const std::string& right)
	{
		return right.length() < left.length();
	});

	backtrack_t bt(ctx.target.size(), ctx.vs.size(), ctx);
	int dfsCost = bt.dfs();
	bt.print_solution();

	assert(dfsCost == dynaproCost);

	return 0;
}



#endif