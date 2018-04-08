#if 0
#include <stdio.h>
#include <vector>
#include <cassert>
#include <algorithm>
#include <queue>

char *needle = "aabaa"; //then b = {-1, 0, 1, 0, 1, 2}
char *haystack = "aabaabaababaaaabaa";

void kmp_preprocess(int* b, const char* needle)
{
	int needle_length, i, j;
	for(needle_length = 0; needle[needle_length] != '\0'; needle_length++);

	b[0] = -1; //no string: 
	b[1] = 0;  //no proper border

	for(i = 1, j = 0; //compare second element with the first
		i < needle_length; i++ /* j++ cant be here*/)
	{
		//assigning -1 to b[0] helps setting right value of j (= 0) when the loop
		//breaks after no matching/
		//j >= 0: the equality condition is necessary 
		while(j >= 0 && needle[i] != needle[j])
			j = b[j];
		
		//if no match was found, j would be -1 here */
		//The length of the string so far is (i + 1)
		b[i + 1] = ++j; /* j is the border length for i size prefix of needle */
	}

}

void kmp_search(int *b, char * needle, char * haystack)
{
	int needle_length, haystack_length, i, j = 0;
	for(needle_length = 0; needle[needle_length] != '\0'; needle_length++);
	for(haystack_length = 0; haystack[haystack_length] != '\0'; haystack_length++);

	for(i = 0; i < haystack_length; i++)
	{
		while(j >= 0 && haystack[i] != needle[j])
			j = b[j];

		j++;

		if(j == needle_length)
		{
			j = b[j];
			printf("match at index %d, %s\n", i - needle_length + 1, &haystack[i - needle_length + 1]);
		}
	}

}

int* partition(int* begin, int* end, int pivotval)
{
    int* it = begin;
    for (; it != end; ++it)
    {
        if (!(*it < pivotval))
            break;
    }
    if (it == end)
        return end;
    for (int* it2 = it + 1; it2 != end; ++it2)
    {
        if (*it2 < pivotval)
        {
            int temp = *it2;
            *it2 = *it;
            *it = temp;
            ++it;
        }
    }
    return it;
}

void quick_sort(int *begin, int* end)
{
    if (end - begin < 2)
        return;

    int pivotval = *begin;
    auto it = partition(begin + 1, end, pivotval);

    int lastsmall = *(it - 1);
    *(it - 1) = pivotval;
    *begin = lastsmall;

    quick_sort(begin, it - 1); //pivotval is in its right position (it - 1)
    quick_sort(it, end);
    return;
}


std::vector<int> numbers(10);

extern "C"
{
    void c_qsort(int* lo, int* hi);
    struct queue_t;
    queue_t* c_queue();
    void push(queue_t* q, int e);
    int pop(queue_t* q);
    int isempty(queue_t* q);
    int isfull(queue_t* q);
}

void test_c_functions()
{
    if (0)
    {
        int num[1000];

        std::generate(std::begin(num), std::end(num), [](){return rand() % 999; });
        c_qsort(std::begin(num), std::end(num));

        assert(true == std::is_sorted(std::begin(num), std::end(num)));
    }

    if (0)
    {
        std::queue<int> q1;
        auto q2 = c_queue();

        for (int t = 0; t < 100; ++t)
        {
            //do random insertions
            int in = rand() % 256;
            for (int i = 0; i < in && 0 == isfull(q2); ++i)
            {
                int e = rand() % 1024;
                q1.push(e);
                push(q2, e);
            }
            int de = rand() % 256;
            for (int i = 0; i < de && 0 == isempty(q2); ++i)
            {
                int e1 = q1.front();
                q1.pop();
                int e2 = pop(q2);
                assert(e1 == e2);
            }
        }
    }

    
}

int main(int argc, char **argv)
{

    test_c_functions();

	int b[10];
	kmp_preprocess(b, needle);
	kmp_search(b, needle, haystack);
    
    std::generate(numbers.begin(), numbers.end(), [](){return rand() % 99; });
    
    auto n2 = numbers;
    std::sort(begin(n2), end(n2));
   
    quick_sort(numbers.data(), numbers.data() + numbers.size());
    assert(n2 == numbers);

	return 0;
}

#endif


/*
template <typename Iter, typename OutIter>
void merge_sort_recurse(Iter ifirst, Iter ilast, OutIter ofirst)
{
	//how to test for valid range [first, last)?
	auto dist = distance(ifirst, ilast);
	if(1 == dist)
		return;

	auto imid = ifirst;
	advance(imid, dist/2);
	auto omid = ofirst;
	advance(omid, dist/2);

	merge_sort_recurse(ifirst, imid, ofirst);
	merge_sort_recurse(imid, ilast, ofirst);

	//now we have [first, mid) and [mid, last) sorted.
	//merge these into one.

	move_content(ifirst, imid, ofirst);

	auto first1 = ofirst; auto last1 = omid;
	auto first2 = imid; auto last2 = ilast;
	auto dest = ifirst;

	for(; first1 != last1 && first2 != last2; dest++)
	{
		if(*first1 < *first2)
		{
			*dest = move(*first1);
			first1++;
		}
		else
		{
			*dest = move(*first2);
			first2++;
		}
	}

	if(first1 != last1)
	{
		move_content(first1, last1, dest);
	}
	else if(first2 != last2)
	{
		move_content(first2, last2, dest);
	}
}*/