#if 0
#include <iostream>
#include <memory>
#include <vector>
#include <deque>
#include <algorithm>
#include <ios> //boolalpha
#include <functional> //std::function
#include <set>
using namespace std;

char *read_line (char *buf, size_t length, FILE *f);

//Implementation notes: Each vertex can be either identified by an index in a vertex vector/array
// or with a pointer to its memory.
#define IMPL_SKIENA 0
#define IMPL_PITTS	1
#define IMPL_OWN	2
#define IMPL		IMPL_OWN

#if IMPL == IMPL_OWN
typedef char graph_element_t;
typedef enum
{
	VERTEX_STATE_UNDISCOVERED,
	VERTEX_STATE_DISCOVERED,
	VERTEX_STATE_PROCESSED
}VERTEX_STATE_T;
typedef enum 
{
	EDGE_FORWARD,
	EDGE_ANTIPARALLEL
}EDGE_TYPE_T; //for maximum flow, but not required it seems

typedef shared_ptr<struct label_t_> label_ptr_t;
typedef struct label_t_
{
	int label;
	label_ptr_t next;
}label_t;

//typedef shared_ptr<struct vertex_t_> vertex_ptr_t;
typedef struct vertex_t_* vertex_ptr_t;
typedef shared_ptr<int>	int_ptr_t;
typedef struct vertex_t_
{
	graph_element_t ele;
	//meta info
	vertex_ptr_t	parent;	  //for backtracking in general
	VERTEX_STATE_T	state;
	int				time;
	int				outdegree; //num children in DFS
	vertex_ptr_t	era; //earliest reachable ancestor (for Art vertex)
	int				mindistance; //weight of edge to the parent (for Prim's)
	label_ptr_t		labelTwig; //label of component (for Kruskal's)			
	int				distFromParent; //quick access (for maximum flow)
	vector<unique_ptr<struct edge_t_>> edges;
}vertex_t;
//earliest reachable ancestor of a vertex: the topmost ancestor that can be 
// reached from (downward) tree edges and back edges from this vertex.
//Note that parent node cannot be a ERA unless we can reach it via a back edge.
typedef struct edge_t_
{
	vertex_ptr_t to;
	int weight;
	EDGE_TYPE_T type;//for maximum flow
}edge_t;

typedef struct
{
	vertex_ptr_t from;
	vertex_ptr_t to;
	int weight;
}edge_pair_t;

typedef void (*pPreProcessVertex)(vertex_ptr_t v);
typedef void (*pPostProcessVertex)(vertex_ptr_t v);
typedef void (*pProcessEdge)(vertex_ptr_t v1, vertex_ptr_t v2);
class CGraph
{
public:
	bool m_isDirected;
	CGraph(bool isDirected)
	{
		m_isDirected = isDirected;
		m_isVertexStateDirty = false;
	}
	void clear()
	{
		m_v.erase(m_v.begin(), m_v.end());
	}
	void add_vertex(graph_element_t c)
	{
		vertex_ptr_t v(new vertex_t);
		v->ele = c;
		v->state = VERTEX_STATE_UNDISCOVERED;
		v->parent = nullptr; //even if you dont assign
		v->time = 0;
		v->outdegree = 0;
		v->era = nullptr;
		m_v.push_back(v);
	}
	void add_vertex(vertex_ptr_t vin)
	{
		vertex_ptr_t v(new vertex_t);
		v->ele = vin->ele;
		v->state = VERTEX_STATE_UNDISCOVERED;
		v->parent = nullptr;
		v->time = vin->time;
		v->outdegree = vin->outdegree;
		v->mindistance = vin->mindistance;
		v->era = nullptr;
		m_v.push_back(v);
	}
	void print_vertices()
	{
		for(auto it = m_v.begin(); it != m_v.end(); it++)
		{
			cout << it - m_v.begin() << ":" << (*it)->ele << 
				" " << (*it)->mindistance << endl;
		}
		cout << endl;
	}
	void add_edge(int x, int y, int weight = 1, EDGE_TYPE_T etype = EDGE_FORWARD)
	{
		//x -> y
		if(x >= m_v.size() || y >= m_v.size())
		{
			cout << "vertices not found " << endl;
			return;
		}
		unique_ptr<edge_t> e(new edge_t);
		e->to = m_v[y];
		e->weight = weight;
		e->type = etype;
		m_v[x]->edges.push_back(std::move(e));

		if(false == m_isDirected)
		{
			//y -> x
			unique_ptr<edge_t> be(new edge_t);
			be->to = m_v[x];
			be->weight = weight;
			be->type = etype;
			m_v[y]->edges.push_back(std::move(be));
		}
	}
	void add_edge(graph_element_t x, graph_element_t y, int weight = 1, EDGE_TYPE_T etype = EDGE_FORWARD) //not passing weight yet
	{
		auto v1 = find_if(m_v.begin(), m_v.end(), [&x](vertex_ptr_t v)
		{
			return v->ele == x;
		});
		if(v1 == m_v.end()) { cout << "add_edge fatal"; return; }

		auto v2 = find_if(m_v.begin(), m_v.end(), [&y](vertex_ptr_t v)
		{
			return v->ele == y;
		});
		if(v2 == m_v.end()) { cout << "add_edge fatal"; return; }

		add_edge((int) (v1 - m_v.begin()), (int) (v2 - m_v.begin()), weight, etype);

	}
	void print_edges()
	{
		for(auto v = m_v.begin(); v != m_v.end(); v++)
		{			
			for(auto e = (*v)->edges.begin(); e != (*v)->edges.end(); e++)
			{
				cout << (*v)->ele;
				(m_isDirected) ? cout << " -> " : cout << " - ";
				cout << (*e)->to->ele << " " << (*e)->weight << endl;
			}
		}
	}
	//Breadth  First Search
	//Breadth first traversal builds tree that defines the shortest from the root (first node) 
	// to all other nodes in the (unweighted) graph.
	bool bfs(graph_element_t s = 0, graph_element_t t = 0)
	{
		//Oh boy, no recursion!
		//print the vertices in the BFS order starting from the first vertex
		//insert root into q
		//step1: remove element
		//process the element
		//discover all its adj vertices which are undiscovered and push them to q
		//goto step 1
		std::deque<vertex_ptr_t> q;

		//addition for maximum flow problem {
		vertex_ptr_t source = m_v[0];
		vertex_ptr_t sink = nullptr;
		if(0 != s)
		{
			for(auto v = m_v.begin(); v != m_v.end(); ++v)
			{
				if((*v)->ele == s)
				{
					source = (*v); break;
				}
			}
		}

		if(0 != t)
		{
			for(auto v = m_v.begin(); v != m_v.end(); ++v)
			{
				if((*v)->ele == t)
				{
					sink = (*v); break;
				}
			}
		}

		bool sinkFound = false;
		//}

		source->parent = NULL;
		q.push_back(source);
		while(!q.empty())
		{
			auto v = q.front();
			cout << v->ele << " ";
			v->state = VERTEX_STATE_PROCESSED;
			q.pop_front();

			//addition for maximum flow problem {
			if(sink == v)
			{
				sinkFound = true;
				break;
			} //}

			for(auto it = v->edges.begin(); it != v->edges.end(); it++)
			{
				//addition for maximum flow problem {
				if(0 == (*it)->weight) 
					continue; // as if no edge, or edge weight is INT_MAX
				//}
				vertex_ptr_t nextVertex = (*it)->to;
				if(VERTEX_STATE_UNDISCOVERED ==  nextVertex->state)
				{
					nextVertex->state = VERTEX_STATE_DISCOVERED;
					nextVertex->parent = v;
					nextVertex->distFromParent = (*it)->weight;
					q.push_back(nextVertex);
				}
			}
		}
		
		cout << endl;
		m_isVertexStateDirty = true;
		reset_states();

		return sinkFound;
	}
	void find_path_backto_root(graph_element_t x)
	{
		bfs();
		auto vit = find_if(m_v.begin(), m_v.end(), [&x](vertex_ptr_t v)
		{
			return v->ele == x;
		});
		if(vit == m_v.end()) return;

		vertex_ptr_t v = (*vit);
		while(1)
		{
			cout << v->ele << " ";
			if(v->parent == NULL)
				 break;
			v = v->parent;
		}
	}
	void find_path_from_root(graph_element_t x) //shortest with unity weights!
	{
		bfs();
		auto vit = find_if(m_v.begin(), m_v.end(), [&x](vertex_ptr_t v)
		{
			return v->ele == x;
		});
		if(vit == m_v.end()) return;

		vertex_ptr_t v = (*vit);
		find_path_from_root_util(v);
		cout << endl;
	}
	void dfs(pPreProcessVertex a = nullptr, pPostProcessVertex b = nullptr, pProcessEdge c = nullptr)
	{
		m_pPreProcessVertex = a;
		m_pPostProcessVertex = b;
		m_pProcessEdge = c;

		for(auto v = m_v.begin(); v != m_v.end(); ++v)
		{	//for directed graphs, it is unlikely that a single dfs will cover all vertices
			if((*v)->state != VERTEX_STATE_DISCOVERED)
			{
				cout << endl << "DFS iteration>> ";
				dfs_util((*v));
			}
		}

		reset_states();
	}
	void print_eras()
	{
		for(auto v = m_v.begin(); v != m_v.end(); ++v)
		{
			if((*v)->era != nullptr)
				cout << (*v)->ele << " -> " << (*v)->era->ele << endl;
		}
	}
	//Add a vertex to the tree
	//Loop thru all that vertex's edges and update a global structure
	// which holds the minimal distances to all nontree vertices. 
	//Once above loop ends, add to the tree vertex that has smallest edge
	void minimal_spanning_tree_prim(CGraph& mst)
	{
		grow_tree_greedy(mst, [](vertex_ptr_t from, vertex_ptr_t to, int weight)
		{
			if(weight < to->mindistance)
			{
				to->mindistance = weight;
				to->parent = from;
			}
		});
	}
	void dijkstra(CGraph& shortcut)
	{
		grow_tree_greedy(shortcut, [](vertex_ptr_t from, vertex_ptr_t to, int weight)
		{
			//boy, careful with overflows. from->mindistance can (or is it at least of this algo?) be INT_MAX
			//if(weight + from->mindistance < to->mindistance)
			if(weight < (to->mindistance - from->mindistance))
			{
				to->mindistance = weight + from->mindistance;
				to->parent = from;
			}
		});
	}
	void grow_tree_greedy(CGraph& mst, std::function<void (vertex_ptr_t, vertex_ptr_t, int)> optimization_step)
	{
		mst.m_isDirected = false;
		vertex_ptr_t v;
		for(auto it = m_v.begin(); it != m_v.end(); ++it)
		{
			(*it)->state = VERTEX_STATE_UNDISCOVERED;
			(*it)->mindistance = INT_MAX;
			(*it)->parent = nullptr;
		}

		//Dont forget!
		m_v[0]->mindistance = 0;

		v = m_v[0];	
		//loop ends when no more non tree vertexes
		while(nullptr != v) //v is nullptr if there are no more non-tree vertices
		{
			v->state = VERTEX_STATE_DISCOVERED;
			mst.add_vertex(v);
			if(nullptr != v->parent) //if not for the starting vertex
			{
				mst.add_edge(v->parent->ele, v->ele, get_edge_weight(v->parent, v));
			}
			//loop thru the edges of this vertex 'v'
			//for_each does not compile because edge_t is not copyable (unique_ptr)!
			//for_each(v->edges.begin(), v->edges.end(), [&v](edge_t e)
			for(auto e = v->edges.begin(); e != v->edges.end(); ++e)
			{
				vertex_ptr_t to = (*e)->to;
				if(VERTEX_STATE_UNDISCOVERED == to->state) //for all the non-tree vertices linking to v
				{
					optimization_step(v, to, (*e)->weight);
				}
			}

			//pick the next vertex: it has the least edge weight from tree to nontree vertices
			int mindistance = INT_MAX;	v = nullptr;
			for_each(m_v.begin(), m_v.end(), [&mindistance, &v](vertex_ptr_t v2)
			{
				if((VERTEX_STATE_UNDISCOVERED == v2->state) //for all the non-tree vertices
					&& (v2->mindistance < mindistance)) //the vertex with least mindistance
				{
					mindistance = v2->mindistance;
					v = v2;
				}
			});
		}

		reset_states();

	}
	void minimal_spanning_tree_kruskal(CGraph& mst)
	{
		vector<edge_pair_t> edges;
		get_all_edges(edges);
		//put the edges in the 'ascending order' of weights.
		std::sort(edges.begin(), edges.end(), [](edge_pair_t e1, edge_pair_t e2){
			return e1.weight < e2.weight;
		});

		/*
		for_each(edges.begin(), edges.end(), [](edge_pair_t e){
			cout << e.from->ele << " -> " << e.to->ele << " " << e.weight << endl;
		}); 
		cout << endl; */
		reset_states();
		int componentID = 1;
		/* for all edges (+ till all vertices are added) in the ascending order of 
		their weights, add the edge if it does not cause a cycle (the tricky part).
		Unlike Prim's, Kruskal can grow in disconnected components and later coalsce into
		single spanning tree. Argh! how to merge components?!
		*/
		for(auto e = edges.begin(); e != edges.end(); ++e)
		{
#define SHARED_LABELS 1
			//A = from, B = to
			bool addEdge = true;
#if SHARED_LABELS
			bool isAinTree = (nullptr != e->from->labelTwig);
			bool isBinTree = (nullptr != e->to->labelTwig);
#else
			bool isAinTree = (0 != e->from->time);
			bool isBinTree = (0 != e->to->time);
#endif
			if(!isAinTree && !isBinTree)
			{
				//both vertices are not in the tree - so a new component
#if SHARED_LABELS
				e->from->labelTwig = e->to->labelTwig = make_shared<label_t>();
				e->from->labelTwig->label = componentID;
				//e->from->labelTwig->next = nullptr; <- not required because by default sharedptr is null
#else
				e->from->time = e->to->time = componentID;
#endif
				componentID++;
				mst.add_vertex(e->from->ele);
				mst.add_vertex(e->to->ele);
			}
			else if(isAinTree && !isBinTree)
			{
				//one is from tree, another is not. 
				//update the nontree node's component ID
#if SHARED_LABELS
				e->to->labelTwig = e->from->labelTwig;
#else
				e->to->time = e->from->time;
#endif
				mst.add_vertex(e->to->ele);
			}
			else if(!isAinTree && isBinTree)
			{
#if SHARED_LABELS
				e->from->labelTwig = e->to->labelTwig;
#else
				e->from->time = e->to->time;
#endif
				mst.add_vertex(e->from->ele);
			}
			else //both are in the tree
			{			
#if SHARED_LABELS
				label_ptr_t fromLabel;
				for(fromLabel = e->from->labelTwig; nullptr != fromLabel->next; fromLabel = fromLabel->next);

				label_ptr_t toLabel;
				for(toLabel = e->to->labelTwig; nullptr != toLabel->next; toLabel = toLabel->next);

				if(toLabel->label == fromLabel->label)
#else
				if(e->from->time == e->to->time)
#endif
				{
					//A and B are in the same component, then it's a loop
					addEdge = false;
				}
				else
				{
					//A and B are in different components -- merge them
#if SHARED_LABELS
					fromLabel->next = toLabel->next = make_shared<label_t>();
					fromLabel->next->label = fromLabel->label;
#else
					for(auto v = m_v.begin(); v != m_v.end(); ++v)
					{
						if((*v)->time == (*e).to->time)
							(*v)->time = (*e).from->time;
					}
#endif
				}
			}

			if(addEdge)
			{
				mst.add_edge((*e).from->ele, (*e).to->ele, (*e).weight);
				//cout << (*e).from->ele << " -> " << (*e).to->ele << " " << (*e).weight << endl;
			}

		}
		reset_states();
	}
	int num_vertices()
	{
		return m_v.size();
	}
	vertex_ptr_t get_vertex(graph_element_t c)
	{
		for(auto v = m_v.begin(); v != m_v.end(); ++v)
		{
			if(c == (*v)->ele)
				return (*v);
		}
		return nullptr;
	}
	int get_edge_weight(const vertex_ptr_t& from, const vertex_ptr_t& to)
	{
		for(auto e = from->edges.begin(); e != from->edges.end(); ++e)
		{
			if((*e)->to == to)
				return (*e)->weight;
		}
		return INT_MAX;
	}
	void get_all_edges(vector<edge_pair_t>& e)
	{
		edge_pair_t pair;
		reset_states();
		//edges going in one direction
		for(auto v = m_v.begin(); v != m_v.end(); ++v)
		{
			(*v)->state = VERTEX_STATE_DISCOVERED;
			for(auto ed = (*v)->edges.begin(); ed != (*v)->edges.end(); ++ed)
			{
				if(VERTEX_STATE_UNDISCOVERED == (*ed)->to->state)
				{
					pair.from = (*v);
					pair.to	  = (*ed)->to;
					pair.weight = (*ed)->weight;
					e.push_back(pair);
					//cout << pair.from->ele << " -> " << pair.to->ele << endl;
				}
			}
		}
		reset_states();
	}
	void trace_path(const vertex_ptr_t& from, const vertex_ptr_t& to)
	{
		if(nullptr == to) 
			return;
		//in a recursion, go the oldest parent
		if(from != to)
		{
			trace_path(from, to->parent);			
		}		
		cout << to->ele << " -> " ;
	}

	int find_maximum_flow(graph_element_t s, graph_element_t t)
	{
		CGraph rg(true);//residual netowrk
		init_residual_graph(rg);
		return rg.find_maximum_flow_util(s, t); //to avoid prefixing every method with rg
	}
	int find_maximum_flow_util(graph_element_t s, graph_element_t t)
	{
		//do bfs to find a path from s to t
		while(true == bfs(s, t))
		{
			vertex_ptr_t sink = get_vertex(t);
			//find the minimum capacity on the path
			vertex_ptr_t v = sink;
			int minCap = INT_MAX;
			while(nullptr != v->parent)
			{
				minCap = minCap > v->distFromParent? v->distFromParent: minCap;
				v = v->parent;
			}

			v = sink;
			while(nullptr != v->parent) //maneuver the path
			{
				add_edge_weight(v->parent, v, -1 * minCap); //forward edge for this iter
				add_edge_weight(v, v->parent, minCap); //anti parallel edge for this iter
				v = v->parent;
			}
		}

		vertex_ptr_t source = get_vertex(s);
		int flow = 0;
		//sum the weights of incoming APEs to the source
		for(auto e = source->edges.begin(); e != source->edges.end(); ++e)
		{
			for(auto ape = (*e)->to->edges.begin(); ape != (*e)->to->edges.end(); ++ape)
			{
				if((*ape)->to == source && (*ape)->type == EDGE_ANTIPARALLEL)//APE to the source
					flow += (*ape)->weight;
			}
		}

		print_edges();

		return flow;
	}
	void add_edge_weight(vertex_ptr_t from, vertex_ptr_t to, int change)
	{
		for(auto e = from->edges.begin(); e != from->edges.end(); ++e)
		{
			if((*e)->to == to)
			{
				(*e)->weight += change;
				return;
			}
		}
	}
	void init_residual_graph(CGraph& g)
	{
		for_each(m_v.begin(), m_v.end(), [&g](vertex_ptr_t v)
		{
			g.add_vertex(v);
		});

		for(auto v = m_v.begin(), vr = g.m_v.begin(); v != m_v.end(); ++v, ++vr)
		{
			for(auto ed = (*v)->edges.begin(); ed != (*v)->edges.end(); ++ed)
			{
				//hmm EDGE_TYPE is not necessary because there are no incoming
				//edge to the source, therefore, no need to distinguish the anti parallel
				//edges from likely incoming edges. Similarly no outgoing edges at sink.
				g.add_edge((*v)->ele, (*ed)->to->ele, (*ed)->weight, EDGE_FORWARD);
				//add anti parallel edge as well
				g.add_edge((*ed)->to->ele, (*v)->ele, 0, EDGE_ANTIPARALLEL);
			}
		}
	}
private:
	vector<vertex_ptr_t> m_v; //pointers to vertices	
	pPreProcessVertex  m_pPreProcessVertex;
	pPostProcessVertex m_pPostProcessVertex;
	pProcessEdge m_pProcessEdge;
	bool	m_isVertexStateDirty;

	static void find_path_from_root_util(vertex_ptr_t v)
	{
		if(NULL != v->parent)
			find_path_from_root_util(v->parent);
		cout << v->ele << " ";
	}
	void dfs_util(vertex_ptr_t v)
	{
		v->state = VERTEX_STATE_DISCOVERED;
		v->time = (v->parent != nullptr)? v->parent->time + 1 : 0; 
		
		if(m_pPreProcessVertex) m_pPreProcessVertex(v);
		for(auto it = v->edges.begin(); it != v->edges.end(); it++)
		{
			vertex_ptr_t to = (*it)->to;
			if(m_pProcessEdge) m_pProcessEdge(v, to);
			if(to->state == VERTEX_STATE_UNDISCOVERED)
			{				
				to->parent = v;
				dfs_util(to);
			}
		}

		if(m_pPostProcessVertex) m_pPostProcessVertex(v);
	}
	void reset_states()
	{
		if(false == m_isVertexStateDirty)
			return;
		for(auto it = m_v.begin(); it != m_v.end(); it++)
		{
			(*it)->state = VERTEX_STATE_UNDISCOVERED;
			(*it)->time  = 0;
		}
		m_isVertexStateDirty = false;
	}

};
#pragma region
#elif IMPL == IMPL_SKIENA
#define MAX_NUM_VERTICES 100
typedef struct edge_t_
{
	int to;		//index of the vertex
	int weight;
	struct edge_t_* next; //meta info -- not related to graph
}edge_t;	

typedef struct 
{
	edge_t edges[MAX_NUM_VERTICES]; //array of linked lists of edges
	int	degree[MAX_NUM_VERTICES];
	int nVertices;
	int nEdges;
	bool isDirected;
}graph_t;
/* Notes: 
There is no dedicated memory for storing the vertices content.
The vertices are identified by the array index, such as:
the edges[i] corresponds to edge list of vertex 'i'.
Such indexing allows random access to the vertices, so some 
operations become easier to (compared to using pointers) implement
such as find_path_backto_root of bfs().
Storing the degree of each vertex seems unnecessary.
nVertices tell how many vertices out of MAX_NUM_VERTICES are valid.
Deleting a particular vertex can be painful.
Disconnected parts of the graphs can also be represented
Fixed number of nodes but variable number of edges.
*/
#elif IMPL == IMPL_PITTS
typedef char graph_element_t;
typedef struct vertex_t_
{
	graph_element_t ele;
	//meta info
	bool visited;
	struct edge_t_* edges;	//a linked list of edges
	struct vertex_t_ *next; //meta info -- not related to graph
}vertex_t; 

typedef struct edge_t_
{
	 vertex_t* to; //to which node this edge connects 
	 struct edge_t_ *next; //meta info -- not related to graph
}edge_t;

typedef struct 
{
	vertex_t *vertices; // a linked list of vertices
}graph_t;
/* Note:
Vertices are put in a linked list. This llist has nothing to do with graph sturcture
Each vertex just like Skiena's, contains a llist of edges connected to that vertex.
Each edge node has a pointer to the vertex that it connects to. However, in Skiena's 
each edge node has array index of the vertex it connects to.
*/
#endif
#pragma endregion

void pre_process_vertex(vertex_ptr_t v)
{
	//cout << " -> " << v->ele;
}
/* 
1. Parent node (of DFS) cannot be an articulation vertex (of GRAPH) unless it has more than 1 child.
2. A leaf node (of DFS) is obvioulsy not an articulation vertex (of GRAPH).
3. If a node does not have an ERA (i.e., no backedge from it or its children to
some above vertex), then its bridge-cut art vertex, because deleting it will leave
its children separated from the rest of the graph. In this case a single (tree) edge is holding 
the bridge-cut node with rest of the graph. So the parent of that particular node is also an 
art node.
4. In the above point, the parent was also an art node. A parent can also be an art node even though 
the child is not. Grand children and the immediate child (via some tree edges followed by
a back-edge) of this parent have ERA as this parent node. So deleting the child will not make the 
grand children orphans but deleting the parent node itself will do.
5. A node can be a bridge-cut node as well as a parent cut node.
6. A 'bridge edge' is an edge deleting which disconnects the graph. The bridge edge joins two art vertices.
7. But an edge joining two art vertices need not be a bridge-edge.
Q. How do we define art vertex for a directed graph?
*/
void find_articulation_vertex(vertex_ptr_t v)
{
#if 0
	if(nullptr != v->parent)
		cout << " <- " << v->parent->ele;
#elif 1
	//Note: the root can be deteced in two ways:
	//v->parent is nullptr or v->time is zero

	//This is 'post' processing: all the info about children is available.
	//Root-cut node check:
	if(nullptr == v->parent) //v is root
	{
		if(v->outdegree > 1) //computed in process_edge
			cout << "Root-cut node: " << v->ele << endl;
		return; 
	}

	//Parent-cut node check:
	if((v->parent == v->era) && 
		(v->parent->parent != nullptr)) //v's parent is not root
	{
		//v and its children (the subtree rooted at v) are holding to the parent 
		//with tree and backedges. 
		//The parent is just a parent-cut node is also a bridge cut node when there 
		//are no backedges emanating from the parent.
		//Parent-cut node does not require to disconnect all its subtrees from the 
		//graph; just sufficient to sever one subtree. 
		cout << "Parent-cut node: " << v->parent->ele << endl;
	}

	//Bridge-cut node
	if(v->era == nullptr)
	{
		cout << "bridge-cut node: ";
		if(v->outdegree > 0) //not a leaf
			 cout << v->ele;
		if(v->parent->parent != nullptr) //v->parent is not root
			cout << ", " << v->parent->ele;
		cout << endl;
	}

	//set the era of the "parent"!
	//It is difficult to check if a vetex's child era is earlier than that of itself.
	//So instead, we check the parent's era from the child.
	if(v->parent != nullptr)
	{
		int myEraTime = (v->era == nullptr)? v->time : v->era->time;
		int myParentEraTime = (v->parent->era == nullptr)? v->parent->time 
			: v->parent->era->time;
		v->parent->era = (myEraTime < myParentEraTime)?
			v->era : v->parent->era;
		//Imp: This check also rules out the case where the parent node's ERA is 
		//the parent node itself, which is by definition not an ERA at all.
	}
#endif
}
void find_loops(vertex_ptr_t from, vertex_ptr_t to)
{
	//An amazing property of depth-first search is that all edges fall into these two
	//classes: tree edges or back edges. A back edge connects a vertex only to its ancestor!
	//Below back edges are found by checking this: if the 'to' vertex already discovered
	// and that it is indeed an ancestor to the 'from' vertex.

	//usually from is the parent of to, unless
	if(to->state == VERTEX_STATE_DISCOVERED)
	{
		//so the to node is below the from node 
		if(from->time < to->time)
			return; //backtracking..

		//Now the case where to is 'above' from -- atleast while going deep.
		//found a loop unless to is the from's parent 
		if(to != from->parent) //we need this check because we add each edge twice in case of undirected graphs?
		{
			//A BACK-EDGE!
			//Not all loops are given by this!! For e.g., a loop inside a loop
			//might be skipped. Try Fig 5.15 as undirected and see for yourself
#if 0
			cout << "found a loop from " << from->ele 
				<< " to " << to->ele << endl;			
			for(vertex_ptr_t v = from; v != to; v = v->parent)
			{
				cout << v->ele << " -> " ;
			}
			cout << to->ele << endl;
#endif
			//ERA can be reached by zero or more (downward) tree edges
			//followed by (only) one backedge.
			if(from->era != nullptr && from->era->time < to->time)
			{
				cout << "Had a better ancestor for " << from->ele;
			}
			else
			{
				from->era = to;
			}
		}
	}
	else
	{
		//found a tree edge
		from->outdegree++;
	}
}

vector<graph_element_t> gtopsort;
void topological_sort(vertex_ptr_t v)
{
	//cout << v->ele << " ";
	gtopsort.push_back(v->ele);
}

#define INPUT_STREAM fp
int main(int argc, char** argv)
{
	int nVertices = 0;
	int nEdges = 0;
	char buf[128];
	char *p;
	bool weightedgraph = true;

	CGraph g(true);
	g.clear();

	//printf("Enter number of vertices and edges:\n");
	FILE* fp = fopen("graphdata.txt", "r");
	p = read_line(buf, 127, INPUT_STREAM);
	sscanf(p, "%d %d", &nVertices, &nEdges);
	//printf("Enter %d vertices data:\n", nVertices);

	graph_element_t c;
	for(int v = 0; v < nVertices; v++)
	{
		p = read_line(buf, 127, INPUT_STREAM);
		sscanf(p, "%c", &c);
		g.add_vertex(c);
	}
	g.print_vertices();

	char x, y;
	if(weightedgraph)
	{
		int weight;
		for(int e = 0; e < nEdges; e++)
		{
			p = read_line(buf, 127, INPUT_STREAM);
			sscanf(p, "%c %c %d", &x, &y, &weight);
			g.add_edge(x, y, weight);
		}
	}
	else
	{
		for(int e = 0; e < nEdges; e++)
		{
			p = read_line(buf, 127, INPUT_STREAM);
			sscanf(p, "%c %c", &x, &y);
			g.add_edge(x, y);
		}
	}
	fclose(fp);
	//g.print_edges();

	//g.bfs();
	//g.find_path_backto_root((graph_element_t) '4');
	//g.find_path_from_root((graph_element_t) '4');
	//g.dfs(pre_process_vertex, find_articulation_vertex, find_loops);
	//g.print_eras();
	//g.dfs(NULL, topological_sort, NULL);
	cout << endl;
	//seems to work fine even if DFS requires multiple passes
	/*for(auto it = gtopsort.rbegin(); it != gtopsort.rend(); it++)
	{
		cout << (*it) << " ";
	}
	*/
	/*
	{
		CGraph mst(false);
		//g.minimal_spanning_tree_prim(mst);
		g.minimal_spanning_tree_kruskal(mst);
		mst.print_edges();
		mst.print_vertices();
	}
	*/
	/*
	{
		CGraph shortcut(false);
		g.dijkstra(shortcut);
		shortcut.print_vertices();
	}
	*/

	{
		graph_element_t s = 'A', t = 'F'; 
		int flowValue = g.find_maximum_flow(s, t);
		cout << "Flow of the graph is " << flowValue << endl;
	}

	return 0;
}
#endif