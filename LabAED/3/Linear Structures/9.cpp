#include <iostream>
using namespace std;

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

void Queue::enqueue(int x)
{
    if ((rear + 1) % size == front)
        cout << "Queue is full\n";
    else
    {
        rear = (rear + 1) % size;
        Q[rear] = x;
    }
}

int Queue::dequeue()
{
    int x = -1;
    if (front == rear)
        cout << "Queue is empty\n";
    else
    {
        front = (front + 1) % size;
        x = Q[front];
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

int main()
{
    Queue q(5);

    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);

    q.display();

    cout << "Dequeued: " << q.dequeue() << endl;

    q.display();

    return 0;
}