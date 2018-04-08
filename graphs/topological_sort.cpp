#if 0
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>
#include <vector>
#include <numeric>
#include <cassert>
#include <queue>

const int max_ = 5000;
enum State
{
	UNVISITED,
	DISCOVERED,
	FINISHED
};
struct Node {
	int indegree;
	int numedges;
	int e;
	int state;
};
Node nodes[max_];
int tmp[max_][2];
int edges[max_];
int pushid;
int order[max_];

//returns false if found a cycle
bool dfs(int n) {
	Node& from = nodes[n];
	from.state = DISCOVERED;
	for (int i = from.e; i < from.e + from.numedges; ++i) {
		int to = edges[i];
		if (DISCOVERED == nodes[to].state)
			return false; //cycle
		if (UNVISITED == nodes[to].state && false == dfs(to))
			return false;
	}
	from.state = FINISHED;
	order[--pushid] = n; //<-- only addition to normal dfs
	return true;
}

bool topological_sort(int N, int M)
{
	// sorting the 'to' nodes is not sufficient for lexicographic TS (LTS).
	//in fact, i did not come across any method for LTS with dfs variation
#if 0 
	for (int i = 0; i < N; ++i)
	{
		Node& node = nodes[i];
		auto start = edges + node.e;
		auto end = start + node.numedges;
		std::sort(start, end, std::greater<int>());
	}
#endif

	pushid = N;
	for(int i = N - 1; i >= 0; --i)
		if(nodes[i].state == UNVISITED)
		{
			if (dfs(i) == false)
				return false;
		}

	return true;
}


struct Minheap
{
	int arr[max_];
	int size;
	void reset() { size = 0; }
	bool empty() { return 0 == size; }
	void insert(int a)
	{

	}
};

struct PPQ { //poorman priority queue
	int arr[max_]; //to remove an element, store -1 in its location
	int size = 0;
	const int bignum = max_ + 1;
	bool empty() { return size == 0; }
	void reset() { size = 0; }
	void push(int a) { //at some empty slot
		int i = 0;
		for (int s = 0; s < size && arr[i] != bignum; ++s, ++i);
		arr[i] = a;
		++size;
	}	
	int popmin() {
		int mindex = -1; int mini = bignum; int a;
		for(int s = 0, i = 0; s < size; ++i)
			if (bignum != (a = arr[i]))
			{
				mini = (mini > a) ? (mindex = i, a) : mini;
				++s;
			}
		arr[mindex] = bignum;
		--size;
		return mini;
	}
};
PPQ q;
//std::priority_queue<int, std::deque<int>, std::greater<int>> q;


bool lexico_first_topsort(int N, int M)
{
	q.reset();

	for (int i = 0; i < N; i++)
		if (nodes[i].indegree == 0)
			q.push(i);

	pushid = 0;
	while (!q.empty())
	{
		int a = order[pushid++] = q.popmin();
		//q.pop();
		Node& from = nodes[a];
		for (int i = from.e; i < from.e + from.numedges; ++i) {
			int to = edges[i];
			if (0 == --nodes[to].indegree)
				q.push(to);
		}

		//interestingly, we need not modify edge info
	}
	return (N == pushid) ? true : false;
}


/* Below method fails deduce ordering beyond what implied by edges.
Fails with
1
5 2
5 2
3 1
*/
bool topological_sort_brute_wont_work(int N, int M)
{
	for (int i = 0; i < N; ++i)
	{
		Node& node = nodes[i];
		auto start = edges + node.e;
		auto end = start + node.numedges;
		std::sort(start, end);
	}

	std::iota(order, order + N, 0);
	std::sort(order, order + N, [](int a, int b)
	{
		//a < b if there is an edge from a to b
		//a == b if there is no edge from a to b AND b to a
		Node& from = nodes[a];
		bool less = std::binary_search(edges + from.e, edges + from.e + from.numedges, b);
		if (less) return true;
		Node& to = nodes[b];
		bool greater = std::binary_search(edges + to.e, edges + to.e + to.numedges, a);
		if (greater) return false;
		return (a < b) ? true : false; //lexicographic
	});
	return true;
}

int nc2(int n) { return n * (n - 1) / 2; }

struct Gen {
	int N, M, c = -1;
	int from, to;
	std::set<std::pair<int, int>> eset;
	int pos[max_]; // pos[node index] = position in topo sort
	Gen& operator>>(int& i)
	{
		if (-1 == c) i = 200;
		else if (0 == c) { 
			i = N = 1 + (rand() % 50); 
			std::iota(pos, pos + N, 0);
			std::random_shuffle(pos, pos + N);
		}
		else if (1 == c) eset.clear(), i = M = rand() % (nc2(N) + 1);
		else if (0 == c % 2) do {
			from = rand() % N, to = rand() % N;
			if (pos[from] > pos[to]) std::swap(from, to);
			i = from + 1;
		} while (from == to || eset.insert(std::make_pair(from, to)).second == false);
		else i = to + 1;
		c = (++c) % (2 + 2 * M);
		return *this;
	}
};
Gen tg;

int pos[max_];
void test_toposort(int N, int M, bool lexico)
{
	//check if order is consistent with edges
	std::fill(pos, pos + N, -1);
	for (int i = 0; i < N; ++i)
		pos[order[i]] = i;
	for (size_t i = 0; i < N; i++)
		assert(pos[i] != -1);
	for (int i = 0; i < M; i++)
	{
		auto from = tmp[i][0], to = tmp[i][1];
		assert(pos[from] < pos[to]);
	}

	if (false == lexico)
		return;

	//lexicographically first 
	//always increasing as long as there is no dependency between consecutive nodes in the order
	int from = order[0];
	for (int i = 1; i < N; ++i)
	{
		int to = order[i];
		if (from > to) //there better be dependency
		{
			Node n = nodes[from];
			auto end = edges + n.e + n.numedges;
			assert(end != std::find(edges + n.e, end, to));
		}
		from = to;
	}
}

//#define INPUT std::cin
#define INPUT tg

const char nl = '\n';
void test_gen()
{
	using namespace std;
	int T;
	INPUT >> T; cout << T << nl;
	for (int tc = 0; tc < T; ++tc)
	{
		int N, M;
		INPUT >> N >> M; cout << N << " " << M << nl;
		for (int i = 0; i < M; ++i)
		{
			int from, to;
			INPUT >> from >> to; cout << from << " " << to << nl;
		}
		cout << nl;
	}
}

int main(int, char**)
{
	//test_gen(); return 0;
	freopen("topo_input.txt", "r", stdin);

	int T;
	INPUT >> T;
	for (int tc = 0; tc < T; ++tc)
	{
		int N, M;
		INPUT >> N >> M;

		for (int i = 0; i < N; ++i)
			nodes[i].numedges = nodes[i].indegree = 0;

		for (int i = 0; i < M; ++i)
		{
			int from, to;
			INPUT >> from >> to;
			--from, --to;
			tmp[i][0] = from; tmp[i][1] = to;
			++nodes[from].numedges;
			++nodes[to].indegree;
		}
		for (int i = 0, off = 0; i < N; ++i)
		{
			nodes[i].e = off;
			off += nodes[i].numedges;
			nodes[i].numedges = 0;
			nodes[i].state = UNVISITED;
		}
		for (int i = 0; i < M; ++i)
		{
			int from = tmp[i][0], to = tmp[i][1];
			int index = nodes[from].e + nodes[from].numedges;
			edges[index] = to;
			++nodes[from].numedges;
		}
#if 0
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < nodes[i].numedges; ++j)
				std::cout << i + 1 << " " << edges[nodes[i].e + j] + 1 << '\n';
		}
#endif
		bool lexico = true;
		bool possible = (lexico == true) ? lexico_first_topsort(N, M) : topological_sort(N, M);
		if (possible)
		{
			for (int i = 0; i < N; ++i)
				std::cout << order[i] + 1 << " ";
			std::cout << "\n";

			test_toposort(N, M, lexico);
		}
		else
			std::cout << "Cycle!\n";
	}

	return 0;
}
#endif