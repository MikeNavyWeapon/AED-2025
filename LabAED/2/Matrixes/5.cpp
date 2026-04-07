#include <iostream>
using namespace std;

class Element
{
    public:
    int i;
    int j;
    int x;
};

class Sparse
{
    private:
    int m;
    int n;
    int num;
    Element *ele;
    
    public:
    Sparse(int m,int n,int num)
    {
        this->m=m;
        this->n=n;
        this->num=num;
        ele=new Element[this->num];
    }
    ~Sparse()
    {
        delete []ele;
    }
    friend istream & operator>>(istream &is, Sparse &s)
    friend ostream & operator<<(ostream &os, Sparse &s)
    
};
Sparse Sparse::operator+(Sparse &s)
{
    Sparse *sum=new Sparse(m,n,num+s.num);
    int i, j, k;
    i=j=k=0;

    while(i<num && j<s.num)
    {
        if(ele[i].i<s.ele[j].i || (ele[i].i==s.ele[j].i && ele[i].j<s.ele[j].j))
            sum->ele[k++]=ele[i++];
        else if(ele[i].i>s.ele[j].i || (ele[i].i==s.ele[j].i && ele[i].j>s.ele[j].j))
            sum->ele[k++]=s.ele[j++];
        else
        {
            if (ele[i].j<s.ele[j].j)
                sum->ele[k++]=ele[i++];
            else if(ele[i].j>s.ele[j].j)
                sum->ele[k++]=s.ele[j++];
            else
            {
                sum->ele[k]=ele[i];
                sum->ele[k++].x=ele[i++].x+s.ele[j++].x;
            }
        }
    }
    for (;i<num;i++)
                sum->ele[k++]=ele[i];
            for (;j<s.num;j++) 
                sum->ele[k++]=s.ele[j];
    return *sum;
}


istream & operator>>(istream &is, Sparse &s)
{    cout<<"Enter the non-zero elements: \n";
    for(int i=0;i<s.num;i++)
    {
        is>>s.ele[i].i>>s.ele[i].j>>s.ele[i].x;
    }   return is;              
}
ostream & operator<<(ostream &os, Sparse &s)
{    int k=0;
    for (int i=0;i<s.m;i++)
    {
        for(int j=0;j<s.n;j++)
        {
            if (s.ele[k].i==i && s.ele[k].j==j)
                os<<s.ele[k++].x<<" ";
            else
                os<<"0 ";
        }
        os<<endl;
    }   return os;          
}   


    void read()
    {
        cout<<"Enter the non-zero elements: \n";
        for(int i=0;i<num;i++)
        {
            cin>>ele[i].i>>ele[i].j>>ele[i].x;
        }
    }
    void display()
    {
        for (int i=0;i<m;i++)
        {
            for(int j=0;j<n;j++)
            {
                if (ele[k].i==i && ele[k].j==j)
                    cout<<ele[k++].x<<" ";
                else
                    cout<<"0 ";
            }
            cout<<endl;
        }
    }
};

int main()
{
    Sparse s1(5,5,5);
    Sparse s2(5,5,5);
    
    cin>>s1;
    cin>>s2;

    Sparse sum=s1+s2;

    cout<<"First matrix: \n"<<s1;
    cout<<"Second matrix: \n"<<s2;
    cout<<"Sum of the two matrices: \n"<<sum;
    return 0;
}