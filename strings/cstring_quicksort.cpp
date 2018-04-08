#if 0
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

typedef const char* cstring_t;

cstring_t strings[] = { "amar", "akbar", "anthony" };

void swap(cstring_t* one, cstring_t* two)
{
    cstring_t s = *one;
    *one = *two;
    *two = s;
}

bool compare_less(cstring_t a, cstring_t b)
{
    int i = 0;
    for (; a[i] == b[i] && a[i] != 0; ++i);

    return (a[i] < b[i]) ? true : false;
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

void test_my_string_sort()
{
    const int numstrings = 100;
    char sarray[numstrings][32] = {};
    cstring_t strings[numstrings] = {};

    std::vector<std::string> vs;

    for (int i = 0; i < numstrings; ++i)
    {
        scanf("%s", sarray[i]);
        strings[i] = sarray[i];
        vs.push_back(strings[i]);
    }

    cstring_t* begin = strings;
    cstring_t* end = strings + numstrings;

    quick_sort(begin, end);
    std::sort(vs.begin(), vs.end());

    for (int i = 0; i < numstrings; ++i)
    {
        std::cout << strings[i] << " ";
        assert(strings[i] == vs[i]);
    }

    return;
}

int main(int, char**)
{
    //test_my_string_sort();
    //return 0;

    cstring_t* begin = strings;
    cstring_t* end = strings + sizeof(strings) / sizeof(cstring_t);

    quick_sort(begin, end);

    for (auto it = begin; it != end; ++it)
    {
        std::cout << *it << " ";
    }


    return 0;
}
#endif