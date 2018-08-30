#if 0
#include <iostream>
#include <iomanip>
#include <deque>
#include <cassert>
#include <algorithm>

//https://e-maxx-eng.appspot.com/data_structures/treap.html
//also look at refereces\fast-set-operations-using-treaps.pdf
/* Treaps are randomized search trees where each node in the tree has a key and
an associated random priority. The keys appear in in-order and the priorities in heap order.

The treap implemented here is a max heap -- numerically largest priority(!) is at root.
*/

struct Node
{
	int key; int priority;
	int count;
	Node *left, *right;
};

#define LEFT(t) (t ? t->left : nullptr)
#define RIGHT(t) (t ? t->right : nullptr)
#define COUNT(t) (t ? t->count : 0)

struct Pair
{
	Node *ltt, *get;
};

int poolcnt = 0;
const int max_ = 5000;
Node pool[max_];

Node* new_node(int key, int priority = 0, int count = 0)
{ 
	Node* n = &pool[poolcnt++];
	n->key = key; n->priority = priority;
	n->count = count;
	n->left = n->right = nullptr;
	return n;  
}
Node* new_node(const Node* t)
{
	Node* n = &pool[poolcnt++];
	*n = *t;
	return n;
}
Node* bst_insert(Node* t, Node* n)
{
	if (!t)
		return n;
	Node* c = t;
	while (c) //walk down till you find a null-child
	{
		t = c;
		c = t->key < n->key ? t->right : t->left;
	}
	return (t->key < n->key) ? t->right = n : t->left = n;
}
Node* bst_insert(Node* t, int key)
{
	if (nullptr == t) 
		return new_node(key);
	Node* c = t;
	while (c != nullptr)
	{
		t = c;
		c = t->key < key ? t->right : t->left;
	}
	return (t->key < key) ? t->right = new_node(key) : 
		t->left = new_node(key);
}

/* Splits the tree t into two tree ltt (less-than-tree) and get (greater-or-equal tree)
based on keys and a given token. Also updates 'count' for each node - the number of elements
in the sub-tree rooted at that node (child count + 1). 
Split is purely based on keys. No operation based on priorities.
*/
void split(Node* t, int token, Node* &ltt, Node* &get)
{
	if (nullptr == t)
		return void(ltt = get = nullptr);
	else if (t->key < token)
		ltt = t, split(t->right, token, ltt->right, get);
	else
		get = t, split(t->left, token, ltt, get->left);

	t->count = (t->left ? t->left->count : 0) + 1
		+ (t->right ? t->right->count : 0);
}

//copies only the minimal stuff: the nodes that are modified
void split_copy(const Node* t, int token, Node* &ltt, Node* &get)
{
	if (!t) return void(ltt = get = nullptr);

	Node* copy = new_node(t);
	if (t->key < token) ltt = copy, split_copy(t->right, token, ltt->right, get);
	else get = copy, split_copy(t->left, token, ltt, get->left);
	
	copy->count = COUNT(copy->left) + 1 + COUNT(copy->right);
}

/* The root of the treap is t, and passed as reference for updation. 
Walk down the treap in BST order as long as priority is satisfied. 
When the priority of n-node is larger than the current t-node, 
split the subtree rooted at t (including t), and assign left and right subtrees to n.
*/
void treap_insert(Node* &t, Node* n)
{
	if (nullptr == t)
		t = n;
	else if (t->priority < n->priority)
		split(t, n->key, n->left, n->right), t = n;
	else
		treap_insert(t->key < n->key ? t->right : t->left, n);
}

void split_debug(Node* t, int token, Node* &ltt, Node* &get)
{
	if (nullptr == t)
		return void(ltt = get = nullptr);
	else if (t->key < token)
	{
		ltt = t;
		int lcbefore = COUNT(t->left);
		int rcbefore = COUNT(t->right);
		split(t->right, token, ltt->right, get);
		assert(lcbefore == COUNT(t->left));
		assert(rcbefore >= COUNT(t->right));
	}
	else
	{
		get = t;
		int lcbefore = COUNT(t->left);
		int rcbefore = COUNT(t->right);
		split(t->left, token, ltt, get->left);
		assert(lcbefore >= COUNT(t->left));
		assert(rcbefore == COUNT(t->right));
	}

	t->count = (t->left ? t->left->count : 0) + 1
		+ (t->right ? t->right->count : 0);
}

Node* tree_copy(const Node* t)
{
	if (nullptr == t)
		return nullptr;
	Node* clone = new_node(t->key, t->count);
	clone->left = tree_copy(t->left);
	clone->right = tree_copy(t->right);
	return clone;
}

void split_nondestructive(const Node* t, int token, Node* &ltt, Node* &get)
{
	Node* tmp; 
	if (nullptr == t)
		return void(ltt = get = nullptr);
	else if (t->key < token)
	{
		tmp = ltt = new_node(t->key, t->count);
		ltt->left = tree_copy(t->left);
		split(t->right, token, ltt->right, get);
	}
	else
	{
		tmp = get = new_node(t->key, t->count);
		get->right = tree_copy(t->right);
		split(t->left, token, ltt, get->left);
	}

	tmp->count = COUNT(tmp->left) + 1 + COUNT(tmp->right);
}

#if 0 // does not have count update :(
Pair split_iterative(Node* t, int token)
{
	Node *t1, *t2;
	Pair p = { nullptr, nullptr };
	while (t)
	{
		if (t->key < token) {
			if (p.ltt == nullptr)
				p.ltt = t1 = t;
			else
				t1->right = t, t1 = t1->right;				
			t = t->right;
		}
		else {
			if (p.get == nullptr)
				p.get = t2 = t;
			else
				t2->left = t, t2 = t2->left;
			t = t->left;
		}			
	}
	t1->right = t2->left = nullptr;
	return p;
}
#endif

Pair split_v2(Node* t, int token)
{
	if (nullptr == t)
		return Pair{ nullptr, nullptr };
	Pair p;
	if (t->key < token)
	{
		p = split_v2(t->right, token);
		t->right = p.ltt;
		p.ltt = t;
	}
	else
	{
		p = split_v2(t->left, token);
		t->left = p.get;
		p.get = t;
	}
	t->count = (t->left ? t->left->count : 0) + 1
		+ (t->right ? t->right->count : 0);
	return p;
}

//pre-condition: ltt and get are two BST such that every element's key 
//in ltt is less than every element's key in get.
//higher (larger) priority is up in the tree
void merge(Node*& t, Node* ltt, Node* get)
{
	if (!ltt || !get)
		return void(t = ltt ? ltt : get);
	if (ltt->priority < get->priority)
		t = get, merge(get->left, ltt, get->left);
	else
		t = ltt, merge(ltt->right, ltt->right, get);

	t->count = (t->left ? t->left->count : 0) + 1
		+ (t->right ? t->right->count : 0);
}

Node* merge_copy(Node* ltt, Node* get)
{
	if (nullptr == ltt) return get;
	if (nullptr == get) return ltt;

	Node* t;
	if (ltt->priority < get->priority)
		t = new_node(get), t->left = merge_copy(ltt, get->left);
	else
		t = new_node(ltt), t->right = merge_copy(ltt->right, get);

	t->count = COUNT(t->left) + 1 + COUNT(t->right);
	return t;
}

void split_strict(Node* t, int token, Node*& ltt, Node*& gtt)
{
	if (nullptr == t)
		return void(ltt = gtt = nullptr);
	if (t->key < token) //go right
		ltt = t, split_strict(t->right, token, t->right, gtt);
	else if (t->key > token)
		gtt = t, split_strict(t->left, token, ltt, gtt->left);
	else
		ltt = t->left, gtt = t->right; //TODO: free t
	
	t->count = COUNT(t->left) + 1 + COUNT(t->right);
}

Node* unite(Node* l, Node* r) {
	if (!l || !r)  return l ? l : r;
	if (l->priority < r->priority)  std::swap(l, r);
	//now l has higher priority <-- up in the heap
	Node* lt, *rt;
	split_strict(r, l->key, lt, rt);
	l->left = unite(l->left, lt);
	l->right = unite(l->right, rt);
	return l;
}


const int line_length = 128;
int line[line_length];

void print_line()
{
	for (int i = 0; i < line_length; i++)
		if (line[i] < 0) std::cout << "  ";
		else std::cout << std::setw(2) << line[i];
}

template<class T>
void print(Node* tree, T f)
{
	std::deque<Node*> q;
	q.push_back(tree);
	int len = (1 << 7);
	for (int level = 0; level < 5; level++)
	{
		len = len / 2;
		int gap = len; 
		std::fill_n(line, line_length, -1);
		for (int i = 0; i < (1 << level); ++i)
		{
			Node* n = q.front();
			q.pop_front();
			q.push_back(LEFT(n)), q.push_back(RIGHT(n));
			int pos = (gap / 2) + i * gap - 1;
			line[pos] = n ? f(n) : -1;
		}
		print_line();
		std::cout << '\n';
	}
}

void print(Node* tree)
{
	print(tree, [](Node* n) { return n->key; });
}

void update_count(Node* t)
{
	if (t == nullptr)
		return;
	update_count(t->left);
	update_count(t->right);
	t->count = (t->left ? t->left->count : 0) + 1 
		+ (t->right ? t->right->count : 0);
}

void print_count(int numnodes)
{
	for (int i = 0; i < numnodes; ++i)
		std::cout << pool[i].key << " " << pool[i].count << ", ";
	std::cout << '\n';
}

struct Gen
{
	int c = -1;
	int numnodes = 0;
	Gen& operator>>(int& i)
	{
		if (-1 == c) i = 10;
		else if (0 == c) i = numnodes = 1 + (rand() % 5);
		else i = rand() % 5;

		c = (c + 1) % (1 + 2 * numnodes);
		return *this;
	}
};
Gen tg;

#define INPUT std::cin
//#define INPUT tg

//inorder traversal should give a sorted array
std::deque<int> sorted;
template<class T>
void inorder(const Node* t, T fun)
{
	if (nullptr == t) return;
	inorder(t->left, fun);
	fun(t);
	inorder(t->right, fun);
}

int check_count(const Node* t)
{
	if (nullptr == t) return 0;
	return check_count(t->left) + 1 + check_count(t->right);
}
void test_bst(Node* t, int numnodes)
{
	sorted.clear();
	inorder(t, [](const Node* t) { sorted.push_back(t->key); });
	assert(sorted.size() == numnodes);
	assert(is_sorted(begin(sorted), end(sorted)));
}
#define PRIOR(t) (t ? t->priority : 0)
void test_treap(Node* t, int numnodes)
{
	sorted.clear();
	inorder(t, [](const Node* t) {
		assert(PRIOR(t->left) <= t->priority && PRIOR(t->right) <= t->priority);
		sorted.push_back(t->key);
	});
	assert(sorted.size() == numnodes);
	assert(is_sorted(begin(sorted), end(sorted)));
}
void test_count(int numnodes)
{
	for (int i = 0; i < numnodes; i++)
		assert(pool[i].count == check_count(&pool[i]));
}
void test_count(Node* t)
{
	inorder(t, [](const Node* a) {
		assert(COUNT(a) == COUNT(a->left) + 1 + COUNT(a->right));
	});
}
void test_split(Node* ltt, Node* get, int token, int numnodes)
{
	test_count(numnodes);
	assert(numnodes == COUNT(ltt) + COUNT(get));
	test_bst(ltt, COUNT(ltt)); assert(COUNT(ltt) ? sorted.back() < token : true);
	test_bst(get, COUNT(get)); assert(COUNT(get) ? token <= sorted.front() : true);
}
void test_nondestructive_split(Node* ltt, Node* get, int token, int numnodes)
{
	auto countcheck = [](const Node* t) {
		assert(t->count == check_count(t));
	};
	inorder(ltt, countcheck);
	inorder(get, countcheck);
	assert(numnodes == COUNT(ltt) + COUNT(get));
	test_bst(ltt, COUNT(ltt)); assert(COUNT(ltt) ? sorted.back() < token : true);
	test_bst(get, COUNT(get)); assert(COUNT(get) ? token <= sorted.front() : true);
}
void test_equal(const Node* a, const Node* b)
{
	if (nullptr == a || nullptr == b)
		return void(assert(nullptr == a && nullptr == b));

	test_equal(a->left, b->left);
	assert(a->key == b->key && a->priority == b->priority);
	test_equal(a->right, b->right);
}

int main(int, char**)
{
	freopen("trees/treap-input.txt", "r", stdin);

	using namespace std;
	int T;
	INPUT >> T;
	for (int tc = 0; tc < T; tc++)
	{
		poolcnt = 0;
		int numnodes;
		INPUT >> numnodes;
		assert(numnodes > 0);
		Node *tree = nullptr;
		for (int i = 0; i < numnodes; i++)
		{
			int key, priority;
			INPUT >> key >> priority;
			//if (nullptr == tree) tree = bst_insert(nullptr, key);
			//else bst_insert(tree, key);
			
			//if (nullptr == tree) tree = bst_insert(nullptr, new_node(key, priority));
			//else bst_insert(tree, new_node(key, priority));

			treap_insert(tree, new_node(key, priority));
			//ref_insert(tree, new_node(key, priority));
		}

		//test_bst(tree, numnodes);
		//print(tree);
		print(tree, [](Node* a) {return a->priority; });
		test_treap(tree, numnodes);

		//VERY IMPORTANT: update count before split!
		update_count(tree);
		//print(tree, [](Node* a) {return a->count; });
		test_count(numnodes);
		//print_count(numnodes);

		int token = 6;
		Node* ltt, *get;
#if 0
		split_debug(tree, token, ltt, get);
		print(ltt);
		print(get);
		test_split(ltt, get, token, numnodes);
		if(ltt) test_treap(ltt, ltt->count);
		if(get) test_treap(get, get->count);
#elif 0
		Pair p = split_v2(tree, token);
		ltt = p.ltt, get = p.get;
		//print(p.ltt);
		//print(p.get);
		test_split(ltt, get, token, numnodes);
#elif 0
		split_nondestructive(tree, token, ltt, get);
		test_nondestructive_split(ltt, get, token, numnodes);
#elif 0
		Pair p = split_iterative(tree, token);
		ltt = p.ltt, get = p.get;
		print(p.ltt);
		print(p.get);
		test_split(ltt, get, token, numnodes);
#elif 0
		print(tree);
		cout << "-----------------------------------\n";
		split_copy(tree, token, ltt, get);
		//print(tree);
		//print(ltt);
		//print(get);
#endif

#if 0
		Node* combined;
		merge(combined, ltt, get);
		test_treap(combined, combined->count);
		test_count(combined);
		//print(combined);
#elif 0
		auto combined = merge_copy(ltt, get);
		test_treap(combined, numnodes);
		test_count(combined);
		test_equal(tree, combined);
#endif

		Node* clone = tree_copy(tree);
		print(tree);
		cout << "-----------------------------------\n";

		Node* unique = unite(tree, clone);
		print(unique);
		test_treap(unique, numnodes);
		//print(tree, [](Node* a) {return a->priority; });
		//return 0;

		//print_count(numnodes);

	}

	return 0;
}
#endif