#if 0
//The same algo implemented differently in gaffe.cpp
//This version uses simplified data structures.
//Refer to maxflow.jpg

#include <algorithm>
#include <iostream>
#include <fstream>
#include <array>

const int MNV = 16;

struct queue_t
{
	queue_t() {
		popid = pushid = 0;
	}
	std::array<int, MNV> ele;
	int popid;
	int pushid;
	int pop() { return ele[popid++]; }
	void push(int i) { ele[pushid++] = i; }
	bool empty() { return popid == pushid;  }
};

struct graph_t
{
	int numvertices;
	std::array<std::array<int, MNV>, MNV> adj;
	std::array<int, MNV> parent; 

	void print()
	{
		for (int i = 0; i < numvertices; ++i)
		{
			for (int j = 0; j < numvertices; ++j)
				std::cout << adj[i][j] << "\t";
			std::cout << '\n';
		}
		std::cout << '\n';
	}
	
	bool bfs() //return false if cant augment
	{
		//start BFS
		std::fill(begin(parent), end(parent), -1);
		queue_t q;

		parent[0] = 0; //source parent is source. Don't set it to -1!
		q.push(0); //zero is my source

		int target = numvertices - 1;
		while (!q.empty()) //when queue is empty but target is not visited, we are done.
		{
			auto from = q.pop();
			for (int to = 0; to < numvertices; ++to)
			{
				if (parent[to] != -1 || adj[from][to] == 0)
					continue; //if the residual capacity is zero

				parent[to] = from;
				if (to == target) //addition to usual BFS
					return true;

				q.push(to);
			}
		}
		return false;
	}

	bool augment_one_path()
	{
		auto foundpath = bfs();
		if (foundpath == false)
			return false;

		//walk the path from target to source
		//1. find minimum capacity on the path
		int mini = INT_MAX;
		const int source = 0;
		int to = numvertices - 1;
		while (1)
		{
			int from = parent[to];
			mini = std::min(mini, adj[from][to]);
			to = from;
			if (source == to)
				break;
		}
		//2. adjust the forward capacity and backward flow on the path
		to = numvertices - 1; //dont forget to re-initialize!
		while (1)
		{
			int from = parent[to];
			adj[from][to] -= mini; //forward dir wrt to current path -- decrease capacity
			adj[to][from] += mini; //revere dir wrt to current path -- increase flow
			to = from;
			if (source == to)
				break;
		}
		return true;
	}

	int find_max_flow()
	{
		while (true == augment_one_path())
		{
			print();
		}
		//once done, flow is equal to sum of edges going "into" source
		int flow = 0;
		for (int i = 1; i < numvertices; ++i)
			flow += adj[i][0];
		return flow;
	}
};

graph_t gf{};

int main(int, char**)
{
	int nume;
	std::ifstream file("graphdata.txt");
	file >> gf.numvertices >> nume;
	char dummy;
	for (int i = 0; i < gf.numvertices; ++i)
		file >> dummy;
	for (int i = 0; i < nume; ++i)
	{
		char from, to;
		int cap;
		file >> from >> to >> cap;
		gf.adj[from - 'A'][to - 'A'] = cap;
		gf.adj[to - 'A'][from - 'A'] = 0;
	}
	
	gf.print();
	std::cout << gf.find_max_flow() << '\n';

	return 0;
}
#endif