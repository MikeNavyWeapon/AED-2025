#include <stdio.h>

struct Node
{
    int data;
    struct Node *next;
}*Head;

void create(int A[], int n)
{
    int i;
    struct Node *t, *last;
    Head = (struct Node *)malloc(sizeof(struct Node));
    Head->data = A[0];
    Head->next = NULL;
    last = Head;

    for (int i = 1; i < n; i++)
    {
        t = (struct Node *)malloc(sizeof(struct Node));
        t->data = A[i];
        t->next = NULL;
        last->next = t;
        last = t;
    }
}

void Display(struct Node *p)
{
    do
    {
        printf("%d ", h->data);
        h = h->next;
    }while(h!=Head);
    printf("\n");
}

void RDisplay(struct Node *h)
{
    static int flag=0;
    if(h!=Head || flag==0)
    {
        flag=1;
        printf("%d ", h->data);
        RDisplay(h->next);
    }
    flag=0;
}

void Insert(struct Node *p, int index, int x)
{
    struct Node *t;
    int i;
    if (index ==0)
    {
        t = (struct Node *)malloc(sizeof(struct Node));
        t->data = x;
        if(Head==NULL)
        {
            Head=t;
            Head->next=Head;
        }
        else
        {
            while(p->next!=Head)
                p=p->next;
            p->next=t;
            t->next=Head;
            Head=t;
        }
    }
    else
    {
        for(int i=0;i<index-1;i++)
            p=p->next;
        t=(struct Node *)malloc(sizeof(struct Node));
        t->data=x;
        t->next=p->next;
        p->next=t;
    }
}

int Length(struct Node *p)
{
    int len=0;
    do
    {
        len++;
        p=p->next;
    }while(p!=Head);
    return len;
}

int Delete(struct Node *p, int index)
{
    struct Node *q;
    int x=-1;
    if(index<1 || index>Length(p))
        return -1;
    if(index==1)
    {
        while(p->next!=Head)
            p=p->next;
        x=Head->data;
        if(Head==p)
        {
            free(Head);
            Head=NULL;
        }
        else
        {
            p->next=Head->next;
            free(Head);
            Head=p->next;
        }
    }
    else
    {
        for(int i=0;i<index-2;i++)
            p=p->next;
        q=p->next;
        p->next=q->next;
        x=q->data;
        free(q);
    }
    return x;
}

void Reverse(struct Node *p)
{
    struct Node *temp;

    while(p!=NULL)
    {
        temp=p->next;
        p->next=Head;
        Head=p;
        p=temp;
    }
}


int main()
{
    int A[]={3,5,7,10,15};
    create(A,5);
    return 0;
}