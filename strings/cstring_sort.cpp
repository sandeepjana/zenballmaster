#if 0
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

//typedef const char* cstring_t;
struct cstring_t
{
    const char* s;
    int id;
};

const int numstrings = 5;
const char* strings[] = { "amar", "akbar", "anthony", "anthony", "abhijeet" };


void swap(cstring_t* one, cstring_t* two)
{
    cstring_t s = *one;
    *one = *two;
    *two = s;
}

bool compare_less(cstring_t a, cstring_t b)
{
    int i = 0;
    for (; a.s[i] == b.s[i] && a.s[i] != 0; ++i);

    /*if (a.s[i] == b.s[i])
        return a.id < b.id;*/

    return (a.s[i] < b.s[i]) ? true : false;
}

cstring_t* partition(cstring_t* begin, cstring_t* end, cstring_t pivot)
{
    //find if not
    //find the first element which is "not less than" pivot
    cstring_t* it;
    for (it = begin; it != end; ++it)
    {
        //compare returns true when (first < second)
        if (!compare_less(*it, pivot))
        {
            break;
        }
    }

    //if every string is less than pivot
    cstring_t* fi2g = it;
    if (fi2g == end)
        return fi2g;

    for (it = fi2g + 1; it != end; ++it)
    {
        if (compare_less(*it, pivot))
        {
            swap(it, fi2g);
            ++fi2g; //point to element which is "not less than" pivot
        }
    }

    return fi2g;
}

void quick_sort(cstring_t* begin, cstring_t* end)
{
    //termination condition
    if (end - begin < 2)
        return;

    cstring_t pivot = *begin;
    //get all elements strictly less than pivot to left hand side of 
    //firstIn2ndGroup
    cstring_t* firstIn2ndGroup = partition(begin + 1, end, pivot);

    //
    swap(begin, firstIn2ndGroup - 1);
    firstIn2ndGroup = firstIn2ndGroup - 1;
    

    quick_sort(begin, firstIn2ndGroup); //recall where end points to.
    quick_sort(firstIn2ndGroup + 1, end);
}

int main(int, char**)
{
    cstring_t records[numstrings] = {};
    for (int i = 0; i < numstrings; ++i)
    {
        records[i].id = i;
        records[i].s = strings[i];
    }

    cstring_t* begin = records;
    cstring_t* end = records + numstrings;

    quick_sort(begin, end);

    for (auto it = begin; it != end; ++it)
    {
        std::cout << it->id << " " << it->s << '\n';
    }


    return 0;
}
#endif