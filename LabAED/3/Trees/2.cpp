#include <stdio.h>

void Treecreate()
{
    struct Node *p, *t;
    int x;
    struct Queue q;
    create(&q, 100);
    printf("Enter root value ");
    scanf("%d", &x);
    root = (struct Node *)malloc(sizeof(struct Node));
    root->data = x;
    root->lchild = root->rchild = NULL;
    enqueue(&q, *root);

    while(!isEmpty(q))
    {
        p=dequeue(&q);
        printf("Enter left child of %d ", p.data);
        scanf("%d", &x);
        if(x!=-1)
        {
            t=(struct Node *)malloc(sizeof(struct Node));
            t->data=x;
            t->lchild=t->rchild=NULL;
            p->lchild=t;
            enqueue(&q, *t);
        }
        printf("Enter right child of %d ", p->data);
        scanf("%d", &x);
        if(x!=-1)
        {
            t=(struct Node *)malloc(sizeof(struct Node));
            t->data=x;
            t->lchild=t->rchild=NULL;
            p->rchild=t;
            enqueue(&q, *t);
        }
    }
}
void inorder(struct Node *p)
{
    if(p)
    {
        inorder(p->lchild);
        printf("%d ", p->data);
        inorder(p->rchild);
    }
}
void preorder(struct Node *p)
{
    if(p)
    {
        printf("%d ", p->data);
        preorder(p->lchild);
        preorder(p->rchild);
    }
}

void IPreorder(struct Node *p)
{
    struct Stack s;
    create(&s, 100);
    while(p || !isEmpty(s))
    {
        if(p)
        {
            printf("%d ", p->data);
            push(&s, p);
            p = p->lchild;
        }
        else
        {
            p = pop(&s);
            p = p->rchild;
        }
    }
}

void IInorder(struct Node *p)
{
    struct Stack s;
    create(&s, 100);
    while(p || !isEmpty(s))
    {
        if(p)
        {
            push(&s, p);
            p = p->lchild;
        }
        else
        {
            p = pop(&s);
            printf("%d ", p->data);
            p = p->rchild;
        }
    }
}

void Levelorder(struct Node *p)
{
    struct Queue q;
    create(&q, 100);
    printf("%d ", p->data);
    enqueue(&q, *p);

    while(!isEmpty(q))
    {
        p=dequeue(&q);
        if(p->lchild)
        {
            printf("%d ", p->lchild->data);
            enqueue(&q, *p->lchild);
        }
        if(p->rchild)
        {
            printf("%d ", p->rchild->data);
            enqueue(&q, *p->rchild);
        }
    }
}

int count(struct Node *p)
{
    if(p)
    {
        return count(p->lchild) + count(p->rchild) + 1;
    }
    return 0;
}
int height(struct Node *p)
{
    if(p==NULL)
        return 0;
    int x=height(p->lchild);
    int y=height(p->rchild);
    return x>y?x+1:y+1;
}


int main()
{
    Treecreate();
    printf("Preorder Traversal is ");
    preorder(root);
    return 0;
}