#include <iostream>

class LowerTri
{
private:
    int *A;
    int n;
public:
    LowerTri()
    {
        n=2;
        A=new int[n];
    }
    LowerTri(int n)
    {
        this->n=n;
        A=new int[this->n];
    }
    ~LowerTri()
    {
        delete []A;
    }
    void Set(int i,int j,int x)
    {
        if(i>=j)
            A[i*(i-1)/2+j-1]=x;
    }
    int Get(int i,int j)
    {
        if(i>=j)
            return A[i*(i-1)/2+j-1];
        else
            return 0;
    }
    void Display()
    {
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                if(i>=j)
                    std::cout<<A[i*(i-1)/2+j-1]<<" ";
                else
                    std::cout<<"0 ";
            }
            std::cout<<"\n";
        }
    }
};

void LowerTri::Set(int i, int j, int x)
{
    if(i>=j)
        A[i*(i-1)/2+j-1]=x;
}
int LowerTri::Get(int i, int j)
{
    if(i>=j)
        return A[i*(i-1)/2+j-1];
    else
        return 0;
}
void LowerTri::Display()
{
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            if(i>=j)
                std::cout<<A[i*(i-1)/2+j-1]<<" ";
            else
                std::cout<<"0 ";
        }
        std::cout<<"\n";
    }
}

int main()
{
    LowerTri d(4);
    d.Set(1,1,5);
    d.Set(2,2,8);
    d.Set(3,3,9);
    d.Set(4,4,6);
    d.Display();
    return 0;
}