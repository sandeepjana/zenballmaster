#if 0
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <deque>
#include <iterator>
#include <string>
#if USE_ASYNC
#include <thread>
#include <future>
#endif
using namespace std;

char *read_line (char *buf, size_t length, FILE *f);

#define STD_QUICK_SORT 0
#define QUICK_SORT 0
#define COUNTING_SORT 0
#define QUICK_SORT_CSTYLE 0
#define SORT_STRINGS 0
#define MERGE_SORT 1

struct Comp
{
    Comp(int v) : val(v){};
    int val;
    bool operator()(int v)
    {
        return v < val;
    }
};

int* std_find_if_not(int* first, int* last, Comp p)
{
    for (auto it = first; it != last; ++it)
    {
        if (!p(*it))
            return it;
    }

    return last;
}

int* std_partition(int *first, int *last, Comp p)
{
    //skip all the elements at the beginning which 
    //are already meeting the comparison
    first = std_find_if_not(first, last, p);
    if (first == last) 
        return last;

    //'first' now got an element that is !Comp
    //This can be used to swap elements from the remaining range
    
    for (auto it = first + 1; it != last; ++it)
    {
        if (p(*it))
        {
            std::iter_swap(first, it);
            ++first;
        }
    }

    return first;
}

void std_quicksort(int* lo, int* hi)
{
    if (hi - lo < 2) 
        return; //need at least two elements to compare

    Comp less(*lo);
    auto pivot = std_partition(lo + 1, hi, less);
    //pivotit is the iterator to the first element in second group

    //now i dont understand why I need to swap: may be to have a different pivot element
    //in next recursion
    //later added: [lo, pivot) should be strictly "less" than 
    //whatever the element is chosen as pivot -- so needed this swap
    std::iter_swap(lo, pivot - 1);

    std_quicksort(lo, pivot - 1); //strictly less than pivot 
    std_quicksort(pivot, hi); //equal to or greater than pivot
}


#if QUICK_SORT
template <typename T> typename vector<T>::iterator qs_partition(vector<T>& v, typename vector<T>::iterator lo, typename vector<T>::iterator hi)
{	
	//Sorting in ascending order
	//Proceeds by scanning through and collating the smaller elements towards the left side.
	//Whenever a smaller element than the pivot is found, the first bigger element after the chain of smaller elements
	// in the range [lo + 1,...) is swapped so that the chain can be extended.

	//Choose the first element as the pivot
	//Now scan from left to right through the range [lo + 1, hi)
	//Whenever you find a bigger element than the pivot, note this location of LMBE (leftmost bigger element)
	//When you ncounter a smaller element than the pivot after zero or more bigger elements after LMBE,
	//swap this smaller element with LMBE (this operation will extend the chain of smaller elements by one)
	//LMBE location now becomes LMBE + 1;
	//Once the scan is completed, the elements in [lo + 1, hi) are partitioned.
	//Now, to partition [lo, hi), just swap the LMBE with pivot.
	//Return the latest pivot position so that [lo, pivotPos) and [pivotPos + 1, hi) can be processed.

	T pivotval = *lo;
	vector<T>::iterator j = lo; 
	//If you start with j = lo++, and increment j after the swap,
	//then later it becomes cumbersome to swap the pivot element
	//with the 'last element' of the array in case all the elements in 
	//the range [lo + 1, hi) are less than pivot. In such case, j points to 'hi'

	//sorting in ascending order
	for(auto it = lo + 1; it != hi; it++)
	{
		if(*it <= pivotval) //increment j in the quest to find big element
		{			
			j++;
			//j == it would be for many times if the range starts with a chain of smaller elements 
			//Probably, it is good to remove the condition and let identical elements swap
			if(j != it) 
				swap(*j, *it);
		}
		else
		{
			//j wont get incremented so (j + 1) keeps pointing to 
			//the leftmost element bigger than pivot
		}
	}

	swap(*j, *lo);
	return j;
}

template <typename T> void qs_recurse(vector<T>& v, typename vector<T>::iterator lo, typename vector<T>::iterator hi)
{
	if(lo == hi)
		return;

	auto pivotIt = qs_partition(v, lo, hi);
	qs_recurse(v, lo, pivotIt); //sort [lo, pivot)
	// [pivot] is already containing the correct element
	qs_recurse(v, pivotIt + 1, hi); //sort [pivot + 1, hi)
}
#endif

#if QUICK_SORT_CSTYLE
int qs_partition(int* a, int lo, int hi)
{
	int pivot = a[lo];
	int lmbe, t;
	int i;
	for(lmbe = i = lo + 1; i < hi; i++)
	{
		if(a[i] < pivot)
		{
			if(i != lmbe)
			{
				t = a[i];
				a[i] = a[lmbe];
				a[lmbe] = t;
			}
			lmbe++;			
		}
	}

	lmbe--; //the rightmost smaller element than pivot
	t = a[lo];
	a[lo] = a[lmbe];
	a[lmbe] = t;

	return lmbe;
}
void qs_recurse_cstyle(int* a, int lo, int hi)
{
	if(lo == hi)
		return;
	int pIndex = qs_partition(a, lo, hi);
	qs_recurse_cstyle(a, lo, pIndex);
	qs_recurse_cstyle(a, pIndex + 1, hi);
}
#endif

#if COUNTING_SORT
void counting_sort(vector<int>& v, int max)
{
	//1. find histogram of elements of v
	vector<int> ch(max, 0);
	for_each(v.begin(), v.end(), [&ch](int x){
		ch[x]++;
	});

	//2. Then cumulative histogram
	for(auto it = ch.begin() + 1; it != ch.end(); it++)
	{
		*it += *(it - 1);
	}

	//3. ch[v[j] - 1] is the correct position for v[j]
	// because ch[v[j]] many number of elements are smaller than v[j].
    //added later: actually ch[v[j]] are the number of elements smaller or equal to v[j]

	vector<int> s(v.size());
	//for(auto it = v.begin(); it != v.end(); it++) //<-- not stable
	for(auto it = v.rbegin(); it != v.rend(); it++) //<-- stable
	{
		s[ch[*it] - 1] = *it; 
		ch[*it]--; //<-- to handle non-distinct elements
	}

	copy(s.begin(), s.end(), v.begin());

	return;
}
#endif
#if SORT_STRINGS
typedef struct
{
	char* s;
	int c;
	int length;
	int pos;
}string_t;

int qsort_partition(string_t* sInfo, int lo, int hi)
{
	int i;
	int j = lo;
	string_t tmp;
	int pivot = sInfo[lo].c;
	
	for(i = lo + 1; i < hi; i++)
	{
		if(sInfo[i].c <= pivot)
		{
			j++;
			if(j != i)
			{
				//swap
				tmp = sInfo[i];
				sInfo[i] = sInfo[j];
				sInfo[j] = tmp;
			}
		}
	}

	if(sInfo[lo].c != sInfo[j].c)
	{
		tmp = sInfo[lo];
		sInfo[lo] = sInfo[j];
		sInfo[j] = tmp;
	}

	return j;
}
void qsort_string(string_t* sInfo, int lo, int hi)
{
	int pIndex = 0;
	if(lo == hi)
		return;
	pIndex = qsort_partition(sInfo, lo, hi);
	qsort_string(sInfo, lo, pIndex);
	qsort_string(sInfo, pIndex + 1, hi);
}

typedef enum
{
	NO_GROUP,
	START_GROUP,
	IN_GROUP,
	END_GROUP
}CHAR_GROUP_T;

CHAR_GROUP_T is_char_group(string_t* s, int i, int isLast)
{
	int isStart = 0;
	if(i == 0 || s[i - 1].pos != s[i].pos)
		isStart = 1;
	if(isLast || s[i].pos != s[i+1].pos)
		isLast = 1;

	if(isStart && isLast)
		return NO_GROUP;

	if(isStart) //and not last
	{
		return (s[i].c == s[i+1].c)?START_GROUP: NO_GROUP;
	}

	if(isLast) //and not start
	{
		return (s[i-1].c == s[i].c)?END_GROUP:NO_GROUP;
	}

	//somewhere in the middle of same positions
	//where left and right strings (and char positions) are valid
	if(s[i-1].c != s[i].c && s[i].c == s[i+1].c)
		return START_GROUP;

	if(s[i-1].c == s[i].c && s[i].c != s[i+1].c)
		return END_GROUP;

	if(s[i-1].c == s[i].c && s[i].c == s[i+1].c)
		return IN_GROUP;

	return NO_GROUP;
}

void sort_strings(int argc, char** argv)
{
	string_t sInfo[10];
	int pos, group = NO_GROUP, j;

	int i = 0, sorted = 0, lo = -1, hi = 0;
	for(i = 0; i < argc && i < 10; i++)
	{
		sInfo[i].s = argv[i];
		sInfo[i].c = (int) sInfo[i].s[0];
		sInfo[i].length = strlen(sInfo[i].s);
		sInfo[i].pos = 0;
	}

	for(i = 0; i < argc; i++)
	{
		printf("%d %s\n", is_char_group(sInfo, i, (argc - 1) == i), sInfo[i].s);
	}

	//sort based on ith character if pos <= length
	qsort_string(sInfo, 0, argc);
	pos = 0;
#if 0
	while(!sorted)
	{
		sorted = 1;
		lo = -1;
		for(i = 0; i < argc - 1; i++)
		{
			if((sInfo[i].pos == sInfo[i + 1].pos)
				&& (sInfo[i].pos == pos)) //pick only the strings that needed next character for comparison
			{
				//find the same character groups
				if(sInfo[i].c == sInfo[i+1].c 
					&& 0 != sInfo[i].c /* sInfo[i].c = 0 means duplicate string */)
				{
					sorted = 0;
					if(-1 == lo) 
						lo = i;

					sInfo[i].pos++;
					sInfo[i].c = sInfo[i].s[sInfo[i].pos]; //the next char of comparison
				}
				else if(-1 != lo)
				{
					//a character group ended, sort them according their next char
					sInfo[i].pos++;
					sInfo[i].c = sInfo[i].s[sInfo[i].pos]; //the next char of comparison
					qsort_string(sInfo, lo, i + 1);
					lo = -1;
				}
			}
		}		

		if(-1 != lo) //to cater to the case where the last string is in a character group
		{
			sInfo[i].pos++;
			sInfo[i].c = sInfo[i].s[sInfo[i].pos]; //the next char of comparison
			qsort_string(sInfo, lo, i + 1);
			lo = -1;
		}

		pos++;
	}
#else
	while(!sorted)
	{
		sorted = 1;
		for(i = 0; i < argc; i++)
		{
			if(pos != sInfo[i].pos)
				continue;

			group = is_char_group(sInfo, i, (argc - 1) == i);

			if(START_GROUP == group)
			{
				sorted = 0;
				lo = i;
			}
			else if(END_GROUP == group)
			{
				for(j = lo; j < i + 1; j++)
				{
					sInfo[j].pos++;
					sInfo[j].c = sInfo[j].s[sInfo[j].pos]; //the next char of comparison
				}
				qsort_string(sInfo, lo, i + 1);
			}
		}
		pos++;
	}
#endif
	for(i = 0; i < argc && i < 10; i++)
	{
		argv[i] = sInfo[i].s;
	}
}
#endif

template <typename Iter, typename OutIter>
void move_content(Iter ifirst, Iter ilast, OutIter ofirst)
{
	for(; ifirst != ilast; ifirst++, ofirst++)
		*ofirst = move(*ifirst);
}

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

#if USE_ASYNC
	auto fture = async( [&]() {  
		merge_sort_recurse(ifirst, imid, ofirst);
		});
#else
	merge_sort_recurse(ifirst, imid, ofirst);
#endif
	merge_sort_recurse(imid, ilast, omid);

#if USE_ASYNC
	fture.get();
#endif
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
}

//merge sort: take couple of iterators (random access not required, and bi-directional is sufficient. Necessary?)
template <typename Iter>
void merge_sort(Iter first, Iter last)
{
	auto count = distance(first, last);
	deque<typename std::iterator_traits<Iter>::value_type> out; //the temporary space for elements
	//out.resize((count + 1)/2); //the temporary space for elements

	out.resize(count);

	//each element in 'out' has memory requirement same as that of a default constructed element
	merge_sort_recurse(first, last, out.begin());
}

vector<int> v(10);

int main(int argc, char** argv)
{
	int max = 99;
	
	generate(v.begin(), v.end(), [&max](){ return rand() % max;});
	v.push_back(v.back()); //duplicate a couple of elements
	v.push_back(v.front());

#if STD_QUICK_SORT
    std_quicksort(v.data(), v.data() + v.size());
#endif

#if QUICK_SORT
	qs_recurse(v, v.begin(), v.end());
#endif

#if QUICK_SORT_CSTYLE
	qs_recurse_cstyle(&(v[0]), 0, v.size());
#endif

#if COUNTING_SORT
	counting_sort(v, max);
#endif

#if MERGE_SORT
	merge_sort(v.begin(), v.end());
#endif

	for_each(v.begin(), v.end(), [](int x){ cout << x << " ";});
	cout << endl;

	int i, j;
#if SORT_STRINGS
	
	char buf[128];
	char* p;
	char* ptrArray[10];

#if 0
	i = 0;
	while(i < 10)
	{
		p = read_line(buf, 127, stdin);
		if(p == NULL)
			break;
		if(p[0] == 'q' &&  p[1] == 0)
			break;

		ptrArray[i] = (char*) malloc(strlen(p) + 1);
		strcpy(ptrArray[i], p);

		i++;
	}

	sort_strings(i, ptrArray);
#else

	i = 0;
	for(j = 1; j < argc && j < 10; j++)
	{
		ptrArray[j-1] = (char*) malloc(strlen(argv[j]) + 1);
		strcpy(ptrArray[j-1], argv[j]);
		i++;
	}

	sort_strings(i, ptrArray);
#endif

	for(j = 0; j < i; j++)
	{
		printf("%s\n", ptrArray[j]);
	}
#endif

#if MERGE_SORT
	deque<string> strings;
	i = 0;
	for(j = 1; j < argc && j < 10; j++)
	{
		strings.emplace_back(argv[j]);
	}

	merge_sort(strings.begin(), strings.end());
	copy(strings.begin(), strings.end(), ostream_iterator<string>(cout, ", "));
#endif

	return 0;
}

#endif