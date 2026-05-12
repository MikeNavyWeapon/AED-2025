#include <stdio.h>
#include <stdlib.h>

struct Node
{
    int data;
    struct Node *next;

}*first=NULL;

void create(int A[], int n)
{
    int i;
    struct Node *t, *last;
    first = (struct Node *)malloc(sizeof(struct Node));
    first->data = A[0];
    first->next = NULL;
    last = first;

    for (int i = 1; i < n; i++)
    {
        t = (struct Node *)malloc(sizeof(struct Node));
        t->data = A[i];
        t->next = NULL;
        last->next = t;
        last = t;
    }
}

void display(struct Node *p)
{
    while (p != NULL)
    {
        printf("%d ", p->data);
        p = p->next;
    }
}

void Rdisplay(struct Node *p)
{
    if (p != NULL)
    {
        printf("%d ", p->data);
        Rdisplay(p->next);
    }
}

int Rcount(struct Node *p)
{
    if (p == NULL)
        return 0;
    else
        return Rcount(p->next) + 1;
}

int sum(struct Node *p)
{
    int s = 0;
    while (p != NULL)
    {
        s += p->data;
        p = p->next;
    }
    return s;
}

int Rsum(struct Node *p)
{
    if (p == NULL)
        return 0;
    else
        return Rsum(p->next) + p->data;
}

int Max(struct Node *p)
{
    int x, max = INT_MIN;
    while (p != NULL)
    {
        x = p->data;
        if (x > max)
            max = x;
        p = p->next;
    }
    return max;
}

int RMax(struct Node *p)
{
    int x, max = INT_MIN;
    if (p == NULL)
        return max;
    x = RMax(p->next);
    return x > p->data ? x : p->data;
}

struct Node *Lsearch(struct Node *p, int key)
{
    while (p != NULL)
    {
        if (key == p->data)
            return p;
        p = p->next;
    }
    return NULL;
}

struct Node *Rsearch(struct Node *p, int key)
{
    if (p == NULL)
        return NULL;
    if (key == p->data)
        return p;
    return Rsearch(p->next, key);
}

void Insert(struct Node *p, int index, int x)
{
    struct Node *t;
    if (index < 0 || index > Rcount(p))
        return;
    t = (struct Node *)malloc(sizeof(struct Node));
    t->data = x;
    if (index == 0)
    {
        t->next = first;
        first = t;
    }
    else
    {
        for (int i = 0; i < index - 1; i++)
            p = p->next;
        t->next = p->next;
        p->next = t;
    }
}

void sortedInsert(struct Node *p, int x)
{
    struct Node *t, *q = NULL;
    t = (struct Node *)malloc(sizeof(struct Node));
    t->data = x;
    t->next = NULL;

    if (first == NULL)
        first = t;
    else
    {
        while (p && p->data < x)
        {
            q = p;
            p = p->next;
        }
        if (p == first)
        {
            t->next = first;
            first = t;
        }
        else
        {
            t->next = q->next;
            q->next = t;
        }
    }
}
int Delete(struct Node *p, int index)
{
    struct Node *q = NULL;
    int x = -1;

    if (index < 1 || index > Rcount(p))
        return;
    if (index == 1)
    {
        q = first;
        first = first->next;
        x = q->data;
        free(q);
    }
    else
    {
        for (int i = 0; i < index - 1; i++)
        {
            q = p;
            p = p->next;
        }
        q->next = p->next;
        x = p->data;
        free(p);
    }
}

int isSorted(struct Node *p)
{
    int x = INT_MIN;
    while (p != NULL)
    {
        if (p->data < x)
            return 0;
        x = p->data;
        p = p->next;
    }
    return 1;
}

void RemoveDuplicates(struct Node *p)
{
    struct Node *q = p->next;
    while (q != NULL)
    {
        if (p->data != q->data)
        {
            p = q;
            q = q->next;
        }
        else
        {
            p->next = q->next;
            free(q);
            q = p->next;
        }
    }
}

void Reverse2(struct Node *p)
{
    struct Node *q = NULL, *r = NULL;
    while (p != NULL)
    {
        r = q;
        q = p;
        p = p->next;
        q->next = r;
    }
    first = q;
}

void Reverse3(struct Node *q, struct Node *p)
{
    if (p != NULL)
    {
        Reverse3(p, p->next);
        p->next = q;
    }
    else
        first = q;
}

void create2(int A[], int n)
{
    struct Node *t, *last;
    first = (struct Node *)malloc(sizeof(struct Node));
    first->data = A[0];
    first->next = NULL;
    last = first;

    for (int i = 1; i < n; i++)
    {
        t = (struct Node *)malloc(sizeof(struct Node));
        t->data = A[i];
        t->next = NULL;
        last->next = t;
        last = t;
    }
}

void Concat(struct Node *p, struct Node *q)
{
    third=p;
    while (p->next != NULL)
        p = p->next;
    p->next = q;
}

void Merge(struct Node *p, struct Node *q)
{
    struct Node *last;
    if (p->data < q->data)
    {
        third = last = p;
        p = p->next;
        last->next = NULL;
    }
    else
    {
        third = last = q;
        q = q->next;
        last->next = NULL;
    }
    while (p && q)
    {
        if (p->data < q->data)
        {
            last->next = p;
            last = p;
            p = p->next;
            last->next = NULL;
        }
        else
        {
            last->next = q;
            last = q;
            q = q->next;
            last->next = NULL;
        }
    }
    if (p) last->next=p;
    if (q) last->next=q;
}

int isLoop(struct Node *f)
{
    struct Node *p, *q;
    p = q = f;
    do
    {
        p = p->next;
        q = q->next;
        q = q ? q->next : q;
    } while (p && q && p != q);
    return p == q ? 1 : 0;
}

int main()
{
    int A[] = {3, 5, 7, 10, 15};
    create(A, 5);
    Rdisplay(first);
    return 0;
}
