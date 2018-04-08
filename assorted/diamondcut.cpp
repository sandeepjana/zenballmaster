#if 0
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>

#define TWO_DIM 0
const int CS = 4;

#if TWO_DIM
const int dimensions = 2;
int cube[CS][CS];
#else
const int dimensions = 3;
int cube[CS][CS][CS];
#endif

struct tuple_t
{
    int d[dimensions];
};

struct meta_t
{
    int id;    
    tuple_t ori;
    tuple_t dim;

    int parent;
    int child;
};

std::ostream& operator<<(std::ostream& os, const meta_t& m)
{
    //for (int i = 0; i < dimensions; ++i)
    //    os << m.ori.d[i] << " " << m.ori.d[i] + m.dim.d[i] << ", ";

    for (int i = 0; i < dimensions; ++i)
        os << m.ori.d[i] << " ";
    os << "; ";
    for (int i = 0; i < dimensions; ++i)
        os << m.ori.d[i] + m.dim.d[i] - 1 << " ";

    os << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const tuple_t& p)
{
    for (int i = 0; i < dimensions; ++i)
        os << p.d[i] << " ";

    os << std::endl;
    return os;
}

int freenode = 0;
const int maxnodes = 1000000;
meta_t metas[maxnodes];

tuple_t operator-(tuple_t t1, tuple_t t2)
{
    for (int i = 0; i < dimensions; ++i)
    {
        t1.d[i] -= t2.d[i];
    }

    return t1;
}

tuple_t get_extreme(tuple_t ori, tuple_t dim)
{
    for (int i = 0; i < dimensions; ++i)
    {
        ori.d[i] += (dim.d[i] - 1);
    }

    return ori;
}

tuple_t operator+(tuple_t t1, tuple_t t2)
{
    for (int i = 0; i < dimensions; ++i)
    {
        t1.d[i] += t2.d[i];
    }

    return t1;
}

tuple_t operator/(tuple_t t1, int n)
{
    for (int i = 0; i < dimensions; ++i)
    {
        t1.d[i] /= n;
    }

    return t1;
}


bool is_point_inside(tuple_t ori, tuple_t dim, tuple_t p)
{
    auto dist = p - ori;
    bool flag = true;
    for (int i = 0; i < dimensions; ++i)
    {
        //if any of dimension is zero, then the cuboid does not exist
        flag = flag && (0 != dim.d[i]) && 
            (dist.d[i] >= 0 && dist.d[i] <= dim.d[i]);
    }
    return flag;
}

void add_node(int id, tuple_t p)
{
    auto& m = metas[id];

    if (false == is_point_inside(m.ori, m.dim, p))
        return;

    //if m is not a leaf node, we can traverse down for a tighter cuboid
    if (-1 != m.child)
    { 
        //there are four or six children, laid contiguously
        for (int i = 0; i < 2 * dimensions; ++i)
        {
            if (metas[m.child + i].parent != m.id)
                break;
            add_node(m.child + i, p);
        }

        return;
    }

    //here comes the leaf node

    //each line (or plane) parallel to axes and going through p divides
    // the parent cuboid into two cuboids.
    //so there are 2 * dimension cuboids to consider
    //Half of those cuboids have ori as corner

    m.child = freenode; //id of the first child 

    auto dist = p - m.ori;

    std::cout << "parent " << m;
    std::cout << "point " << p;

    for (int i = 0; i < dimensions; ++i)
    {
        //|+|x|+|+| dist = 1, ori = 0, m.dim = 4
        // half1 ori = m.ori
        // half2 ori = m.ori + dist + 1
        // half2 dim = m.dim - (dist + 1)

        if (0 != dist.d[i])
        {
            meta_t half;
            half.ori = m.ori;
            half.dim = m.dim;
            half.dim.d[i] = dist.d[i];
            half.id = freenode++;
            half.child = -1;
            half.parent = m.id;

            assert(maxnodes > freenode);
            metas[half.id] = half;
            std::cout << half;
        }

        if (m.dim.d[i] > (dist.d[i] + 1))
        {
            meta_t half;
            half.dim = m.dim;
            half.dim.d[i] -= (dist.d[i] + 1); //can be zero: cuboid half does not exist
            half.ori = m.ori;
            half.ori.d[i] += (dist.d[i] + 1);


            half.id = freenode++;
            half.child = -1;
            half.parent = m.id;

            assert(maxnodes > freenode);

            metas[half.id] = half;
            std::cout << half;
        }
    }

    std::cout << "\n";
}

void debug_check_containments()
{
    for (int j = 0; j < freenode; ++j)
    {
        if (-1 != metas[j].child)
            continue;
        for (int i = 0; i < freenode; ++i)
        {
            if (-1 != metas[i].child)
                continue;

            if (is_point_inside(metas[j].ori, metas[j].dim, metas[i].ori)
                && is_point_inside(metas[j].ori, metas[j].dim, get_extreme(metas[i].ori, metas[i].dim)))
                assert("gotcha");
        }
    }
}

int find_volume()
{
    int nleafs = 0;
    int nnulls = 0;
    int maxv = 0;
    for (int i = 0; i < freenode; ++i)
    {
        auto& m = metas[i];
        if (-1 == m.child)
        {
            ++nleafs;
            int v = 1;
            for (int j = 0; j < dimensions; ++j)
            {
                v *= m.dim.d[j];
            }

            maxv = maxv > v ? maxv : v;

            if (v == 0) ++nnulls;
        }
    }
    
    std::cout << "nleafs " << nleafs << "\n";
    std::cout << "nnulls " << nnulls << "\n";

    return maxv;
}

#if TWO_DIM
int test(int cube[CS][CS])
#else
int test(int cube[CS][CS][CS])
#endif
{
    tuple_t ori{};
    tuple_t dim;
    for (int i = 0; i < dimensions; ++i)
        dim.d[i] = CS;

    meta_t m;
    m.id = freenode++;
    m.child = -1;
    m.parent = -1;
    m.ori = ori;
    m.dim = dim;
   
    metas[0] = m;

#if TWO_DIM
    for (int y = 0; y < CS; ++y)
    {
        for (int x = 0; x < CS; ++x)
        {
            if (0 == cube[y][x])
            {
                tuple_t t;
                t.d[0] = y;
                t.d[1] = x;
                add_node(0, t);
                std::cout << "added\n";
            }
        }
    }
#else
    for (int z = 0; z < CS; ++z)
        for (int y = 0; y < CS; ++y)
            for (int x = 0; x < CS; ++x)
                if (0 == cube[z][y][x])
                {
                    tuple_t t;
                    t.d[0] = z;
                    t.d[1] = y;
                    t.d[2] = x;
                    add_node(0, t);
                }

#endif

    int vol = find_volume();

    std::cout << vol << std::endl;

    return 0;
}

#if TWO_DIM
int main(int, char**)
{
    srand(0);

    for (int y = 0; y < CS; ++y)
        for (int x = 0; x < CS; ++x)
            cube[y][x] = 1;

    cube[1][2] = 0;
    cube[2][1] = 0;
    cube[0][0] = 0;
    cube[3][3] = 0;

    test(cube);

    return 0;
}
#else
int main(int, char**)
{
    srand(0);

    for (int z = 0; z < CS; ++z)
        for (int y = 0; y < CS; ++y)
            for (int x = 0; x < CS; ++x)
                cube[z][y][x] = (rand() % CS == 0) ? 0 : 1;


    test(cube);

    debug_check_containments();

    return 0;
}
#endif
#endif