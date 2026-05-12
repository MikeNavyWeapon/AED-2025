#include <stdio.h>

struct Node
{
    struct Node *lchild;
    int data;
    struct Node *rchild;
}*root=NULL;

void Insert(int key)
{
    struct Node *t=root;
    struct Node *r=NULL, *p;

    if(root==NULL)
    {
        p=(struct Node *)malloc(sizeof(struct Node));
        p->data=key;
        p->lchild=p->rchild=NULL;
        root=p;
        return;
    }
    while(t!=NULL)
    {
        r=t;
        if(key < t->data)
        {
            t=t->lchild;
        }
        else if(key > t->data)
        {
            t=t->rchild;
        }
        else
        {
            return;
        }
    }
    p=(struct Node *)malloc(sizeof(struct Node));
    p->data=key;
    p->lchild=p->rchild=NULL;

    if(key < r->data)
    {
        r->lchild=p;
    }
    else
    {
        r->rchild=p;
    }
}

void Inorder(struct Node *p)
{
    if(p)
    {
        Inorder(p->lchild);
        printf("%d ", p->data);
        Inorder(p->rchild);
    }
}
struct Node *Search(struct Node *p, int key)
{
    if(p==NULL)
    {
        return NULL;
    }
    if(key==p->data)
    {
        return p;
    }
    else if(key < p->data)
    {
        return Search(p->lchild, key);
    }
    else
    {
        return Search(p->rchild, key);
    }
}
struct Node *RInsert(struct Node *p, int key)
{
    struct Node *t=NULL;
    if(p==NULL)
    {
        t=(struct Node *)malloc(sizeof(struct Node));
        t->data=key;
        t->lchild=t->rchild=NULL;
        return t;
    }
    if(key < p->data)
    {
        p->lchild=RInsert(p->lchild, key);
    }
    else if(key > p->data)
    {
        p->rchild=RInsert(p->rchild, key);
    }
    return p;
}
int Height(struct Node *p)
{
    int x, y;
    if(p==NULL)
    {
        return 0;
    }
    x=Height(p->lchild);
    y=Height(p->rchild);
    return x>y ? x+1 : y+1;
}

struct Node *InSucc(struct Node *p)
{
    while(p && p->lchild!=NULL)
    {
        p=p->lchild;
    }
    return p;
}

struct Node *InPre(struct Node *p)
{
    while(p && p->rchild!=NULL)
    {
        p=p->rchild;
    }
    return p;
}

struct Node *Delete(struct Node *p, int key)
{
    struct Node *q;
    if(p==NULL)
    {
        return NULL;
    }
    if(p->lchild==NULL && p->rchild==NULL)
    {
        if(p==root)
        {
            root=NULL;
        }
        free(p);
        return NULL;
    }
    if(key < p->data)
    {
        p->lchild=Delete(p->lchild, key);
    }
    else if(key > p->data)
    {
        p->rchild=Delete(p->rchild, key);
    }
    else
    {
        if(Height(p->lchild) > Height(p->rchild))
        {
            q=InPre(p->lchild);
            p->data=q->data;
            p->lchild=Delete(p->lchild, q->data);
        }
        else
        {
            q=InSucc(p->rchild);
            p->data=q->data;
            p->rchild=Delete(p->rchild, q->data);
        }
    }
    return p;
}

int main()
{
    Insert(10);
    Insert(5);
    Insert(15);
    Insert(3);
    Insert(7);
    Insert(12);
    Insert(18);

    printf("Inorder Traversal: ");
    Inorder(root);
    printf("\n");

    int key = 7;
    struct Node *result = Search(root, key);
    if(result)
    {
        printf("Element %d found in the tree.\n", key);
    }
    else
    {
        printf("Element %d not found in the tree.\n", key);
    }

    return 0;
}