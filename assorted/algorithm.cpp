#if 0
#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <cassert>

template<typename It>
It my_unique(It first, It last)
{
    if (first == last)
        return last; 

    auto replace = first; 
    //The invariant: ++replace is the position where a new unique element will be copied to.

    while (++first != last) //breaks immediately for one element array
    {
        //initially replace and first are neighbors: replace|first
        //as long as content of replace and first are different,
        //replace is incremented to be equal to first

        if (*replace != *first)
        {
            ++replace; //try to catch up with first

            if (replace != first) //whoa! replace did not catch up with first
            {
                //replace is different from first means, we have some copying to do
                *replace = *first;
            }
        }

    }

    return ++replace; //return past-the-end iterator
}


template <class ForwardIt>
void my_rotate(std::vector<int>& v, ForwardIt first, ForwardIt n_first, ForwardIt last)
{
    auto _first = first;
    ForwardIt next = n_first;
    while (first != next) { //first == next means nothing more to swap

        std::cout << std::distance(_first, first) << " "
            << std::distance(_first, next) << "\n";

        std::iter_swap(first, next);
        ++first;
        ++next;
        if (next == last) {
            next = n_first;
        }
        else if (first == n_first) {
            n_first = next;
        }
    }
}

template <class ForwardIt>
void my_rotate_v2(std::vector<int>& v, ForwardIt first, ForwardIt n_first, ForwardIt last)
{
    //swap element it1 : [first ...] with it2: [n_first ...]
    //the swapping pauses when either it2 reaches last, 
    //leaving elements (the dots) from it1 to n_first unswapped : [xxxx it1 ... n_first yyyy it2=last]
    //or it1 reaches n_first, 
    //leaving elements from it2 to last unswapped [xxxxx it1=n_first xxxx it2 ..... last]

    /* In first case,  the ... should be swapped with yyy
    e.g.[0, 1, 2, 3, 4, 5 = n_first, 6]
    after swap: [5, 6, 2 = it1, 3, 4, 0 = n_first, 1], it2 = last
    after swap again after setting it2 to n_first: [5, 6, 0, 1, 4 = it1, 2 = n_first, 3], it2 = last
    after swap again after setting it2 to n_first: 
    step 1: [5, 6, 0, 1, 2, 4 = it1 = n_first, 3 = it2] 
    Here it1 reached n_first - the second case
    You can continue swapping but now n_first becomes it2
    After modification [5, 6, 0, 1, 2, 4 = it1, 3 = it2 = n_first] 
    

    */

}

template <class ForwardIt>
void my_rotate_recurse(std::vector<int>& v, ForwardIt first, ForwardIt n_first, ForwardIt last)
{
    auto _first = first;
    ForwardIt next = n_first;

    if (first == next)
        return;

    while (1)
    {
        std::iter_swap(first, next);
        ++first;
        ++next;
        if (next == last) { //elements [first, n_first] are still not swapped
            //n_first remains the same
            my_rotate_recurse(v, first, n_first, last);
            break;
        }
        else if (first == n_first) {
            my_rotate_recurse(v, first, next, last);
            break;
        }
    }
}

template<typename It, typename T>
It my_remove(It first, It last, const T& value)
{
    if (first == last) return last;

    auto replace = first;
    while (++first != last)
    {
        if (*replace != val)
        {
            ++replace;
        }
    }

    return replace;
}

template<typename It, typename T>
It my_lower_bound(It first, It last, T val)
{
    //break the loop when first == last
    //iteratively modify first and last
    //first should point to the first element in second group

    //you can either stick to using first and last everwhere or
    //first and 'n' everywhere.

    //invariant:
    //n is last - first 
    //if the key exists in initial list, then the first occurance of it will exist in [first, last)
    //else the first occurance of element greater than key will exist in [first, last)

    //in other words, the first occurance of element e such that !(e < val) will exist in [first, last)
    for (auto n = std::distance(first, last); 0 != n;)
    {
        n = n / 2;
        auto mid = std::next(first, n);
        if (*mid < val) //not --> val < *mid
        {
            first = std::next(mid);
            n = std::distance(first, last);
        }
        else
        {
            last = mid;
        }
    }

    return first;
}


int main(int, char**)
{

    //std::vector<int> v{ 1, 1, 2, 2, 3, 3, 3, 6, 7 };
    //auto vcopy = v;

    //v.erase(my_unique(v.begin(), v.end()), v.end());
    //vcopy.erase(std::unique(vcopy.begin(), vcopy.end()), vcopy.end());

    //v.erase(my_remove(v.begin(), v.end(), 3), v.end()); 
    //vcopy.erase(std::remove(vcopy.begin(), vcopy.end(), 3), vcopy.end());

    //assert(v == vcopy);

#if 0
    std::vector<int> v = { 0, 1, 2, 3, 4, 5, 6, 7 };
    my_rotate_recurse(v, v.begin(), v.begin() + 3, v.end());

    std::copy(v.begin(), v.end(), std::ostream_iterator<int> { std::cout, ", " });
#endif

    //for (int test = 0; test < 10000; ++test)
#if 0
    {
        int size = rand() % 100;
        std::vector<int> v(size);
        std::generate(begin(v), end(v), []() { return rand() % 100;  });

        std::sort(begin(v), end(v));

        auto val = -1; // rand() % 200;
        auto it1 = std::lower_bound(begin(v), end(v), val);
        auto it2 = my_lower_bound(begin(v), end(v), val);

        assert(it1 == it2);
    }
#endif

    int arr[] = { 5 };
    auto it = my_lower_bound(std::begin(arr), std::end(arr), 5);


    return 0;
}
#endif