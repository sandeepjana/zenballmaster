#if 0
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>

//reference\mincost_perfectmatching.pdf

const int maxsize = 10;


struct MinCostPerfectMatch
{
    int cost[maxsize][maxsize];
    int origcost[maxsize][maxsize];
    int mini[maxsize];
    int size;

    int w2v[maxsize]; //indexed by w;
    int v2w[maxsize]; //indexed by v;

    int parents[2][maxsize]{}; //0 is indexed by v, 1 by w
    std::vector<int> nodes[2];

    int currentpath[maxsize];

    MinCostPerfectMatch()
    {
        nodes[0].reserve(maxsize);
        nodes[1].reserve(maxsize);
        std::fill_n(w2v, maxsize, -1);
        std::fill_n(v2w, maxsize, -1);
    }

    int find_unmatched_v()
    {
        auto vit = std::find_if(v2w, v2w + size, [](int a){
            return -1 == a;
        });
        if (v2w + size == vit)
            return -1;
        return vit - v2w;
    }

    void fill_odd_level()
    {
        //normal BFS on tight edges from this even level
        //if no vertex in next level found, you are stuck
        for (auto v : nodes[0])
        {
            //for given v, find all w with corresponding tight edges
            for (int w = 0; w < size; ++w)
            {
                //parent -1 means not visited
                if (cost[v][w] == 0 && -1 == parents[1][w])
                {
                    nodes[1].push_back(w);
                    parents[1][w] = v;
                }
            }
        }
    }

    int fill_even_level()
    {
        //add next even level vertices from this odd level
        //not just tight edge, but matching edge is required
        //if no such edge is found, then you are lucky -- you found a unmatched w
        for (auto w : nodes[1])
        {
            int v = w2v[w];
            if (-1 != v)
            {
                assert(-1 == parents[0][v]); //parent -1 means not visited
                nodes[0].push_back(v);
                parents[0][v] = w;
            }
            else
            {
                return w;
            }
        }
        return -1;
    }

    void xor_path_with_edge(int v, int w)
    {
        //check if {v, w} is in the current matching
        //if it is there: then 
        if (v == w2v[w])
        {
            //simply remove the edge
            w2v[w] = v2w[v] = -1;
        }
        else //add the edge
        {
            w2v[w] = v, v2w[v] = w;
        }
    }

    //note that path length (number of edges on the path) is always odd
    void update_matchings(int unmatchedv, int unmatchedw)
    {
        //the path info is contained in the parents array
        int w = unmatchedw;
        while (1)
        {
            int v = parents[1][w];
            assert(-1 != v);
            xor_path_with_edge(v, w); //add this path

            if (unmatchedv == v)
                break; //done with the path

            w = parents[0][v];
            assert(-1 != w);
            //the link is already broken above so... should not do anything
            //xor_path_with_edge(v, w); <-- nope
        }
    }

    void update_prices()
    {
        int mini = INT_MAX;
        //first need to find the minimum price
        for (int v = 0; v < size; ++v)
        {
            for (int w = 0; w < size; ++w)
            {
                if (-1 != parents[0][v] && //v is in even level S
                    -1 == parents[1][w]) //w is not in odd level => w is W - N(S)
                {
                    mini = std::min(cost[v][w], mini);
                }
            }
        }

        for (int v = 0; v < size; ++v)
        {
            for (int w = 0; w < size; ++w)
            {
                //note: if v is in even level and w is in odd level, then we dont chage cost of {v, w}
                //if {v, w} does not appear in our levels, then also we dont change the cost.
                //the other two cases:
                if (-1 != parents[0][v] && //v is in even level S
                    -1 == parents[1][w]) //w is not in odd level => w is W - N(S)
                {
                    cost[v][w] -= mini;
                }
                else if (-1 == parents[0][v] && //v is not in even level S
                    -1 != parents[1][w]) //w is in odd level => w is N(S)
                {
                    cost[v][w] += mini;
                }
            }
        }

    }

    bool update_matchings()
    {
        int v = find_unmatched_v();
        if (-1 == v)
            return false;

        bool stuck = false;
        int unmatchedw = -1;

        nodes[0].clear();
        nodes[1].clear();

        nodes[0].push_back(v);        
        std::fill_n(&parents[0][0], 2 * maxsize, -1);
        parents[0][v] = v; //starting node is parent of itself

        while (1)
        {
            fill_odd_level();
            nodes[0].clear();
            if (nodes[1].size() == 0) //nothing found at odd level
            {
                stuck = true;
                break;
            }

            int w = fill_even_level();
            nodes[1].clear();
            //can nodes[0] be empty? when unmatched w is encountered first
            if (-1 != w)
            {
                nodes[1].clear();
                unmatchedw = w;
                break;
            }
        }
  
        if (true == stuck) 
        {
            update_prices();
        }
        else // found good path
        {
            update_matchings(v, unmatchedw);
        }

        return true;
    }

    int get_min_cost()
    {
        int mincost = 0;
        for (int v = 0; v < size; ++v)
        {
            mincost += origcost[v][v2w[v]];
        }
        return mincost;
    }

    void prepare_matrix()
    {
        //save a copy of cost 
        std::copy_n(&cost[0][0], maxsize * maxsize, &origcost[0][0]);

        //process each row
        for (int i = 0; i < size; ++i)
        {
            auto from = &cost[i][0];
            auto it = std::min_element(from, from + size);
            auto minval = *it;
            std::transform(from, from + size, from, [minval](int e)
            {
                return e - minval;
            });
        }

        //process each column
        std::copy_n(&cost[0][0], size, &mini[0]);
        for (int i = 1; i < size; ++i)
        {
            auto from = &cost[i][0];
            std::transform(from, from + size, &mini[0], &mini[0], [](int e, int m)
            {
                return std::min(e, m);
            });
        }
        for (int i = 0; i < size; ++i)
        {
            auto from = &cost[i][0];
            std::transform(from, from + size, &mini[0], from, [](int e, int m)
            {
                return e - m;
            });
        }
    }

};

int main(int, char**)
{
    std::ifstream input("mincost_matching.txt");

    MinCostPerfectMatch m;
    input >> m.size;

    for (int i = 0; i < m.size; ++i)
    {
        for (int j = 0; j < m.size; ++j)
        {
            input >> m.cost[i][j];
        }
    }
    
    m.prepare_matrix();

    while (m.update_matchings());

    auto cost = m.get_min_cost();

    std::cout << cost << "\n";

    return 0;
}
#endif