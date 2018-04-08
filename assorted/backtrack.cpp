#if 0

#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream> //for stringstream
#include <functional>

#define vecvec(T) vector< vector<T> >

#define PRINT(v, T) do{ copy(v.begin(), v.end(), ostream_iterator<T>(cout, ",")); cout << endl; } while(0)

using namespace std;

typedef struct
{
	int depth;
	int spread;
	bool isLeaf;
	int compute; //app can put whatever it wants for later reference
}state_t;

typedef bool (*pPromising)(vector<state_t>& v, state_t& s, void* ctx);

class CBacktrack
{
public:

	CBacktrack::CBacktrack(int nary, int depth, pPromising promising,
		void* ctx)
	{
		m_nary = nary;
		m_depth = depth;
		m_promising = promising;
		m_v.reserve(depth);
		m_ctx = ctx;
	}

	void DFS()
	{
		bool found = false;
		for(int i = 0; i < m_nary; i++)
		{
			m_DFS(0, i, found);
			if(found) return;
		}
	}

	void solution(vector<state_t>& v)
	{
		v.clear();
		v.insert(v.end(), m_v.begin(), m_v.end());
	}

private:
	pPromising m_promising;
	vector<state_t> m_v;
	int m_nary;
	int m_depth;
	void* m_ctx;

	void m_DFS(int depth, int spread, bool& found)
	{
		state_t s;
		s.depth = depth;
		s.spread = spread;
		s.isLeaf = (depth == m_depth - 1);
		s.compute = 0;
		
		if(m_promising(m_v, s, m_ctx))
		{
			m_v.push_back(s);

			if(s.isLeaf) //reached the promising leaf
			{
				found = true;
				return;
			}

			for(int i = 0; i < m_nary; i++)
			{
				m_DFS(depth + 1, i, found);
				if(found) 
					return;
			}

			m_v.pop_back(); //back track
		}
	}

};

bool queens_problem(vector<state_t>& v, state_t& s, void* ctx)
{
	//cout << s.depth << " , " << s.spread << endl;

	for(auto it = v.begin(); it != v.end(); it++)
	{
		if(it->spread == s.spread)
			return false;
		if(abs(s.spread - it->spread) == (s.depth - it->depth))
			return false;
	}

	return true;
}

bool print_subsets(vector<state_t>& v, state_t& s,  void* ctx)
{
	if(s.isLeaf) 
	{
		//reached the leaf -- so a subset complete
		cout << "{ ";
		for(auto it = v.begin(); it != v.end(); it++)
		{
			char c = (char) (it->depth + 'A');
			if(it->spread) cout << c; //to be or not to be
		}

		char c = (char) (s.depth + 'A');
		if(s.spread) cout << c;

		cout << " }" << endl;
		return false; //so that the recursion can continue
	}

	return true;
}

bool print_permutations(vector<state_t>& v, state_t& s, void* ctx)
{
	if(none_of(v.begin(), v.end(), [&s](state_t prev){
		return s.spread == prev.spread;
	}))
	{
		if(s.isLeaf) 
		{
			cout << "{ ";

			for_each(v.begin(), v.end(), 
				[](state_t prev){ 
					cout << (char) (prev.spread + 'A');
			});

			cout << (char)(s.spread + 'A') << " }" << endl; 
			return false;
		}
		return true;
	}

	return false;
}

//Moral of the story: the condition checks below
// lie in an order. Dont juggle
bool simple_paths_from_1_to_3(vector<state_t>& v, state_t& s, void* ctx)
{
	if(0 == s.depth)
	{
		if(s.spread == (1 - 1))
			return true; // my start node
		else
			return false; //dont go further because the code below 
		//refers to previous nodes
	}

	//construct_candidates {
	if(any_of(v.begin(), v.end(), 
		[&s](state_t prev) {
			return prev.spread == s.spread; //loop
	}))
	{
		return false;
	}

	vecvec(int) *adj = (vecvec(int) *) ctx;
	//from prev node to current node
	vector<int> *row = &((*adj)[v.back().spread]);
	if(row->end() == find(row->begin(), row->end(), s.spread + 1))
	{
		//cout << s.spread + 1 << " is NOT connected to " << v.back().spread + 1 << endl;
		return false;
	}
	else
	{
		//cout << s.spread + 1 << " is connected to " << v.back().spread + 1 << endl;
	}

	//} construct_candidates

	if(s.spread == (3 - 1))
	{
		for_each(v.begin(), v.end(), 
			[](state_t prev){
				cout << prev.spread + 1;
		});
		cout << 3 << endl;
		return false; //keep the recursion rolling
	}

	return true;
}

int main(int argc, char** argv)
{
	vector<state_t> v;

	//depth = length of solution vector
	//spread = number of candidates/options going from one node to its descendent
	//CBacktrack bt(4, 4, queens_problem, NULL);

	CBacktrack bt(2, 3, print_subsets, NULL);
#if 0
	CBacktrack bt(4, 4, print_permutations, NULL);

	vecvec(int) adj;
	vector<int> node;

	//node 1
	node.push_back(2);
	node.push_back(3);
	node.push_back(4);
	node.push_back(5);
	adj.push_back(node);

	//node 2
	node.clear();
	node.push_back(1);
	node.push_back(6);
	adj.push_back(node);

	//node 3
	node.clear();
	node.push_back(1);
	node.push_back(4);
	node.push_back(6);
	adj.push_back(node);

	//node 4
	node.clear();
	node.push_back(1);
	node.push_back(3);
	node.push_back(6);
	adj.push_back(node);

	//node 5
	node.clear();
	node.push_back(1);
	node.push_back(6);
	adj.push_back(node);

	//node 6
	node.clear();
	node.push_back(2);
	node.push_back(3);
	node.push_back(4);
	node.push_back(5);
	adj.push_back(node);

	CBacktrack bt(6, 6, simple_paths_from_1_to_3, (void*) &adj);
#endif
	bt.DFS();

	/*bt.solution(v);
	for(auto it = v.begin(); it != v.end(); it++)
	{
		cout << "( " << it->depth << " , " << it->spread << " )" << endl;
	}
	*/

	return 0;	
}


#endif