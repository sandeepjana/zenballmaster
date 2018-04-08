#if 0
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHUNK_SIZE 256
struct array_node_;
typedef struct array_node_
{
	int e[CHUNK_SIZE];
	struct array_node_* next;
}array_node_t;

typedef struct
{
	array_node_t* node;
	array_node_t* cachedNode;
	int	cachedBank;
}int_array_t;

array_node_t* new_array_node()
{
	array_node_t* node = (array_node_t*) malloc(sizeof(array_node_t));
	node->next = NULL;
	return node;
}
int_array_t* new_int_array()
{
	int_array_t* a = (int_array_t *) malloc(sizeof(int_array_t));
	a->cachedBank = 0;
	a->cachedNode = a->node = new_array_node();
	return a;
}

void set(int_array_t* a, int i, int e)
{
	int j;
	array_node_t* node = a->node;
	int bank = i/CHUNK_SIZE;
	if(bank == a->cachedBank)
	{
		node = a->cachedNode;
	}
	else
	{
		for(j = 0; j < i/CHUNK_SIZE; node = node->next = (node->next ? node->next : new_array_node()), j++);
		a->cachedNode = node;
		a->cachedBank = bank;
	}

	node->e[i % CHUNK_SIZE] = e;
}
int get(int_array_t* a, int i)
{
	int j;
	array_node_t* node = a->node;
	int bank = i/CHUNK_SIZE;
	if(bank == a->cachedBank)
	{
		node = a->cachedNode;
	}
	else
	{
		for(j = 0; j < bank; node = node->next, j++);
		a->cachedNode = node;
		a->cachedBank = bank;
	}

	return node->e[i % CHUNK_SIZE];
}
void delete_int_array(int_array_t* a)
{
	array_node_t* node = a->node;
	while(node != NULL)
	{
		array_node_t* next = node->next;
		free(node);
		node = next;
	}
}

typedef struct
{
	int_array_t* a;
	int numElements;
}stack_t;

stack_t* new_stack(void)
{
	stack_t* s= (stack_t*) malloc(sizeof(stack_t));
	s->a = new_int_array();
	s->numElements = 0;
	return s;
}
void stack_push(stack_t* s, int e)
{
	set(s->a, s->numElements, e);
	s->numElements++;
}
int stack_back(stack_t* s, int e)
{
	return get(s->a, s->numElements - 1);
}
int stack_is_empty(stack_t* s)
{
	return (0 == s->numElements);
}
void stack_pop_back(stack_t* s)
{
	s->numElements--;
}
void delete_stack(stack_t* s)
{
	delete_int_array(s->a);
	free(s);
}

typedef struct
{
	int* a;
	int size;
	int readIndex;
	int writeIndex;
	int numElements;
}queue_t;

queue_t* new_queue(int size)
{
	queue_t* q = (queue_t *) malloc(sizeof(queue_t));
	q->a = (int *) malloc(size);
	q->size = size;
	q->readIndex = 0;
	q->writeIndex = 0;
	q->numElements = 0;
	return q;
}
int queue_push_back(queue_t* q, int e)
{
	if(q->numElements == q->size)
	{
		return -1;
	}

	q->numElements++;
	q->a[q->writeIndex] = e;
	q->writeIndex++;
	if(q->writeIndex == q->size)
	{
		q->writeIndex = 0;
	}

	return  0;
}
int queue_pop_front(queue_t* q)
{
	int e;
	if(0 == q->numElements)
		return -1;

	e = q->a[q->readIndex];

	q->numElements--;
	q->readIndex++;
	if(q->readIndex == q->size)
	{
		q->readIndex = 0;
	}

	return e;
}
void delete_queue_t(queue_t* q)
{
	free(q->a);
	free(q);
}

#define TEST_SIZE 10*1024*1024
int test[TEST_SIZE];
#define PROFILE_IMPL 0
int main(int argc, char** argv)
{
	int i;
	int numfails = 0;
	stack_t* s;
#if PROFILE_IMPL
	int_array_t* a;
#else
	int* a;
#endif
	for(i = 0; i < TEST_SIZE; i++)
	{	
		test[i] = rand() % 99;
	}

	printf("Ready...\n");
#if PROFILE_IMPL
	a = new_int_array();
#else
	a = (int*) malloc(TEST_SIZE*sizeof(int));
#endif
	for(i = 0; i < TEST_SIZE; i++)
	{
#if PROFILE_IMPL
		set(a, i, test[i]);
#else
		a[i] = test[i];
#endif
	}
	for(i = 0; i < TEST_SIZE; i++)
	{
#if PROFILE_IMPL
		if(get(a, i) != test[i]) 
#else
		if(a[i] != test[i])
#endif
			numfails++;
	}
#if PROFILE_IMPL
	delete_int_array(a);
#else
	free(a);
#endif
	if(numfails) 
		printf("FATAL %d\n", __LINE__);

	return 0;
}

#endif