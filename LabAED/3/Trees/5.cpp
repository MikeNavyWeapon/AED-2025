#include <stdio.h>

struct Node
{
    struct Node *lchild;
    int data;
    struct Node *rchild;
}*root=NULL;

int NodeHeight(struct Node *p)
{
    int x, y;
    if(p==NULL)
    {
        return 0;
    }
    x=NodeHeight(p->lchild);
    y=NodeHeight(p->rchild);
    return x>y?x+1:y+1;
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

struct Node *LLrottation(struct Node *p)
{
    struct Node *pl=p->lchild;
    struct Node *plr=pl->rchild;

    pl->rchild=p;
    p->lchild=plr;

    return pl;
}
struct Node *LRrottation(struct Node *p)
{
    struct Node *pl=p->lchild;
    struct Node *plr=pl->rchild;

    pl->rchild=plr->lchild;
    p->lchild=plr->rchild;

    plr->lchild=pl;
    plr->rchild=p;

    return plr;
}
struct Node *RRrottation(struct Node *p)
{
    struct Node *pr=p->rchild;
    struct Node *prl=pr->lchild;

    pr->lchild=p;
    p->rchild=prl;

    return pr;
}
struct Node *RLrottation(struct Node *p)
{
    struct Node *pr=p->rchild;
    struct Node *prl=pr->lchild;

    pr->lchild=prl->rchild;
    p->rchild=prl->lchild;

    prl->rchild=pr;
    prl->lchild=p;

    return prl;
}
struct Node *LRRotation()
{
    struct Node *pl=p->lchild;
    struct Node *plr=pl->rchild;

    pl->rchild=plr->lchild;
    p->lchild=plr->rchild;

    plr->lchild=pl;
    plr->rchild=p;

    return plr;
}


int main()
{
    root=RInsert(root, 10);
    RInsert(root, 5);
    RInsert(root, 3);

    printf("Height of the tree is %d\n", NodeHeight(root));

    root=LLrottation(root);

    printf("Height of the tree after LL rotation is %d\n", NodeHeight(root));

    return 0;
}