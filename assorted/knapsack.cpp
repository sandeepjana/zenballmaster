#if 0
#include <iostream>
#include <vector>
#include <algorithm>
/* depth of a node = the number of edges from root to this node */
/* depth of root = 0 */

/* height of the node = the number of edges on the "longest downward" 
simple path from this node to a leaf */
/* height of the nodes can be tricky because height of nodes at same depth can be different in different branches */
/* but height of the tree = max depth of the tree */

/* 
The knapsack problem is as follows: given a set of integers S = {s1, s2, . . . , sn},
and a given target number T, find a subset of S that adds up exactly to T. For
example, within S = {1, 2, 5, 9, 10} there is a subset that adds up to T = 22 but
not T = 23.
*/

/* Note for constructing tree:
+ Since the API call DFS starts at depth 0, the first recursive call contains the 
	depth argument as 1.
+ The tree depth is set as the max length of the solution vector. 
+ When reached a promising leaf node, the recursion may or may not end depending on the problem:
for e.g., for an optimization problem the recursion may not end because we may reach another leaf
with better score.

depth of the leaf node = maximum umber of elements in the possible solution vector

*/

struct state_t
{
	int depth;
	int branch;
	int whatever;
};

struct custom_t
{
	std::vector<int> numbers;
	int total;

};

class backtrack_t
{
public:
	backtrack_t(int tdepth, int degree, custom_t& ctx) : m_tdepth(tdepth), m_degree(degree),
		m_ctx(ctx)
	{
		m_v.reserve(tdepth);
	}

	bool dfs()
	{
		bool found = false;
		for(int i = 0; i < m_degree; ++i)
		{
			//I'm at root node, at depth 0. Now I will go through the children at depth 1
			m_dfs(1, i, found);
			if(true == found)
				break;
		}

		return found;
	}

	//custom stuff
	void print_solution()
	{
		std::cout << "Solution: \n";
		for(auto it = m_v.begin(); it != m_v.end(); ++it)
		{
			if(it->branch)
				printf("%d + ", m_ctx.numbers[it->depth - 1]);
		}
		std::cout << "\n";
	}

private:
	int m_tdepth; //tree depth
	int m_degree;
	std::vector<state_t> m_v;

	//custom stuff
	custom_t m_ctx;


	enum NODE_STATUS_T
	{
		PROMISING,
		NAILED_IT,
		DEAD_END
	};

	void m_dfs(int depth, int branch, bool& found)
	{
		state_t s;
		s.depth = depth;
		s.branch = branch;

		auto status = m_promising(s);

		if(NAILED_IT == status)
		{
			m_v.push_back(s);
			found = true;
			return;
		}

		if(PROMISING == status)
		{
			if(depth == m_tdepth)
			{
				//though the node was promising, it is in fact a leaf
				//we could not nail it even though we reached the leaf
				std::for_each(m_v.begin(), m_v.end(), 
					[](state_t& s){ printf("(%d, %d) ", s.depth, s.branch); });
				printf("(%d, %d)\n", s.depth, s.branch);
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
		//if 
		int current = m_ctx.numbers[s.depth - 1];
		int sumSoFar = (0 == m_v.size()) ? 0 : m_v.back().whatever;

		s.whatever = sumSoFar;
		if(1 == s.branch) //1 means include this number
		{
			s.whatever += current; 
		}

		if(s.whatever == m_ctx.total)
			return NAILED_IT;
		if(s.whatever > m_ctx.total)
			return DEAD_END;

		//branch and bound: assumes that the numbers are sorted in ascending order
		//TODO

		return PROMISING;
	}


};


int main(int , char**)
{
	std::vector<int> numbers;
	numbers.push_back(1);
	numbers.push_back(2);
	numbers.push_back(5);
	numbers.push_back(9);
	numbers.push_back(10);

	custom_t ctx;
	ctx.numbers = numbers;
	ctx.total = 22;

	backtrack_t bt(numbers.size(), 2, ctx);
	if(true == bt.dfs())
	{
		bt.print_solution();
	}

	return 0;
}

#endif




