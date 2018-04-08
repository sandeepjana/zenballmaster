#if 0
#include <stdlib.h>

#define MAXSIZE 128
typedef struct
{
    int a[MAXSIZE];
    int f; //remove
    int r; //insert
    int n;
}queue_t;

queue_t* c_queue()
{
    static queue_t q;
    q.f = q.r = q.n = 0;
    return &q;
}
void push(queue_t* q, int e)
{
    q->a[q->r++ % MAXSIZE] = e;
    ++q->n;
}
int pop(queue_t* q)
{
    --q->n;
    return q->a[q->f++ % MAXSIZE];
}
int isempty(queue_t* q)
{
    return 0 == q->n ? 1 : 0;
}
int isfull(queue_t* q)
{
    return MAXSIZE == q->n ? 1 : 0;
}

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int* c_partition(int* lo, int* hi, int p)
{
    for (; lo != hi; ++lo)
    {
        if (!(*lo < p))
            break;
    }
    if (lo == hi)
        return hi;
    for (int* it = lo + 1; it != hi; ++it)
    {
        if (*it < p)
        {
            swap(lo, it);
            ++lo;
        }
    }
    return lo;
}

void c_qsort(int* lo, int* hi)
{
    if (hi - lo < 2)
        return;

    int* m = c_partition(lo + 1, hi, *lo);

    swap(lo, m - 1);

    c_qsort(lo, m - 1);
    c_qsort(m, hi);
}


#endif