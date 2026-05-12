#include <iostream>
using namespace std;
class Node
{
    public:
    Node *lchild;
    int data;
    Node *rchild;
};

class Queue
{
    private:
    int front;
    int rear;
    int size;
    int *Q;
    public:
    Queue(int size)
    {
        this->size = size;
        front = rear = 0;
        Q = new int[size];
    }
    Queue(int size){front=rear=-1; this->size=size; Q=new int[size];}
    void enqueue(int x);
    int dequeue();
    void display();
};

void Queue::enqueue(Node *x)
{
    if ((rear + 1) % size == front)
        cout << "Queue is full\n";
    else
    {
        rear = (rear + 1) % size;
        Q[rear] = Node *x;
    }
}

int Queue::dequeue()
{
    Node *x = NULL;
    if (front == rear)
        cout << "Queue is empty\n";
    else
    {
        front = (front + 1) % size;
        Node *x = Q[front];
    }
    return x;
}

void Queue::display()
{
    int i = front + 1;
    do
    {
        cout << Q[i] << " ";
        i = (i + 1) % size;
    } while (i != (rear + 1) % size);
    cout << endl;
}

class Tree
{
    public:
    Node *root;
    Tree(){root=NULL;}
    void Treecreate();
    void preorder(Node *p);
    void inorder(Node *p);
    void postorder(Node *p);
    void levelorder(Node *p);
    void height(Node *p);
};

void Tree::Treecreate()
{
    int x;
    Queue q(100);
    cout << "Enter root value ";
    cin >> x;
    root = new Node;
    root->data = x;
    root->lchild = root->rchild = NULL;
    q.enqueue(root);

    while(!q.isEmpty())
    {
        Node *p=q.dequeue();
        cout << "Enter left child of " << p->data << " ";
        cin >> x;
        if(x!=-1)
        {
            Node *t=new Node;
            t->data=x;
            t->lchild=t->rchild=NULL;
            p->lchild=t;
            q.enqueue(t);
        }
        cout << "Enter right child of " << p->data << " ";
        cin >> x;
        if(x!=-1)
        {
            Node *t=new Node;
            t->data=x;
            t->lchild=t->rchild=NULL;
            p->rchild=t;
            q.enqueue(t);
        }
    }
}

void Tree::inorder(Node *p)
{
    if(p)
    {
        inorder(p->lchild);
        cout << p->data << " ";
        inorder(p->rchild);
    }
}

void Tree::preorder(Node *p)
{
    if(p)
    {
        cout << p->data << " ";
        preorder(p->lchild);
        preorder(p->rchild);
    }
}

void Tree::postorder(Node *p)
{
    if(p)
    {
        postorder(p->lchild);
        postorder(p->rchild);
        cout << p->data << " ";
    }
}

void Tree::levelorder(Node *p)
{
    Queue q(100);
    cout << p->data << " ";
    q.enqueue(p);

    while(!q.isEmpty())
    {
        Node *p=q.dequeue();
        if(p->lchild)
        {
            cout << p->lchild->data << " ";
            q.enqueue(p->lchild);
        }
        if(p->rchild)
        {
            cout << p->rchild->data << " ";
            q.enqueue(p->rchild);
        }
    }
}

void Tree::height(Node *p)
{
    if(p==NULL)
        return 0;
    int x=height(p->lchild);
    int y=height(p->rchild);
    return x>y?x+1:y+1;
}



int main()
{
    Tree t;
    t.Treecreate();
    t.preorder(t.root);
    t.inorder(t.root);
    t.postorder(t.root);
    t.levelorder(t.root);
    t.height(t.root);
    return 0;
}