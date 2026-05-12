#include <stdio.h>
#include <stdlib.h>

struct Node
{
    struct Node *lchild;
    int data;
    struct Node *rchild;
};

struct Queue
{
    int size;
    int front;
    int rear;
    Node *Q;
};
void create(struct Queue *q, int size)
{
    q->size = size;
    q->front = q->rear = 0;
    q->Q = (Node *)malloc(q->size * sizeof(Node));
}

void enqueue(struct Queue *q, Node x)
{
    if ((q->rear + 1) % q->size == q->front)
        printf("Queue is Full\n");
    else
    {
        q->rear = (q->rear + 1) % q->size;
        q->Q[q->rear] = x;
    }
}

void dequeue(struct Queue *q)
{
    if (q->front == q->rear)
        printf("Queue is Empty\n");
    else
    {
        q->front = (q->front + 1) % q->size;
        printf("Deleted Element is %d\n", q->Q[q->front]);
    }
}

int isEmpty(struct Queue q)
{
    return q.front == q.rear;
}