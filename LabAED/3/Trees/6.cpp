#include <stdio.h>

void Insert(int H[], int n)
{
    int i=0;
    while(H[i]!=0)
    {
        i++;
    }
    H[i]=n;

    int temp, parent;
    while(i>1)
    {
        parent=i/2;
        if(H[parent]<H[i])
        {
            temp=H[parent];
            H[parent]=H[i];
            H[i]=temp;
            i=parent;
        }
        else
        {
            return;
        }
    }
}

int Delete(int A[],int n)
{
    int x=-1;
    if(n==0)
    {
        printf("Heap is empty\n");
        return x;
    }
    x=A[1];
    A[1]=A[n];
    A[n]=0;

    int i=1, j=2*i;
    while(j<n-1)
    {
        if(A[j]<A[j+1])
        {
            j=j+1;
        }
        if(A[i]<A[j])
        {
            int temp=A[i];
            A[i]=A[j];
            A[j]=temp;
            i=j;
            j=2*i;
        }
        else
        {
            break;
        }
    }
    return x;
}

int main()
{
    int H[]={0,2,5,8,9,4,10,7};
    Insert(H, 6);
    return 0;
}