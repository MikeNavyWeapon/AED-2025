#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class Rectangle
{
    private:
    int length;
    int breadth;
    public:
    Rectangle()
    {
        length=0;
        breadth=0;
    }
    Rectangle(int l,int b)
    {
        length=l;
        breadth=b;
    }
    int area()
    {
        return length*breadth;
    }
    int perimeter()
    {
        
        return 2*(length+breadth);
    }
    void setLength(int l)
    {
        length=l;
    }
    void setBreadth(int b)
    {
        breadth=b;
    }
    int getLength()
    {
        return length;
    }
    int getBreadth()
    {
        return breadth;
    }
    ~Rectangle()
    {
        cout<<"Destructor";
    }
};

int main()
{
    Rectangle r1(10,5);
    cout<<"Area is: "<<r1.area()<<endl;
    cout<<"Perimeter is: "<<r1.perimeter()<<endl;
    return 0;

}
