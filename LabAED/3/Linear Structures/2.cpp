#include <iostream>

class Node
{
    public:
    int data;
    Node *next;
};

class LinkedList
{
    private:
    Node *first;
    public:
    LinkedList(){first=NULL;}
    LinkedList(int A[], int n);
    ~LinkedList();

    void Display();
    void Insert(int index, int x);
    int Delete(int index);
    int Length();

};
LinkedList::LinkedList(int A[], int n)
{
    Node *t, *last;
    int i=0;
    first = new Node;
    first->data = A[0];
    first->next = NULL;
    last = first;

    for (int i = 1; i < n; i++)
    {
        t = new Node;
        t->data = A[i];
        t->next = NULL;
        last->next = t;
        last = t;
    }
}   
LinkedList::~LinkedList()
{
    Node *p=first;
    while(p)
    {
        first=first->next;
        delete p;
        p=first;
    }
}
LinkedList::Display()
{
    Node *p=first;
    while(p)
    {
        std::cout<<p->data<<" ";
        p=p->next;
    }
}
int LinkedList::Length()
{
    Node *p=first;
    int len=0;
    while(p)
    {
        len++;
        p=p->next;
    }
    return len;
}
void LinkedList::Insert(int index, int x)
{
    Node *t, *p=first;
    if(index<0 || index>Length())
        return;
    t=new Node;
    t->data=x;
    t->next=NULL;

    if(index==0)
    {
        t->next=first;
        first=t;
    }
    else
    {
        for(int i=0;i<index-1;i++)
            p=p->next;
        t->next=p->next;
        p->next=t;
    }
}
void LinkedList::Delete(int index)
{
    Node *p=first, *q=NULL;
    int x=-1;
    if(index<1 || index>Length())
        return -1;
    if(index==1)
    {
        first=first->next;
        x=p->data;
        delete p;
    }
    else
    {
        for(int i=0;i<index-1;i++)
        {
            q=p;
            p=p->next;
        }
        q->next=p->next;
        x=p->data;
        delete p;
    }
    return x;
}

int main()
{
    int A[]={3,5,7,10,15};
    LinkedList l(A,5);
    l.Display();
    std::cout<<"\nLength: "<<l.Length();
    l.Insert(0,2);
    l.Insert(3,8);
    l.Insert(7,20);
    l.Display();
    std::cout<<"\nDeleted element: "<<l.Delete(1);
    std::cout<<"\nDeleted element: "<<l.Delete(3);
    std::cout<<"\nDeleted element: "<<l.Delete(6);
    l.Display();
}