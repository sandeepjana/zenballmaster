#if 0
#include <iostream>
#include <iomanip>
using namespace std;

char *read_line (char *buf, size_t length, FILE *f);

class CGraphLite
{
public:
	CGraphLite(int numVertices)
		:m_numVertices(numVertices), m_dist(nullptr)
	{
		m_v = alloc_2d_array(numVertices);
		for(int i = 0; i < m_numVertices; ++i)
			for(int j = 0; j < m_numVertices; ++j)
				m_v[i][j] = (i == j)? 0 : INT_MAX;
	}
	~CGraphLite()
	{
		free_2d_array(m_v, m_numVertices);
		if(m_dist) free_2d_array(m_dist, m_numVertices);
		if(m_path) free_2d_array(m_path, m_numVertices);
	}
	void add_edge(int from, int to, int weight)
	{
		m_v[from][to] = m_v[to][from] = weight;
		cout << "Edge: " << setw(4) << from << " " << setw(4) << to 
			<< " " << setw(4) << weight << endl;
	}
	void floyd()
	{
		m_dist = alloc_2d_array(m_numVertices);
		m_path = alloc_2d_array(m_numVertices);
		for(int i = 0; i < m_numVertices; ++i)
			for(int j = 0; j < m_numVertices; ++j)
			{
				m_dist[i][j] = m_v[i][j];
				m_path[i][j] = -1;
			}

		for(int k = 0; k < m_numVertices; ++k)
			for(int i = 0; i < m_numVertices; ++i)
				for(int j = 0; j < m_numVertices; ++j)
				{
					//the 'less than' condition dictates what value m_path takes if i = j
					//overflow chances:
					//if(m_dist[i][j] >  m_dist[i][k] + m_dist[k][j])
					if(m_dist[i][j] - m_dist[k][j] >  m_dist[i][k])
					{
						m_dist[i][j] = m_dist[i][k] + m_dist[k][j];
						m_path[i][j] = k; 
					}
				}
		//At the end of all loops, m_path[i][j] is the last intermediate vertex k,
		//such that mindist[i][k] + mindist[k][j] = mindist[i][j].
		//For e.g, say min route from 1 to 2: 1 -> 5 -> 9 -> 3 -> 4 -> 2. Then k is 9.
		//But this has no significance to our algorithm.
		//In fact, k suffices to be just (any) one of the intermediate vertices 
		//in the shortest route from i to j.
		//m_path[i][j] will be -1 if there are no intermediate vertices.
		//i.e. if m_v[i][j] is itself the least distance between i and j,
		//or i = j (also if no route from i to j at all -- not considering that)
	}
	void print_path_array()
	{
		for(int i = 0; i < m_numVertices; ++i)
		{
			for(int j = 0; j < m_numVertices; ++j)
			{
				cout << setw(4) << m_path[i][j] << " ";
			}
			cout << endl;
		}
	}
	void print_root(int from, int to)
	{
		cout << from << " -> ";
		print_root_util(from, to);
		cout << to << endl;
	}
private:
	void print_root_util(int from, int to)
	{
		if(-1 != m_path[from][to]) //if intermediate vertices between from and to exist
		{
			print_root_util(from, m_path[from][to]);
			cout << m_path[from][to] << " -> ";
			print_root_util(m_path[from][to], to);
		}
	}
	int ** alloc_2d_array(int s)
	{
		int ** arr = new int*[s];
		for(int i = 0; i < s; i++)
		{
			arr[i] = new int[s];
		}
		return arr;
	}
	void free_2d_array(int ** arr, int s)
	{
		for(int i = 0; i < s; i++)
			delete [] arr[i];
		delete [] arr;
	}
	int m_numVertices;
	int **m_v;
	int **m_dist;
	int **m_path;
};

int main(int argc, char** argv)
{
	char* buf = new char[128];
	int nVertices, nEdges;

	FILE* fp = fopen("graphdata.txt", "r");
	char* p = read_line(buf, 127, fp);
	sscanf(p, "%d %d", &nVertices, &nEdges);
	for(int v = 0; v < nVertices; v++)
	{
		p = read_line(buf, 127, fp);
	}

	CGraphLite g(nVertices);

	char x, y;
	int weight;
	for(int e = 0; e < nEdges; e++)
	{
		p = read_line(buf, 127, fp);
		sscanf(p, "%c %c %d", &x, &y, &weight);
		g.add_edge((int) (toupper(x) - 'A'), (int) (toupper(y) - 'A'), weight); 		
	}

	g.floyd();
	g.print_path_array();

	for(int i = 0; i < nVertices; i++)
	{
		for(int j = 0; j < nVertices; j++)
			g.print_root(i, j);
		cout << endl;
	}

}

#endif