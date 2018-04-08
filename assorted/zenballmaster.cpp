#if 0
#include <set>
#include <iostream>
#include <string>
#include <sstream> //split
#include <vector>
#include <map>
#include <iterator> //for ostream_iterator and back_inserter
#include <algorithm>
#include <assert.h>
#include <sstream> //for stringstream
#include <functional>
#include <regex>
#include <bitset>

//Sequence Containers (vector, deque, list) and Associative Containers (Set, Multiset, Map, Multimap).

#define PRINT(v, T) do{ copy(v.begin(), v.end(), ostream_iterator<T>(cout, ",")); cout << endl; } while(0)
#define CHECK_DUPLICATE(it) do{if(false == it.second) cout << "duplicate key " << endl;} while(0)

using namespace std;
//strings
int main(int argc, char** argv)
{
#if 0
	//strings
	{
		string welcome;
		string banner;
		//string smallBanner;
		welcome = "zen ball master says hello to ";
	
		banner = welcome + (argc > 1 ? argv[1] : "anonymous") + "\n";
		//cout << welcome << argv[1] << endl;
		cout << banner;

		string BANNER = banner;

		std::transform(banner.begin(), banner.end(), BANNER.begin(), std::ptr_fun<int, int>(std::toupper));
		cout << BANNER ;

		/* To perform tranformation in-place:
		std::transform(banner.begin(), banner.end(), banner.begin(), toupper);
		cout << banner;
		Note: since transform does not guarantee the order of evaluation (unlike the for_each) 
		among the elements, the third argument should not be anything other than begin().
		*/

		string smallBanner(BANNER, 0, 4);
		cout << smallBanner << endl;

		string hello("hello");
		//make a string out of welcome from the pos of "hello" to the end
		string smallBanner2(welcome, welcome.find(hello), welcome.size());
		smallBanner2 += (argc > 1 ? argv[1] : "anonymous");
		cout << smallBanner2 << endl;

		//TWO VERSIONS OF "replace"
		//replace E with 3 in BANNER
		std::replace(BANNER.begin(), BANNER.end(), 'E', '3');
		cout << BANNER;

		string sAppend("; have a good time");
		BANNER.replace(BANNER.size() - 1, sAppend.size(), sAppend);
		cout << BANNER << endl;

		//insert -- inserts without overwriting
		string dear("dear ");
		//works: string::iterator it = std::find(BANNER.begin(), BANNER.end(), 'g' );
		//Crash coz no error check: BANNER.insert(BANNER.find("GUEST"), dear);
		size_t sizeOff = BANNER.find("GU3ST"); //find does not return iterator!
		if(sizeOff == string::npos)
		{
			cout << "Couldnt find the string" << endl;
		}
		else
		{
			BANNER.insert(sizeOff, dear);			
			cout << BANNER << endl;
		}

		//returns the position of the first character equal to *one of characters* in input string
		sizeOff = BANNER.find_first_of("ABC");
		if(sizeOff == string::npos)
		{
			cout << "Couldnt find the character" << endl;
		}
		else
		{
			BANNER.insert(sizeOff, "!");			
			cout << BANNER << endl;
		}
	}
#endif

#if 0
	//algorithm: copy
	{
		int myInts[] = {1,2,3,4};
		vector<int> v1(myInts, myInts + 4 /* sizeof(myInts)/sizeof(int) */);
		//printing a vector
		//Method 1
		for(size_t i = 0; i < v1.size(); i++) cout << v1[i] << ", ";
		cout << endl;
		//v1[i] does not throw exception, at() does.
		//Method 2
		for(vector<int>::iterator i = v1.begin(); i != v1.end(); i++) cout << *i << ", ";
		cout << endl;
		//Method 3
		//#include <iterator> for ostream_iterator
		copy(v1.begin(), v1.end(), ostream_iterator<int>(cout, ", "));
		cout << endl;

		//copy a v1 to a vector that has not enough memory for overwriting
		vector<int> v2(1, 0); //one int with value zero
		//This will crash
		//copy(v1.begin(), v1.end(), v2.end());
		copy(v1.begin(), v1.end(), back_inserter(v2)); //#include <iterator>
		copy(v2.begin(), v2.end(), ostream_iterator<int>(cout, ", "));
		cout << endl;

		//Then there are remove_copy_if: ignore while copying those for which the predicate is true
		//replace_copy_if, replace_if (inplace replacement)

		//A function object (or functor) that returns a boolean is a PREDICATE
		//Below uses a lambda expression for predicate
		remove_copy_if(v1.begin(), v1.end(), ostream_iterator<int>(cout, ", "), [](int i){return i < 3;});

	}
#endif

#if 0
	//algorithm continued
	{

	}
#endif

#if 0
	//count and reverse
	{
		string myStr("zen ball master says hello");

		int d = count_if(myStr.begin(), myStr.end(), [](char x){ return x != ' '; });
		cout << d << endl;

		vector<char> v1;
		std::copy(myStr.begin(), myStr.end(), back_inserter(v1));

		copy(v1.begin(), v1.end(), ostream_iterator<char>(cout, ","));
		cout << endl;

		int c = count_if(v1.begin(), v1.end(), [](char x){ return x != ' '; });
		cout << c << endl;

		string myRevStr = "Intro ";
		std::reverse_copy(myStr.begin(), myStr.end(), back_inserter(myRevStr));
		cout << myRevStr << endl;
		//std::reverse does the op in-place

		std::rotate(myStr.begin(), myStr.begin() + 10, myStr.end());
		cout << myStr << endl;
	}

	//
	{
		string my("zenball");
		std::random_shuffle(my.begin(), my.end());
		cout << my << endl;
		//Scan from right, stop at the partition 'p' where
		//letters to the right are in the descending order for the longest seq.
		//This permutation of size 's' marks the end of the permutations possible 
		//by the letters in that sequence after the 'p' partition.
		//So need to add a new letter to the bunch, and that new letter is obviously
		//the nearest left letter 'n' to the sequence (by nature of dictionary).
		//Now the first permutation of these 's + 1' letters starts with 
		//the letter near to and greater than 'n'. The remaining letters, including 'n', 
		//are arranged in ascending order.
		std::next_permutation(my.begin(), my.end());
		cout << my << endl;

	}

	//partition
	{
		//partition is also available
		vector<int> numbers(10); //some 10 numbers
		std::generate(numbers.begin(), numbers.end(), [](){return rand()%99;});
		copy(numbers.begin(), numbers.end(), ostream_iterator<int>(cout, ","));
		cout << endl;
		//separate even and odd
		stable_partition(numbers.begin(), numbers.end(), [](int n){return !(n%2);});
		copy(numbers.begin(), numbers.end(), ostream_iterator<int>(cout, ","));
		cout << endl;
	}

	//string.find and std::find are different
	//search: for string, string.find will do the 'search'
	{
		//i want to find position of '8' in a vector.
		string csv = "1,2,3,4,5,6,7,8,9";
		std::stringstream ss(csv);

		vector<int> v;
		int i;
		while(ss >> i) //read one integer
		{
			v.push_back(i);
			if(',' == ss.peek())
			{
				ss.ignore();
			}
		}

		copy(v.begin(), v.end(), ostream_iterator<int>(cout, ", "));

		vector<int>::iterator iter = std::find(v.begin(), v.end(), 8);
		cout << *iter << endl;

		//make the vector bigger
		vector<int> vcopy = v;
		random_shuffle(vcopy.begin(), vcopy.end());
		v.insert(v.end(), vcopy.begin(), vcopy.end());
		random_shuffle(vcopy.begin(), vcopy.end());
		v.insert(v.end(), vcopy.begin(), vcopy.end());
		copy(v.begin(), v.end(), ostream_iterator<int>(cout, ", "));		

		vector<int> match;
		match.push_back(5); match.push_back(3);
		auto it = find_first_of(v.begin(), v.end(), match.begin(), match.end());
		cout << endl << *it << endl;

		cout << *find_first_of(it + 1, v.end(), match.begin(), match.end()) << endl;

		vector<int> m2;
		m2.push_back(2); m2.push_back(7); m2.push_back(3); m2.push_back(1);
		it = search(v.begin(), v.end(), m2.begin(), m2.end());
		if(it != v.end())
			cout << endl << "(1) search string is found at: " << it - v.begin() << endl;

		m2.clear();
		random_shuffle(v.begin(), v.end());
		PRINT(v, int);
		copy(v.begin() + 5, v.begin() + 8, back_inserter(m2));
		it = search(v.begin(), v.end(), m2.begin(), m2.end());
		PRINT(m2, int);
		if(it != v.end())
			cout << "(2) search string is found at: " << it - v.begin() << endl;

	}

	//min and max
	{
		vector<int> v(10); //some 10 numbers;
		generate(v.begin(), v.end(), [](){ return 50 - (rand() % 99);});
		PRINT(v, int);
		cout << endl;

		auto it = min_element(v.begin(), v.end(), [](int x, int y){ return abs(x) < abs(y);});
		cout << *it  << endl;
	}
#endif

#if 0
	//removing elements
	{
		//remove the minimum value
		vector<int> v(10);
		generate(v.begin(), v.end(), [](){ return (rand() % 99);});
		PRINT(v, int);
		cout << endl;

		cout << "Min value is " << *min_element(v.begin(), v.end()) << endl;
		v.erase(std::remove(v.begin(), v.end(), *min_element(v.begin(), v.end())), v.end());
		PRINT(v, int); 

		vector<int> vorig(v);

		//removing duplicates in a vector
		vector<int> vcopy = v;
		random_shuffle(vcopy.begin(), vcopy.end());
		v.insert(v.end(), vcopy.begin(), vcopy.end());
		//chop the vector just to make sure below predicate is returning required bool
		v.pop_back();v.pop_back();v.pop_back();
		PRINT(v, int); cout << endl;

		set<int> s;
		/*
		vector<int> nodups;
		remove_copy_if(v.begin(), v.end(), back_inserter(nodups), 
			[&s](int x){ 
				return !s.insert(x).second; //-> .second false means already exists
		} ); 
		PRINT(nodups, int); cout << endl;
		*/

		vector<int> vunique(v);

		//in-place removal of duplicate elements
		auto iter = remove_if(v.begin(), v.end(), 
			[&s](int x){ 
				return !s.insert(x).second; //-> .second false means already exists
		});
		v.erase(iter, v.end());
		PRINT(v, int);
		
		unique(vunique.begin(), vunique.end()); //unique works well only on sorted sequences.

		/* 'equal' crashes when the comparing vectors are not of same length */
		if(equal(vorig.begin(), vorig.end(), v.begin()) && v.size() == vorig.size())
			cout << "yay" << endl;

		if(equal(vorig.begin(), vorig.end(), vunique.begin()) && v.size() == vunique.size())
		{
		}
		else
			cout << "lesson: unique removes only adjacent duplicates!" << endl;		

		/* 'mismatch' returns the iterator for the position of mismatch */

	}
#endif

#if 0
	//sort -- works on containers that support random iterators: vector and deque
	{
		//stable_sort
		//sorting is done in place.
		vector<int> v(10);
		generate(v.begin(), v.end(), [](){ return rand()%99; });
		PRINT(v, int);

		vector<int> vsorted(v);
		
		stable_sort(vsorted.begin(), vsorted.end());
		PRINT(vsorted, int);

		//searching the 4th smallest element
		vector<int> vtemp(v);
		//cool n_th element: good for finding medians
		//what is the fourth smallest element?
		nth_element(vtemp.begin(), vtemp.begin() + 4 - 1, vtemp.end()); //modifies vtemp
		int fourthSmallest = vtemp[4 -1];

		//now search: 
		//on unsorted stuff
		auto it = find(v.begin(), v.end(), fourthSmallest);
		cout << "4th smallest is at the index " << it - v.begin() << endl;

		//on sorted stuff
		auto posToInsert = lower_bound(vsorted.begin(), vsorted.end(), fourthSmallest);
		//must test the existence
		if(*posToInsert == fourthSmallest)
			cout << "4th smallest is at " << posToInsert - vsorted.begin() + 1 
			<< ". wat a realization." << endl;
		else
			cout << fourthSmallest << " could can be inserted at" << 
				posToInsert - vsorted.begin() + 1 << "without disturbing the order" << endl;

		//binary_search just returns a bool -- whether the element exists; not the position :(

		//CAUTION: WORKS ONLY ON SORTED RANGES
		//merge and inplace_merge for merging sorted ranges

		//CAUTION: WORKS ONLY ON SORTED RANGES
		//equal_range returns a pair of iterators which point to just before and just after the 
		//given value in the sorted sequence
		v.resize(4);
		generate(v.begin(), v.end(), [](){ return rand()%99; });
		sort(v.begin(), v.end());
		PRINT(v, int);
		//insert a repeated middle element
		vector<int> repeated;
		int ele = (v[1] + v[2])/2;
		fill_n(back_inserter(repeated), 3, ele);
		//insert from just before the iterator pointed by the first argument
		//so that inserted sequence begins at the location pointed by the iterator.
		//v.begin() points to the 0th element or 1st position
		//v.begin() + 5 points to the 6th position
		//so there are 6 elements before the inserted sequence
		v.insert(v.begin() + 2, repeated.begin(), repeated.end());
		copy(v.begin(), v.end(), ostream_iterator<int>(cout, "\t"));
		auto pair = equal_range(v.begin(), v.end(), ele);
		std::cout << "\n" << *pair.first << ", " << *pair.second << "\n";
		fill_n(ostream_iterator<char>(cout), pair.first - v.begin(), '\t');
		std::cout << "|\n";
		fill_n(ostream_iterator<char>(cout), pair.second - v.begin(), '\t');
		std::cout << "|\n";
	}

	//set operations: only on sorted 
	{
		vector<int> vtmp(10);
		generate(vtmp.begin(), vtmp.end(), [](){ return rand()%99; });
		vector<int> v(vtmp);		
		stable_sort(v.begin(), v.end());
		PRINT(v, int);

		vector<int> vsub(v.begin() + 2, v.begin() + 5);
		PRINT(vsub, int);

		if(true == includes(v.begin(), v.end(), vsub.begin(), vsub.end()))
			cout << "vsub is indeed a subset of v" << endl;

		//set intersection
		int a[] = {10,20,30,40,50}; //already sorted
		int b[] = {20, 25, 40}; //sorted
		vector<int> intersec(5); //big enough
		int c[5]= {0};

		auto it = set_intersection(a, a + sizeof(a)/sizeof(int), 
			b, b + sizeof(b)/sizeof(int), intersec.begin());
		intersec.resize( it - intersec.begin()) ;
		PRINT(intersec, int);
		
		auto it2 = set_intersection(a, a + sizeof(a)/sizeof(int), 
			b, b + sizeof(b)/sizeof(int), c);
		copy(c, it2, ostream_iterator<int>(cout, ","));

		//similarly set difference
	}

	//heap operations
	//stack, queue and priority queue are 'abstract data types'
	//while heap is a 'data structure'
	//Unlike BST, there is no implied ordering between the siblings in a heap
	{
		vector<int> v(10);
		generate(v.begin(), v.end(), [](){ return rand()%99; });
		PRINT(v, int);

		make_heap(v.begin(), v.end()); //heapify
		PRINT(v, int);

		pop_heap(v.begin(), v.end());
		PRINT(v, int);
		cout << "popped element is " << v[v.size() - 1] << endl;
		//heap becomes invalid here if we consider the entire range
		//re-make the heap -- SO NOT REQUIRED, see below for loop
		//make_heap(v.begin(), v.end() - 1);

		pop_heap(v.begin(), v.end() - 1);
		cout << "popped element is " << v[v.size() - 2] << endl;

		push_heap(v.begin(), v.end() - 1);
		PRINT(v, int);
		push_heap(v.begin(), v.end());
		PRINT(v, int);
		//now a full element heap is back.

		//include more
		v.push_back(101);	//push into vector
		push_heap(v.begin(), v.end()); //heapify including the new last element
		PRINT(v, int);

		/*
		cout << "printing in the sorted order" << endl;
		make_heap(v.begin(), v.end()); //heapify
		PRINT(v, int);
		int size = v.size();
		for(int i = 0; i < size; i++)
		{
			pop_heap(v.begin(), v.end() - i);
			cout << v[size - 1 - i] << endl;
		}
		*/
	}


	//all_of, any_of, none_of
	{
		vector<int> v(10, 5);
		//check if all of the elements are 5 }
		if(all_of(v.begin(), v.end(), [](int x) { return x == 5;}))
			cout << "yay" << endl;
		else
			cout << "uh oh" << endl;
	}

	//making a map from vectors
	{
		int key[] = {1,2,3,4,4};
		string value[] = {"creating ", "a ", "map ", "from ", "vectors "};
		vector<int> v1;
		copy(key, key + sizeof(key)/sizeof(int), back_inserter(v1));
		PRINT(v1, int);

		vector<string> v2;
		//copy(value, value + sizeof(value)/sizeof(char *), back_inserter(v2));
		copy(value, value + 5, back_inserter(v2));
		PRINT(v2, string);

		map<int, string> m;
		assert(v1.size() == v2.size());
		//no hassle way
		for(size_t i = 0; i < v1.size(); i++)
		{
			if(false == m.insert(make_pair(v1[i], v2[i])).second)
				cout << v1[i] << " already existed as the key" << endl;
				//insertion does not happen in such cases 
		}

		for(auto it = m.begin(); it != m.end(); it++)
		{
			cout << "{ " << it->first << ", " << it->second << " }" << endl;
		}	
	}
#endif

#if 0
	//erase
	{
		vector<int> v(10);
		generate(v.begin(), v.end(), [](){return rand()%99;});
		PRINT(v, int);
		//let's erase 5th element
		auto it = v.begin() + 5;
		v.erase(it);
		PRINT(v,int);
		v.push_back(22);v.push_back(22);v.push_back(22);
		random_shuffle(v.begin(), v.end());
		PRINT(v, int);
		//let erase all occurances of 22: erase-remove idiom
		// v.erase(remove(v.begin(), v.end(), 22), v.end()); 
		// OR
		v.erase(remove_if(v.begin(), v.end(), [](int x){ return 22 == x;}), v.end());
		//CAUTION: Dont forget v.end() as the second argument. 
		PRINT(v, int);


		//associative containers dont have erase based on predicate
		//create a map
		map<int, char> m;
		m.clear();
		CHECK_DUPLICATE(m.insert(make_pair(10, 'A')));
		CHECK_DUPLICATE(m.insert(make_pair(11, 'B')));
		CHECK_DUPLICATE(m.insert(make_pair(12, 'C')));
		CHECK_DUPLICATE(m.insert(make_pair(13, 'D')));
		//erase key 11
		m.erase(11); //this is based on key 
		//OR
		//m.erase(m.find(11)); //based on iterator
		//CAUTION: 
		//auto it = m.find(11);
		//m.erase(it); 
		//Now 'it' is invalid after erase!!

		//suppose you want to simulate
		//m.erase(m.find(11), m.end()); by looping through
		//then iterator should be post-incremented
		for(auto i = m.find(11); i != m.end(); )
			m.erase(i++);

		for(auto i = m.begin(); i != m.end(); i++)
				cout << i->first << " -> " << i->second << endl;


	}
#endif

#if 0
	//converting vector of integers to a string
	//all the time you did PRINT to cout. Now do it for stringstream
	{
		vector<int> v(5);
		generate(v.begin(), v.end(), [](){return rand()%99;});
		PRINT(v, int);

		stringstream ss;
		copy(v.begin(), v.end(), ostream_iterator<int>(ss, " "));
		string s = ss.str();

		cout << s << endl;
	}
#endif

#if 0
	{
        std::string s("abc");
        s.push_back('\0');
        s.append("def");
        std::cout << "The string is " << s << ", the size is " << s.size() << "\n";
	}
#endif

#if 1
    {
        //printing in hex format
        //for (int i = 0; i < 16; ++i)
        //{
        //    char s[4];
        //    sprintf(s, "%0*x", 3, i); //remember 0 comes first
        //    printf("%s\n", s);
        //}

        

        std::vector<std::pair<std::string, int>> vp;
        vp.push_back({ "101", 0 });
        vp.push_back({ "111", 0 });
        int n = 0;
        const int bitlen = 4;
        int numseq = std::pow(2, bitlen);
        for (int i = 0; i < numseq; ++i)
        {
            //print numbers in binary with fixed number of bits
           auto bs = std::bitset<bitlen>(i);
           std::cout << bs.to_string() << endl;

           std::string s = bs.to_string();
           bool hasAtleast = false;
           for (auto& p : vp)
           {
               if (string::npos != s.find(p.first))
               {
                   ++p.second;
                   hasAtleast = true;
               }
                   
           }

           if (hasAtleast) ++n;
           
        }
        for (auto& p : vp)
        {
            std::cout << "num strings without " << p.first << " : " << (numseq - p.second) << std::endl;
        }
        std::cout << "num strings without any of above:" << (numseq - n) << endl;

    }
#endif

	return 0;
}

#endif