#include <iostream>

class Stack
{
    int data;
    Node *next;
};

class Stack
{
    private:
    Node *top;
    public:
    Stack(){top=NULL;}
    void push(int x);
    int pop();
    void Display();
};

void Stack::push(int x)
{
    Node *t=new Node;
    if(t==NULL)
        std::cout<<"Stack overflow\n";
    else
    {
        t->data=x;
        t->next=top;
        top=t;
    }
}

int Stack::pop()
{
    int x=-1;
    if(top==NULL)
        cout<<"Stack underflow\n";
    else
    {
        t=top;
        x=t->data;
        top=top->next;
        delete t;
    }
    return x;
}

void Stack::Display()
{
    Node *p=top;
    while(p)
    {
        std::cout<<p->data<<" ";
        p=p->next;
    }
    std::cout<<"\n";
}

int main()
{
    Stack st;
    st.push(10);
    st.push(20);
    st.push(30);
    st.Display();
    std::cout<<st.pop()<<"\n";
    st.Display();
    return 0;
}   