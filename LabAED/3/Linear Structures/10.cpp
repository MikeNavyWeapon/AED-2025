#include <stdio.h>

struct Node {
    int data;
    struct Node* next;
}*front=NULL,*rear=NULL;

void enqueue(int x) {
    struct Node* t = (struct Node*)malloc(sizeof(struct Node));
    if (t == NULL) {
        printf("Queue is full\n");
    } else {
        t->data = x;
        t->next = NULL;
        if (front == NULL) {
            front = rear = t;
        } else {
            rear->next = t;
            rear = t;
        }
    }
}

void dequeue() {
    struct Node* t;
    if (front == NULL) {
        printf("Queue is empty\n");
    } else {
        t = front;
        front = front->next;
        free(t);
    }
}

void display() {
    struct Node* p = front;
    while (p) {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("\n");
}

int main() {
    enqueue(10);
    enqueue(20);
    enqueue(30);

    display();

    dequeue();
    display();

    return 0;
}   