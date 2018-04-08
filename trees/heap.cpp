#if 0
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

void my_make_heap(vector<int>& v);
void sift_up(vector<int>& v);

int main(int argc, char** argv)
{
	vector<int> v(10);
	generate(v.begin(), v.end(), [](){ return rand() % 99;});

	for_each(v.begin(), v.end(), [](int x){ cout << x << " ";});
	cout << endl;

	vector<int> vcopy(v);

	std::make_heap(vcopy.begin(), vcopy.end());
	for_each(vcopy.begin(), vcopy.end(), [](int x){ cout << x << " ";});
	cout << endl;

	my_make_heap(v);

	for_each(v.begin(), v.end(), [](int x){ cout << x << " ";});
	cout << endl;

	//insert a new element
	v.push_back(100);
	vcopy.push_back(100);

	std::make_heap(vcopy.begin(), vcopy.end());
	for_each(vcopy.begin(), vcopy.end(), [](int x){ cout << x << " ";});
	cout << endl;
#if 0	
	cout << "inserting an element in brute force way" << endl;
	my_make_heap(v);
#else
	sift_up(v);
#endif

	for_each(v.begin(), v.end(), [](int x){ cout << x << " ";});
	cout << endl;

	return 0;
}

int depth_of_heap(int size)
{
	int depth = 0;
	for( ;(size >>= 1); depth++);
	return depth;
}
int start_index(int depth)
{
	return ((1 << depth) - 1);
}

int parent_of(int child)
{
	//if parent has index 'j' (>= 0), its children are 2j + 1 and 2j + 2 
	int parent;
	if(child % 2) /* 2j + 1 */
		parent = (child - 1)/2;
	else
		parent = (child - 2)/2;	

	return parent;
}

void swap(int& a, int& b)
{
	int temp = a;
	a = b;
	b = temp;
}

void sift_up(vector<int>& v)
{
	int parent = parent_of(v.size() - 1);
	int sky = v.back();
	int child = v.size() - 1;

	while(parent >= 0)
	{
		if(v[parent] < v[child])
		{
			swap(v[parent], v[child]);
			child = parent;
			parent = parent_of(parent);			
		}
		else
		{
			break;
		}
	}
}

void sift_down(vector<int>& v, int parent)
{
	int sky = v[parent];
	int s = v.size();
	int le;
	int ri;
	int larger;

	while(1)
	{
		//left and right children
		le = 2*parent + 1; ri = 2*parent + 2;
		if(le < s) 
		{
			larger = le;
		}
		else
		{
			break; // tree ended
		}
		if(ri < s && v[ri] > v[le]) larger = ri;

		//if the larger child is larger than the parent,
		//swap it with the parent <-- root should be the largest
		if(v[larger] > sky)
		{
			//swap
			v[parent] = v[larger];
			parent = larger; //<-- heap property of subtree with root at
			// parent is now to be restored
		}
		else
		{
			break;
		}
	}

	v[parent] = sky;

}
void my_make_heap(vector<int>& v)
{
#if 0
	int d = depth_of_heap(v.size());
	for(int i = d - 1; i >= 0 ; i--)
	{
		int s = start_index(i);
		for(int j = s; j < 2*s + 1 ; j++) //all nodes at level i
			sift_down(v, j);
	}
#endif
	//go to the parent of last node
	
	int lastParent = parent_of(v.size() - 1);
	//from last parent to the root of tree
	for(int i = lastParent; i >= 0 ; i--)
	{
		sift_down(v, i);
	}
}

#endif